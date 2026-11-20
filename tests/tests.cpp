#include "revoice/Processor.h"
#include "revoice/YinDetector.h"
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
void yin() {
    for(double rate:{8000.,44100.,48000.,96000.,192000.}) for(double hz:{55.,82.41,110.,220.,440.,880.,1396.}) {
        YinDetector detector(rate,40,1400,.003);
        std::uint32_t seed=7;
        for(int i=0;i<int(rate*.18);++i) {
            seed=1664525u*seed+1013904223u;
            const double t=i/rate,phase=TWOPI*hz*t;
            const double noise=(double(seed)/4294967296.-.5)*.004;
            detector.process(.13+(.8+.2*std::sin(TWOPI*3*t))*(.07*std::sin(phase)+.1*std::sin(2*phase)+.06*std::sin(3*phase))+noise);
        }
        near(detector.frequency(),hz,std::max(1.0,hz*.012),"YIN harmonic-rich voice at "+std::to_string(rate)+" Hz");
        require(detector.confidence()>.8,"voiced YIN estimate is periodic");
    }
    YinDetector detector(48000,40,1400,.003);
    auto tone=[&](double hz,double amplitude) {
        for(int i=0;i<7200;++i) detector.process(amplitude*std::sin(TWOPI*hz*i/48000));
    };
    tone(110,.02); near(detector.frequency(),110,1,"quiet microphone pitch");
    tone(330,.02); near(detector.frequency(),330,2,"YIN follows changed input pitch");
    for(int i=0;i<7200;++i) detector.process(0);
    near(detector.frequency(),0,0,"silence clears pitch instead of displaying a stale note");
    for(int i=0;i<7200;++i) detector.process(.2);
    near(detector.frequency(),0,0,"DC offset is unvoiced");
    tone(220,.0001); near(detector.frequency(),0,0,"RMS gate rejects very quiet input");
    detector.gate(.00001); tone(220,.0001); near(detector.frequency(),220,1,"lowering the gate admits a quiet microphone");
    std::uint32_t seed=7;
    for(int i=0;i<12000;++i) {seed=1664525u*seed+1013904223u;detector.process((double(seed)/4294967296.-.5)*.2);}
    near(detector.frequency(),0,0,"unvoiced noise has no pitch");
    detector.reset(); near(detector.frequency(),0,0,"reset clears YIN history");
    for(int i=0;i<7200;++i) detector.process(i%2?std::numeric_limits<double>::infinity():std::numeric_limits<double>::quiet_NaN());
    near(detector.frequency(),0,0,"non-finite input is contained");
    rejects([]{YinDetector invalid(48000,0,1400);},"invalid YIN range rejected");
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
void harmonylive() {
    for(double rate:{44100.,48000.}) for(int voice=0;voice<4;++voice) {
        Parameters p; p.set(Param::SourceLevel,0); p.set(Param::ReverbMix,0); p.set(Param::Logic,0);
        for(int v=0;v<4;++v) p.set(voiceParam(v,VoiceParam::Enabled),v==voice);
        const int intervals[]={4,7,12,-12}; const int interval=intervals[voice];
        p.set(voiceParam(voice,VoiceParam::Semitones),interval);
        p.set(voiceParam(voice,VoiceParam::Highpass),0);
        Processor processor(rate,p);
        constexpr int count=8192;
        std::vector<float> audio; audio.reserve(std::size_t(rate));
        std::array<float,514> input{},output{};
        std::size_t position=0;
        std::uint32_t seed=7;
        while(position<std::size_t(rate)) {
            const std::size_t frames=std::min<std::size_t>(voice==0?64:257,std::size_t(rate)-position);
            // Channel 1 is silent; the microphone is on channel 2. This also
            // exercises deinterleaving used by multi-input audio interfaces.
            for(std::size_t i=0;i<frames;++i) {
                const double phase=TWOPI*220*(position+i)/rate;
                seed=1664525u*seed+1013904223u;
                const double sample=voice==1?.012*std::sin(phase)+.02*std::sin(2*phase)+.015*std::sin(3*phase)+.001*(double(seed)/4294967296.-.5):.02*std::sin(phase);
                input[2*i]=0;input[2*i+1]=float(sample);
            }
            processor.process(input.data()+1,output.data(),frames,2);
            for(std::size_t i=0;i<frames;++i) audio.push_back(output[2*i]+output[2*i+1]);
            position+=frames;
        }
        near(processor.pitch(),220,2,"quiet live input informs harmony voice "+std::to_string(voice+1));
        auto power=[&](double hz) {
            double real=0,imag=0;
            for(int i=0;i<count;++i) {
                const double value=audio[audio.size()-count+i]*(.5-.5*std::cos(TWOPI*i/count));
                real+=value*std::cos(TWOPI*hz*i/rate);imag+=value*std::sin(TWOPI*hz*i/rate);
            }
            return real*real+imag*imag;
        };
        const double expected=220*std::pow(2.0,interval/12.0);
        const double targetPower=power(expected),sourcePower=power(220);
        const auto context=" voice "+std::to_string(voice+1)+" at "+std::to_string(rate)+" Hz; target "+std::to_string(targetPower)+", original "+std::to_string(sourcePower);
        require(targetPower>.05,"isolated harmony voice has audible energy at its requested interval"+context);
        if(interval>0) require(targetPower>4*sourcePower,"harmony contains the shifted voice with the original muted"+context);
        // A formant-preserving octave down retains the original frequency as
        // its second harmonic; test the new fundamental against nearby bins.
        else require(targetPower>20*std::max(power(expected-30),power(expected+30)),"octave-down fundamental is resolved"+context);
        std::fill(input.begin(),input.end(),0);
        for(int block=0;block<60;++block) processor.process(input.data()+1,output.data(),257,2);
        near(processor.pitch(),0,0,"harmonizer pitch display clears when live input stops");
    }
}
void instances() {
    Parameters p;p.set(Param::Source,1);p.set(Param::SourceLevel,1);p.set(Param::HarmonyLevel,0);
    p.set(Param::AntiRumble,0);p.set(Param::ReverbMix,1);
    std::vector<float> warm(24000),expected(24000),actual(24000),other(24000);
    {
        Processor baseline(48000,p);baseline.process(nullptr,warm.data(),12000);
        p.set(Param::ReverbTime,4);baseline.apply(p);baseline.process(nullptr,expected.data(),12000);
    }
    p.set(Param::ReverbTime,2.5);
    Processor first(48000,p);first.process(nullptr,warm.data(),12000);
    Processor second(44100,p);second.process(nullptr,other.data(),12000);
    p.set(Param::ReverbTime,4);first.apply(p);first.process(nullptr,actual.data(),12000);
    for(std::size_t i=0;i<actual.size();++i) near(actual[i],expected[i],1e-7,"another sample-rate instance must not change reverb decay");

    Parameters dry;dry.set(Param::SourceLevel,1);dry.set(Param::HarmonyLevel,0);dry.set(Param::ReverbMix,0);dry.set(Param::AntiRumble,0);
    Processor impulse(48000,dry);std::array<float,2048> input{};std::array<float,4096> output{};input[0]=.5f;
    impulse.process(input.data(),output.data(),input.size());
    std::size_t onset=0;while(onset<input.size() && output[onset*2]==0) ++onset;
    require(onset==impulse.latencySamples(),"reported latency matches actual dry impulse onset");
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
    p.set(Param::OutputGain,.7);p.set(Param::Logic,7);p.set(Param::Scale,4);p.set(Param::MidiMode,2);p.set(Param::YinGate,.001);
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
        if(suite=="windows") windows();else if(suite=="frames") frames();else if(suite=="filters") filters();else if(suite=="detectors") detectors();else if(suite=="yin") yin();
        else if(suite=="processing") processing();else if(suite=="pitchshift") pitchshift();else if(suite=="harmonylive") harmonylive();else if(suite=="instances") instances();else if(suite=="wav") wav();else if(suite=="queue") queue();
        else if(suite=="realtime") realtime();else if(suite=="recording") recording();else throw std::runtime_error("Unknown test suite");
        std::cout<<suite<<" passed\n";return 0;
    }catch(const std::exception& error){trackAllocations=false;std::cerr<<error.what()<<'\n';return 1;}
}
