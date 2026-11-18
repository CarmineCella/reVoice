#include "revoice/Processor.h"
#include "revoice/SpscQueue.h"
#include "revoice/Recorder.h"
#include "SignalTable.h"
#include "SoundFrame.h"
#include "PeriodEstimator.h"
#include "SpectralEstimator.h"
#include "RandomGenerators.h"
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

thread_local bool trackAllocations=false;
thread_local std::size_t allocationCount=0;
void* operator new(std::size_t n) { if(trackAllocations) ++allocationCount; if(auto* p=std::malloc(n?n:1)) return p; throw std::bad_alloc(); }
void* operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept {std::free(p);}
void operator delete[](void* p) noexcept {std::free(p);}
void operator delete(void* p,std::size_t) noexcept {std::free(p);}
void operator delete[](void* p,std::size_t) noexcept {std::free(p);}

namespace {
void require(bool ok,const std::string& text) {if(!ok) throw std::runtime_error(text);}
void near(double a,double b,double tolerance,const std::string& what) {require(std::abs(a-b)<=tolerance,what+": expected "+std::to_string(b)+", got "+std::to_string(a));}
template<class F> void rejects(F f,const char* what) {bool threw=false;try{f();}catch(const std::exception&){threw=true;}require(threw,what);}
using namespace revoice;
void windows() {
    for(int n=1;n<=513;++n) {
        SignalTable t(n);
        t.hanning(1); t.hamming(1); t.blackman(1); t.blackmanHarris(1); t.gauss(1); t.bartlett(1);
        for(int i=0;i<n;++i) require(std::isfinite(t[i]),"finite window samples");
        t.clear(); t.rectangle(1);
        for(int i=0;i<n;++i) near(t[i],1,0,"rectangle endpoint");
        t.set(-1,.5); near(t[n-1],.5,0,"negative circular write"); near(t[-1],.5,0,"negative circular read");
        t.delay(-1); t.set(.1); t.delay(2); t.set(.2);
        rejects([&]{t.drawSegment(-1,0,n,1,0);},"invalid segment must fail");
    }
}
void frames() {
    SoundFrame a(true),b(true); a.set(Pandora::LEFT,.5);a.set(Pandora::RIGHT,.25);
    b.set(Pandora::LEFT,.25);b.set(Pandora::RIGHT,.5);
    auto c=a/b; near(c.get(Pandora::LEFT),2,0,"frame division left"); near(c.get(Pandora::RIGHT),.5,0,"frame division right");
    SoundFrame copy(a); require(copy.isStereo(),"copy preserves stereo flag"); near(copy.get16bit(Pandora::LEFT),16383,1,"copy initializes PCM scale");
    SoundFrame assigned; assigned=a; require(assigned.isStereo(),"assignment preserves stereo flag");
    SoundInfo info; rejects([&]{info.samplingRate(0);},"zero rate rejected");
    rejects([&]{info.samplingRate(std::numeric_limits<double>::quiet_NaN());},"NaN rate rejected");
}
void filters() {
    PRNG_31bit random;random.seed(1);
    for(long expected:{16807L,282475249L,1622650073L,984943658L,1144108930L})
        require(random.fastGenerate()==expected,"portable 31-bit random sequence");
    SoundInfo info;
    for(double q:{.1,.3,.707,2.}) for(double cutoff:{80.,1396.,4000.,20000.}) {
        VariState filter(&info,cutoff,q);
        for(int i=0;i<44100;++i) {
            double sample=.35*std::sin(TWOPI*220*i/44100);
            const double out=filter.process(sample);
            require(std::isfinite(out) && std::abs(out)<10,"stable filter over exposed cutoff and Q ranges");
        }
    }
}
void detectors() {
    for(double rate:{44100.,48000.,96000.}) for(int kind:{0,1}) {
        SoundInfo info; info.samplingRate(rate);
        std::unique_ptr<AbstractPitchDetector> detector;
        if(kind==0) detector=std::make_unique<PeriodEstimator>(&info,77.782,1396.913,2,30,.03);
        else detector=std::make_unique<SpectralEstimator>(&info,1024,77.782,1396.913,2,30,1,.03);
        for(int i=0;i<int(rate);++i) {double input=.35*std::sin(TWOPI*220*i/rate);detector->process(input);}
        near(detector->get(),220,5,"220 Hz pitch detection at "+std::to_string(rate)+" detector "+std::to_string(kind));
    }
}
void processing() {
    Parameters parameters; parameters.set(Param::Source,1);
    std::array<float,514> block{};
    for(int detector:{0,1}) {
        parameters.set(Param::Detector,detector);
        Processor processor(48000,parameters);
        double energy=0;
        for(int n=0;n<250;++n) {
            std::fill(block.begin(),block.end(),std::numeric_limits<float>::quiet_NaN());
            processor.process(nullptr,block.data(),257);
            for(float sample:block) {require(std::isfinite(sample) && std::abs(sample)<=1,"bounded, fully written audio");energy+=sample*sample;}
        }
        require(energy>1,"harmonizer produces audible energy");
        near(processor.pitch(),220,5,"processor pitch");
        parameters.set(Param::MorphLevel,.1); parameters.set(Param::MorphRandomFrequency,.7);
        parameters.set(Param::MorphReverse,1); parameters.set(Param::MorphSpeed,-2);
        processor.apply(parameters);
        for(int n=0;n<100;++n) processor.process(nullptr,block.data(),257);
        for(float sample:block) require(std::isfinite(sample),"finite granular output");
    }
    Parameters live; live.set(Param::ReverbMix,0);
    Processor silence(48000,live); silence.process(nullptr,block.data(),257);
    for(float sample:block) near(sample,0,0,"null live input is silence");
    std::array<float,257> bad{};bad[0]=std::numeric_limits<float>::infinity();bad[1]=std::numeric_limits<float>::quiet_NaN();
    silence.process(bad.data(),block.data(),257);
    for(float sample:block) require(std::isfinite(sample),"invalid source samples contained");
    Parameters chords;chords.set(Param::Source,1);Processor harmony(48000,chords);
    for(int mode=0;mode<=10;++mode) for(int scale=0;scale<6;++scale) {
        chords.set(Param::Logic,mode);chords.set(Param::Scale,scale);harmony.apply(chords);
        for(int i=0;i<20;++i) harmony.process(nullptr,block.data(),257);
        for(float sample:block) require(std::isfinite(sample),"finite output for every harmony and scale mode");
    }
}
double dominantFrequency(Processor& processor,double low,double high) {
    std::vector<float> output(96000);processor.process(nullptr,output.data(),48000);
    constexpr int count=8192;std::array<double,count> tail{};
    for(int i=0;i<count;++i) tail[i]=output[2*(48000-count+i)]*(.5-.5*std::cos(TWOPI*i/count));
    double peak=0,frequency=0;
    for(double hz=low;hz<=high;hz+=.5) {
        const double c=std::cos(TWOPI*hz/48000),s=std::sin(TWOPI*hz/48000);
        double x=1,y=0,real=0,imag=0;
        for(double sample:tail) {real+=sample*x;imag+=sample*y;const double next=x*c-y*s;y=y*c+x*s;x=next;}
        const double power=real*real+imag*imag;if(power>peak){peak=power;frequency=hz;}
    }
    require(peak>1,"shifted voice has nonzero spectral energy");return frequency;
}
void pitchshift() {
    Parameters p;p.set(Param::Source,1);p.set(Param::SourceLevel,0);p.set(Param::ReverbMix,0);p.set(Param::Logic,0);
    for(int voice=0;voice<4;++voice){p.set(voiceParam(voice,VoiceParam::Enabled),voice==0);p.set(voiceParam(voice,VoiceParam::Semitones),0);}
    p.set(Param::Transpose,12);Processor processor(48000,p);
    near(dominantFrequency(processor,90,500),440,3,"octave-up output frequency");
    p.set(Param::Transpose,0);p.set(Param::MidiMode,1);processor.apply(p);processor.midi(0x90,69,100);
    p.set(Param::SourceFrequency,330);processor.apply(p);
    near(dominantFrequency(processor,200,500),440,3,"MIDI holds target pitch while input pitch changes");
    processor.midi(0x90,72,100);processor.midi(0x80,69,0);
    near(dominantFrequency(processor,400,600),523.25,4,"releasing an older MIDI note preserves the held note");
    processor.midi(0x80,72,0);
    near(dominantFrequency(processor,250,450),330,3,"MIDI release restores relative harmonization");
}
struct TempFile {
    std::filesystem::path path=std::filesystem::temp_directory_path()/
        ("revoice-test-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())+".wav");
    ~TempFile(){std::error_code ignored;std::filesystem::remove(path,ignored);}
};
void wav() {
    TempFile temp;
    const float samples[]={-1,1,.5f,.25f,0,0,1,1};
    {WavWriter writer(temp.path,48000);writer.write(samples,8);writer.close();}
    auto read=readWav(temp.path); require(read.sampleRate==48000 && read.mono.size()==4,"WAV format and frame count");
    near(read.mono[0],0,1./32768,"WAV stereo downmix"); near(read.mono[1],.375,1./32768,"WAV quantization");near(read.mono[3],1,1./32768,"WAV positive full scale");
    {std::ofstream corrupt(temp.path,std::ios::binary|std::ios::trunc);corrupt<<"RIFF";}
    rejects([&]{readWav(temp.path);},"truncated WAV rejected");
    {std::ofstream corrupt(temp.path,std::ios::binary|std::ios::trunc);
     const unsigned char bytes[]={82,73,70,70,12,0,0,0,87,65,86,69,102,109,116,32,255,255,255,127};
     corrupt.write(reinterpret_cast<const char*>(bytes),sizeof(bytes));}
    rejects([&]{readWav(temp.path);},"oversized WAV chunk rejected");
}
void queue() {
    SpscQueue<unsigned,64> queue;
    for(unsigned i=0;i<63;++i) require(queue.push(i),"queue accepts capacity-1 items");
    require(!queue.push(99),"queue reports full"); unsigned value=0;
    for(unsigned i=0;i<63;++i) {require(queue.pop(value),"queue pops item");require(value==i,"queue preserves ordering");}
    require(!queue.pop(value),"queue reports empty");
    std::atomic<bool> failed{false};
    std::thread producer([&]{for(unsigned i=0;i<200000;++i) while(!queue.push(i)) std::this_thread::yield();});
    for(unsigned i=0;i<200000;++i) {while(!queue.pop(value)) std::this_thread::yield();if(value!=i) failed=true;}
    producer.join();require(!failed,"concurrent SPSC ordering");
}
void recording() {
    TempFile temp;Recorder recorder;
    std::array<float,256> block{};
    for(std::size_t i=0;i<block.size();i+=2){block[i]=.25f;block[i+1]=.75f;}
    for(int take=0;take<3;++take) {
        recorder.start(temp.path,48000);
        allocationCount=0;trackAllocations=true;
        for(int i=0;i<100;++i) recorder.push(block.data(),128);
        trackAllocations=false;recorder.stop();
        require(allocationCount==0,"recording callback must not allocate");
        require(!recorder.failed() && recorder.droppedFrames()==0,"recording worker writes without loss");
        const auto file=readWav(temp.path);require(file.mono.size()==12800,"stop drains and finalizes every recorded frame");
        near(file.mono.back(),.5,1./32768,"recorded sample value");
    }
}
void realtime() {
    Parameters p;p.set(Param::Source,1);Processor processor(48000,p);
    std::array<float,256> block{};
    processor.process(nullptr,block.data(),128);
    p.set(Param::OutputGain,.7);p.set(Param::Logic,7);p.set(Param::Scale,4);p.set(Param::MidiMode,2);
    allocationCount=0;trackAllocations=true;
    processor.apply(p);processor.midi(0x90,60,100);
    for(int i=0;i<400;++i) processor.process(nullptr,block.data(),128);
    processor.midi(0x80,60,0);processor.midi(0xb0,123,0);
    trackAllocations=false;
    require(allocationCount==0,"real-time processing and parameter changes must not allocate");
}
} // namespace
int main(int argc,char** argv) {
    try {
        const std::string suite=argc>1?argv[1]:"";
        if(suite=="windows") windows();else if(suite=="frames") frames();else if(suite=="filters") filters();else if(suite=="detectors") detectors();
        else if(suite=="processing") processing();else if(suite=="pitchshift") pitchshift();else if(suite=="wav") wav();else if(suite=="queue") queue();
        else if(suite=="realtime") realtime();else if(suite=="recording") recording();else throw std::runtime_error("Unknown test suite");
        std::cout<<suite<<" passed\n";return 0;
    }catch(const std::exception& error){trackAllocations=false;std::cerr<<error.what()<<'\n';return 1;}
}
