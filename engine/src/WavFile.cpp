#include "revoice/WavFile.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace revoice {
namespace {
std::uint16_t u16(const unsigned char* p) { return std::uint16_t(p[0]) | std::uint16_t(p[1])<<8; }
std::uint32_t u32(const unsigned char* p) {
    return std::uint32_t(p[0]) | std::uint32_t(p[1])<<8 | std::uint32_t(p[2])<<16 | std::uint32_t(p[3])<<24;
}
void put16(std::ostream& out,std::uint16_t value) {
    const char b[]={static_cast<char>(value&255),static_cast<char>((value>>8)&255)};
    out.write(b,2);
}
void put32(std::ostream& out,std::uint32_t value) { put16(out,value&65535); put16(out,value>>16); }
void readExact(std::istream& in,unsigned char* p,std::size_t n) {
    if(!in.read(reinterpret_cast<char*>(p),static_cast<std::streamsize>(n)))
        throw std::runtime_error("Truncated WAV file");
}
} // namespace

AudioFile readWav(const std::filesystem::path& path) {
    std::ifstream in(path,std::ios::binary);
    if(!in) throw std::runtime_error("Cannot open WAV file");
    in.seekg(0,std::ios::end);
    const auto length=in.tellg();
    in.seekg(0);
    std::array<unsigned char,12> riff{};
    readExact(in,riff.data(),riff.size());
    if(std::memcmp(riff.data(),"RIFF",4) || std::memcmp(riff.data()+8,"WAVE",4))
        throw std::runtime_error("Expected a little-endian RIFF/WAVE file");
    const std::uint64_t riffEnd=std::uint64_t(u32(riff.data()+4))+8;
    if(length<12 || riffEnd>static_cast<std::uint64_t>(length) || riffEnd<12)
        throw std::runtime_error("Invalid WAV file size");
    std::uint16_t format=0,channels=0,bits=0,align=0;
    std::uint32_t rate=0,dataSize=0;
    std::uint64_t dataOffset=0;
    for(std::uint64_t offset=12;offset+8<=riffEnd;) {
        in.seekg(static_cast<std::streamoff>(offset));
        std::array<unsigned char,8> chunk{};
        readExact(in,chunk.data(),chunk.size());
        const std::uint64_t size=u32(chunk.data()+4),begin=offset+8;
        if(size>riffEnd-begin) throw std::runtime_error("WAV chunk exceeds file bounds");
        if(!std::memcmp(chunk.data(),"fmt ",4)) {
            if(size<16) throw std::runtime_error("Invalid WAV format chunk");
            std::array<unsigned char,40> fmt{};
            readExact(in,fmt.data(),static_cast<std::size_t>(std::min<std::uint64_t>(size,40)));
            format=u16(fmt.data()); channels=u16(fmt.data()+2); rate=u32(fmt.data()+4);
            align=u16(fmt.data()+12); bits=u16(fmt.data()+14);
            if(format==0xfffe && size>=40) {
                // WAVE_FORMAT_EXTENSIBLE: PCM or IEEE float with the standard GUID.
                static constexpr unsigned char tail[]={0,0,0,0,0x10,0,0x80,0,0,0xaa,0,0x38,0x9b,0x71};
                if(u16(fmt.data()+16)<22 || std::memcmp(fmt.data()+26,tail,sizeof(tail)))
                    throw std::runtime_error("Unsupported extensible WAV format");
                format=u16(fmt.data()+24);
                if(u16(fmt.data()+18)!=0 && u16(fmt.data()+18)!=bits)
                    throw std::runtime_error("WAV valid bits must match the container size");
            }
        } else if(!std::memcmp(chunk.data(),"data",4) && !dataOffset) {
            dataOffset=begin; dataSize=static_cast<std::uint32_t>(size);
        }
        offset=begin+size+(size&1);
    }
    const bool pcm=format==1 && (bits==8 || bits==16 || bits==24 || bits==32);
    const bool floating=format==3 && (bits==32 || bits==64);
    if(!dataOffset || !channels || channels>32 || rate<8000 || rate>384000 ||
       (!pcm && !floating) || align!=channels*(bits/8) || dataSize%align)
        throw std::runtime_error("Unsupported or malformed WAV encoding");
    const auto frames=std::size_t(dataSize/align);
    // Bound both decoded memory and loading time for this desktop application.
    if(!frames || frames>100000000) throw std::runtime_error("WAV must contain 1 to 100 million frames");
    AudioFile result; result.sampleRate=rate; result.mono.resize(frames);
    in.seekg(static_cast<std::streamoff>(dataOffset));
    std::array<unsigned char,256> frame{};
    for(std::size_t i=0;i<frames;++i) {
        readExact(in,frame.data(),align);
        double sum=0;
        for(unsigned ch=0;ch<channels;++ch) {
            const auto* b=frame.data()+ch*(bits/8);
            double value=0;
            if(pcm) {
                if(bits==8) value=(int(b[0])-128)/128.0;
                else if(bits==16) value=static_cast<std::int16_t>(u16(b))/32768.0;
                else if(bits==24) {
                    std::int32_t v=std::int32_t(b[0]) | std::int32_t(b[1])<<8 | std::int32_t(b[2])<<16;
                    if(v&0x800000) v-=0x1000000;
                    value=v/8388608.0;
                } else value=static_cast<std::int32_t>(u32(b))/2147483648.0;
            } else if(bits==32) {
                const auto raw=u32(b); float v; std::memcpy(&v,&raw,4); value=v;
            } else {
                const std::uint64_t raw=std::uint64_t(u32(b)) | std::uint64_t(u32(b+4))<<32;
                double v; std::memcpy(&v,&raw,8); value=v;
            }
            sum+=std::isfinite(value)?std::clamp(value,-1.0,1.0):0;
        }
        result.mono[i]=static_cast<float>(sum/channels);
    }
    return result;
}

WavWriter::WavWriter(const std::filesystem::path& path,std::uint32_t rate,std::uint16_t channels)
    : sampleRate_(rate),channels_(channels) {
    if(rate<8000 || rate>384000 || channels<1 || channels>2)
        throw std::runtime_error("Invalid WAV recording format");
    file_.open(path,std::ios::binary|std::ios::trunc);
    if(!file_) throw std::runtime_error("Cannot create WAV output file");
    header();
}
WavWriter::~WavWriter() { try { close(); } catch(...) {} }
void WavWriter::header() {
    file_.seekp(0);
    const auto bytes=static_cast<std::uint32_t>(samples_*2);
    file_.write("RIFF",4); put32(file_,36+bytes); file_.write("WAVEfmt ",8);
    put32(file_,16); put16(file_,1); put16(file_,channels_); put32(file_,sampleRate_);
    put32(file_,sampleRate_*channels_*2); put16(file_,channels_*2); put16(file_,16);
    file_.write("data",4); put32(file_,bytes);
    if(!file_) throw std::runtime_error("Cannot write WAV header");
}
void WavWriter::write(const float* samples,std::size_t count) {
    if(!file_.is_open()) throw std::runtime_error("WAV output is closed");
    if(count%channels_ || samples_+count>(std::numeric_limits<std::uint32_t>::max()-36)/2)
        throw std::runtime_error("WAV output exceeds the RIFF 4 GB limit");
    std::array<char,8192> bytes{};
    for(std::size_t offset=0;offset<count;) {
        const auto n=std::min<std::size_t>(bytes.size()/2,count-offset);
        for(std::size_t i=0;i<n;++i) {
            const float sample=samples[offset+i];
            const double value=std::isfinite(sample)?std::clamp(double(sample),-1.0,1.0):0;
            const auto pcm=static_cast<std::int16_t>(std::lround(value*(value<0?32768:32767)));
            const auto raw=static_cast<std::uint16_t>(pcm);
            bytes[2*i]=static_cast<char>(raw&255); bytes[2*i+1]=static_cast<char>(raw>>8);
        }
        file_.write(bytes.data(),static_cast<std::streamsize>(2*n));
        if(!file_) throw std::runtime_error("Cannot write WAV samples (disk may be full)");
        samples_+=n; offset+=n;
    }
}
void WavWriter::close() {
    if(file_.is_open()) { header(); file_.flush(); const bool ok=bool(file_); file_.close();
        if(!ok) throw std::runtime_error("Cannot finalize WAV recording"); }
}
} // namespace revoice
