#include "revoice/Processor.h"
#include "revoice/YinDetector.h"
#include "Harmonizer.h"
#include "SpectralEstimator.h"
#include "InstanceReverb.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

namespace revoice {
namespace {
// The harmonizer advances this shared detector once per input sample. Each
// pitch shifter reads get(), so analysis and audible harmony use the same F0.
class YinPitchAdapter final : public AbstractPitchDetector {
    YinDetector yin_;
public:
    YinPitchAdapter(SoundInfo* info,double low,double high,double gate)
        : AbstractPitchDetector(info),yin_(info->samplingRate(),low,high,gate) { m_val=0; }
    double& process(double& sample) override { m_val=yin_.process(sample); return m_val; }
    double get() const override { return yin_.frequency(); }
    double min() const override { return yin_.minimum(); }
    double max() const override { return yin_.maximum(); }
    void threshold(double rms) override { yin_.gate(rms); }
    void reset() override { yin_.reset(); m_val=0; }
    void smoothingRate(double) override {}
    void smoothingQuality(double) override {}
    void update(Subject*) override { reset(); } // Rate changes construct a fresh Processor.
};
}
struct Processor::Impl {
    SoundInfo info;
    SignalTable table{262144};
    std::unique_ptr<YinPitchAdapter> yin;
    std::unique_ptr<SpectralEstimator> spectral;
    std::unique_ptr<Harmonizer> harmony;
    std::unique_ptr<TableGranulator> morph;
    std::unique_ptr<InstanceReverb> reverbLeft,reverbRight;
    Parameters parameters;
    AudioFile file;
    std::array<int,4> notes{{-1,-1,-1,-1}};
    std::array<unsigned char,4> velocities{};
    std::array<std::uint64_t,128> fundamentalNotes{};
    std::uint64_t noteOrder=0;
    double rate=48000,phase=0,filePosition=0,gain=0;
    float inputPeak=0,leftPeak=0,rightPeak=0;
    bool configured=false;
    void restoreMidi(const Parameters& p) {
        if(p[Param::MidiMode]==1) {
            int selected=-1;std::uint64_t latest=0;
            for(int note=0;note<128;++note) if(fundamentalNotes[note]>latest) {latest=fundamentalNotes[note];selected=note;}
            const double transpose=std::pow(2.0,p[Param::Transpose]/12);
            harmony->fundamental(selected<0?transpose:440*std::pow(2.0,(selected-69)/12.0)*transpose,selected>=0);
        } else if(p[Param::MidiMode]==2) {
            for(int voice=0;voice<4;++voice) {
                if(notes[voice]>=0) harmony->ratios(voice,p[voiceParam(voice,VoiceParam::Level)]*velocities[voice]/127.0,
                    440*std::pow(2.0,(notes[voice]-69)/12.0),true);
                harmony->status(voice,notes[voice]>=0 && p[voiceParam(voice,VoiceParam::Enabled)]!=0);
            }
        }
    }
    explicit Impl(double sr,const Parameters& initial,const AudioFile* source) : rate(sr) {
        if(!std::isfinite(sr) || sr<8000 || sr>192000) throw std::runtime_error("Invalid processing sample rate");
        parameters=initial; parameters.sanitize();
        info.samplingRate(sr); info.channels(2);
        const double low=parameters[Param::MinimumPitch];
        const double high=std::min(parameters[Param::MaximumPitch],sr*.45);
        const int levels=int(parameters[Param::FilterLevels]);
        const double threshold=parameters[Param::Threshold];
        if(parameters[Param::Detector]==0)
            yin=std::make_unique<YinPitchAdapter>(&info,low,high,parameters[Param::YinGate]);
        else spectral=std::make_unique<SpectralEstimator>(&info,128<<int(parameters[Param::FftSize]),
            low,high,levels,30,1.0,threshold);
        AbstractPitchDetector* detector=yin?static_cast<AbstractPitchDetector*>(yin.get()):spectral.get();
        table.delay(sr*parameters[Param::DelayMs]/1000/table.size());
        harmony=std::make_unique<Harmonizer>(&info,&table,detector,0,1);
        morph=std::make_unique<TableGranulator>(&info,&table,0,1,64);
        reverbLeft=std::make_unique<InstanceReverb>(sr,parameters[Param::ReverbTime]);
        reverbRight=std::make_unique<InstanceReverb>(sr,parameters[Param::ReverbTime]);
        if(source) file=*source;
        if(parameters[Param::Source]==3 && file.mono.empty()) throw std::runtime_error("Choose a WAV file before starting playback");
    }
};
Processor::Processor(double rate,const Parameters& parameters,const AudioFile* source)
    : impl_(std::make_unique<Impl>(rate,parameters,source)) { apply(impl_->parameters); }
Processor::~Processor()=default;
void Processor::apply(const Parameters& requested) {
    auto& s=*impl_;
    Parameters p=requested; p.sanitize();
    const auto changed=[&](Param id){return !s.configured || p[id]!=s.parameters[id];};
    auto& h=*s.harmony;
    if(changed(Param::SourceLevel)) h.sourceLevel(p[Param::SourceLevel]);
    if(changed(Param::HarmonyLevel)) h.harmonyLevel(p[Param::HarmonyLevel]);
    if(changed(Param::Freeze)) h.freeze(p[Param::Freeze]!=0);
    if(changed(Param::AntiRumble)) h.antiRumble(p[Param::AntiRumble]!=0);
    if(changed(Param::Key)) h.masterKey(static_cast<Pandora::notes>(int(p[Param::Key])));
    if(changed(Param::Scale)) h.scale(static_cast<Pandora::scales>(int(p[Param::Scale])));
    if(changed(Param::Window)) h.grainEnvelope(static_cast<Pandora::windowType>(int(p[Param::Window])));
    if(changed(Param::AutoGrains)) h.autoMode(p[Param::AutoGrains]!=0);
    if(changed(Param::Cycles) || changed(Param::AutoGrains)) h.cyclesPerGrain(int(p[Param::Cycles]));
    if(changed(Param::Attack) || changed(Param::Decay)) h.attackDecay(p[Param::Attack],p[Param::Decay]);
    if(changed(Param::PitchChange)) h.freqThreshold(std::pow(2.0,p[Param::PitchChange]/12));
    bool chordChanged=changed(Param::Logic) || changed(Param::Transpose) || changed(Param::MidiMode);
    for(int i=0;i<4;++i) {
        auto v=[&](VoiceParam field){return p[voiceParam(i,field)];};
        auto vc=[&](VoiceParam field){return changed(voiceParam(i,field));};
        if(vc(VoiceParam::Enabled)) h.status(i,v(VoiceParam::Enabled)!=0 && (p[Param::MidiMode]!=2 || s.notes[i]>=0));
        if(vc(VoiceParam::Highpass)) h.highpass(i,v(VoiceParam::Highpass)!=0);
        if(vc(VoiceParam::VibratoDepth) || vc(VoiceParam::VibratoRate)) h.vibrato(i,v(VoiceParam::VibratoDepth),v(VoiceParam::VibratoRate));
        if(vc(VoiceParam::RandomVibrato)) h.randomVibrato(i,v(VoiceParam::RandomVibrato));
        if(vc(VoiceParam::Pan)) h.panning(i,v(VoiceParam::Pan));
        if(vc(VoiceParam::Formant)) h.formant(i,v(VoiceParam::Formant));
        chordChanged=chordChanged || vc(VoiceParam::Level) || vc(VoiceParam::Semitones);
    }
    if(chordChanged) {
        for(int i=0;i<4;++i) h.ratios(i,p[voiceParam(i,VoiceParam::Level)],
            std::pow(2.0,p[voiceParam(i,VoiceParam::Semitones)]/12));
        h.logic(p[Param::MidiMode]==2?Pandora::CUSTOM_CHORD:static_cast<Pandora::harmonizationLogic>(int(p[Param::Logic])));
        h.fundamental(std::pow(2.0,p[Param::Transpose]/12),false);
    }
    if(changed(Param::MidiMode)) {
        s.notes.fill(-1);
        s.fundamentalNotes.fill(0);s.velocities.fill(0);s.noteOrder=0;
        for(int i=0;i<4;++i) h.status(i,p[Param::MidiMode]==2?false:p[voiceParam(i,VoiceParam::Enabled)]!=0);
    }
    if(chordChanged && p[Param::MidiMode]!=0) s.restoreMidi(p);
    if(s.yin) {
        if(changed(Param::YinGate)) s.yin->threshold(p[Param::YinGate]);
    } else {
        if(changed(Param::Threshold)) s.spectral->threshold(p[Param::Threshold]);
        if(changed(Param::FilterQuality)) s.spectral->filterQuality(p[Param::FilterQuality]);
        if(changed(Param::SmoothingRate)) s.spectral->smoothingRate(p[Param::SmoothingRate]);
        if(changed(Param::SmoothingQuality)) s.spectral->smoothingQuality(p[Param::SmoothingQuality]);
        if(changed(Param::DetectorSplit)) s.spectral->split(p[Param::DetectorSplit]);
    }
    auto& m=*s.morph;
    if(changed(Param::MorphDensity) || changed(Param::MorphRandomDensity)) m.density(p[Param::MorphDensity],p[Param::MorphRandomDensity]);
    if(changed(Param::MorphDuration) || changed(Param::MorphRandomDuration) || changed(Param::MorphAttack) || changed(Param::MorphDecay))
        m.grainDuration(p[Param::MorphDuration],p[Param::MorphRandomDuration],p[Param::MorphAttack],p[Param::MorphDecay]);
    if(changed(Param::MorphFrequency) || changed(Param::MorphGliss) || changed(Param::MorphRandomFrequency) || changed(Param::MorphRandomGliss)) {
        const double natural=s.table.naturalFreq(s.rate);
        m.frequency(natural*p[Param::MorphFrequency],p[Param::MorphRandomFrequency],natural*p[Param::MorphGliss],p[Param::MorphRandomGliss]);
    }
    if(changed(Param::MorphWindow)) m.grainEnvelope(static_cast<Pandora::windowType>(int(p[Param::MorphWindow])));
    if(changed(Param::MorphPan)) m.spatialPosition(p[Param::MorphPan]);
    if(changed(Param::MorphSpeed) || changed(Param::MorphJitter)) m.speedFactor(p[Param::MorphSpeed],long(p[Param::MorphJitter]));
    if(changed(Param::MorphAmplitude)) m.grainAmplitude(p[Param::MorphAmplitude]);
    if(changed(Param::MorphReverse)) m.grainDirection(p[Param::MorphReverse]?-1:1);
    if(changed(Param::MorphGrainlet)) m.generation(p[Param::MorphGrainlet]?Pandora::GRAINLET:Pandora::ASYNC);
    if(changed(Param::MorphStart) || changed(Param::MorphEnd)) m.setSelection(p[Param::MorphStart],p[Param::MorphEnd]);
    if(changed(Param::ReverbMix)) { s.reverbLeft->setEffectMix(p[Param::ReverbMix]); s.reverbRight->setEffectMix(p[Param::ReverbMix]); }
    if(changed(Param::ReverbTime)) { s.reverbLeft->setT60(p[Param::ReverbTime]); s.reverbRight->setT60(p[Param::ReverbTime]); }
    s.parameters=p; s.configured=true;
}
void Processor::process(const float* input,float* output,std::size_t frames,std::size_t inputStride) noexcept {
    auto& s=*impl_; const auto& p=s.parameters;
    s.inputPeak=s.leftPeak=s.rightPeak=0;
    for(std::size_t i=0;i<frames;++i) {
        double sample=0;
        const int source=int(p[Param::Source]);
        if(source==0) sample=input?input[i*inputStride]:0;
        else if(source==1 || source==2) {
            sample=.35*(source==1?std::sin(TWOPI*s.phase):(s.phase<.5?1.0:-1.0));
            s.phase+=p[Param::SourceFrequency]/s.rate;
            if(s.phase>=1) s.phase-=1;
        } else if(!s.file.mono.empty()) {
            const auto size=s.file.mono.size();
            if(s.filePosition>=size && p[Param::LoopFile]) s.filePosition=std::fmod(s.filePosition,double(size));
            if(s.filePosition<size) {
                const auto index=static_cast<std::size_t>(s.filePosition);
                const auto next=index+1<size?index+1:(p[Param::LoopFile]?0:index);
                const double frac=s.filePosition-index;
                sample=s.file.mono[index]+frac*(s.file.mono[next]-s.file.mono[index]);
                s.filePosition+=double(s.file.sampleRate)/s.rate;
            }
        }
        sample=std::isfinite(sample)?std::clamp(sample*p[Param::InputGain],-4.0,4.0):0;
        s.inputPeak=std::max(s.inputPeak,static_cast<float>(std::abs(sample)));
        s.table.set(sample);
        const auto& harmony=s.harmony->tick();
        double left=harmony.get(Pandora::LEFT),right=harmony.get(Pandora::RIGHT);
        if(p[Param::MorphLevel]>0) {
            const auto& grain=s.morph->tick();
            left+=grain.get(Pandora::LEFT)*p[Param::MorphLevel];
            right+=grain.get(Pandora::RIGHT)*p[Param::MorphLevel];
        }
        left=s.reverbLeft->tick(left); right=s.reverbRight->tick(right,1);
        if(p[Param::Bypass]) left=right=sample;
        s.gain+=std::clamp(p[Param::OutputGain]-s.gain,-1.0/128,1.0/128);
        left*=s.gain; right*=s.gain;
        left=std::isfinite(left)?std::clamp(left,-1.0,1.0):0;
        right=std::isfinite(right)?std::clamp(right,-1.0,1.0):0;
        output[i*2]=static_cast<float>(left); output[i*2+1]=static_cast<float>(right);
        s.leftPeak=std::max(s.leftPeak,static_cast<float>(std::abs(left)));
        s.rightPeak=std::max(s.rightPeak,static_cast<float>(std::abs(right)));
    }
}
void Processor::midi(unsigned char status,unsigned char note,unsigned char velocity) noexcept {
    auto& s=*impl_; const auto& p=s.parameters; auto& h=*s.harmony;
    if((status&15)!=int(p[Param::MidiChannel])-1 || p[Param::MidiMode]==0) return;
    const int type=status&0xf0;
    if(type==0xb0 && (note==120 || note==123)) {
        s.notes.fill(-1);
        s.fundamentalNotes.fill(0);s.velocities.fill(0);
        for(int i=0;i<4;++i) h.status(i,p[Param::MidiMode]==2?false:p[voiceParam(i,VoiceParam::Enabled)]!=0);
        h.fundamental(std::pow(2.0,p[Param::Transpose]/12),false);
        return;
    }
    const bool on=type==0x90 && velocity!=0;
    const bool off=type==0x80 || (type==0x90 && velocity==0);
    if(!on && !off) return;
    if(note>=128) return;
    const double hz=440*std::pow(2.0,(int(note)-69)/12.0);
    if(p[Param::MidiMode]==1) {
        s.fundamentalNotes[note]=on?++s.noteOrder:0;
        s.restoreMidi(p);
    } else {
        if(off) for(int i=0;i<4;++i) { if(s.notes[i]==note) { s.notes[i]=-1; h.status(i,false); } }
        if(on) {
            int voice=-1;
            for(int i=0;i<4;++i) if(s.notes[i]==note) {voice=i;break;}
            if(voice<0) for(int i=0;i<4;++i) if(s.notes[i]<0) {voice=i;break;}
            if(voice>=0) {
                s.notes[voice]=note;
                s.velocities[voice]=velocity;
                h.ratios(voice,p[voiceParam(voice,VoiceParam::Level)]*velocity/127.0,hz,true);
                h.status(voice,p[voiceParam(voice,VoiceParam::Enabled)]!=0);
            }
        }
    }
}
double Processor::pitch() const noexcept { return impl_->yin?impl_->yin->get():impl_->spectral->get(); }
std::size_t Processor::latencySamples() const noexcept {
    const auto delay=static_cast<std::size_t>(impl_->rate*impl_->parameters[Param::DelayMs]/1000);
    return delay?delay-1:0; // The table oscillator advances before its first read.
}
float Processor::inputPeak() const noexcept { return impl_->inputPeak; }
float Processor::leftPeak() const noexcept { return impl_->leftPeak; }
float Processor::rightPeak() const noexcept { return impl_->rightPeak; }
} // namespace revoice
