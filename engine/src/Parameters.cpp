#include "revoice/Parameters.h"
#include <algorithm>
#include <cmath>

namespace revoice {
const std::array<ParameterInfo, parameterCount>& parameterInfo() {
    static const auto info = [] {
        std::array<ParameterInfo, parameterCount> result{};
        auto add = [&](Param id, const char* key, const char* label, const char* group,
                       double low, double high, double initial, double step = .01,
                       const char* choices = nullptr, bool restart = false) {
            result[static_cast<std::size_t>(id)] = {key, label, group, low, high, initial, step, choices, restart};
        };
        add(Param::Source,"source","Source","Source",0,3,0,1,"Live input|Sine|Square|WAV file",true);
        add(Param::SourceFrequency,"source_frequency","Test tone (Hz)","Source",40,2000,220,1);
        add(Param::InputGain,"input_gain","Input gain","Source",0,4,1);
        add(Param::LoopFile,"loop_file","Loop WAV file","Source",0,1,1,1);
        add(Param::DelayMs,"delay_ms","Input delay (ms)","Source",5,200,23.22,.1,nullptr,true);
        add(Param::SourceLevel,"source_level","Original voice","Mix",0,2,.35);
        add(Param::HarmonyLevel,"harmony_level","Harmony","Mix",0,2,1);
        add(Param::MorphLevel,"morph_level","Morphizer","Mix",0,2,0);
        add(Param::ReverbMix,"reverb_mix","Reverb mix","Mix",0,1,.15);
        add(Param::ReverbTime,"reverb_time","Reverb decay (s)","Mix",.1,15,2.5,.1);
        add(Param::OutputGain,"output_gain","Output gain","Mix",0,2,1);
        add(Param::Bypass,"bypass","Bypass effects","Mix",0,1,0,1);
        add(Param::Freeze,"freeze","Freeze pitch","Harmony",0,1,0,1);
        add(Param::AntiRumble,"anti_rumble","Remove low rumble","Harmony",0,1,1,1);
        add(Param::Logic,"logic","Harmony mode","Harmony",0,10,3,1,
            "Custom chord|Dominant seventh|Minor seventh|Major seventh|Diminished seventh|Augmented seventh|Quartal|Scale harmony|Intellichord|Minor sixth|Single chord");
        add(Param::Key,"key","Key","Harmony",0,11,0,1,"C|C#|D|D#|E|F|F#|G|G#|A|A#|B");
        add(Param::Scale,"scale","Scale","Harmony",0,5,0,1,"Major|Lydian|Natural minor|Dorian|Harmonic minor|Blues");
        add(Param::Transpose,"transpose","Master transpose (st)","Harmony",-24,24,0,1);
        add(Param::Window,"window","Grain envelope","Harmony",0,7,4,1,
            "Rectangle|Bartlett|Gaussian|Hamming|Hann|Blackman|Blackman-Harris|ADSR",true);
        add(Param::Attack,"attack","Grain attack","Harmony",.01,.5,.5);
        add(Param::Decay,"decay","Grain decay","Harmony",.01,.5,.5);
        add(Param::Cycles,"cycles","Cycles per grain","Harmony",1,8,2,1);
        add(Param::AutoGrains,"auto_grains","Automatic grain length","Harmony",0,1,1,1);
        add(Param::PitchChange,"pitch_change","Pitch change threshold (st)","Harmony",0,2,0,.01);
        add(Param::Detector,"detector","Pitch detector","Detector",0,1,0,1,"Period|Spectral",true);
        add(Param::MinimumPitch,"minimum_pitch","Minimum pitch (Hz)","Detector",40,500,77.782,.1,nullptr,true);
        add(Param::MaximumPitch,"maximum_pitch","Maximum pitch (Hz)","Detector",501,4000,1396.913,.1,nullptr,true);
        add(Param::Threshold,"threshold","Signal threshold","Detector",.001,.5,.03,.001);
        add(Param::FilterQuality,"filter_quality","Filter Q","Detector",.1,2,.707,.001);
        add(Param::SmoothingRate,"smoothing_rate","Smoothing (Hz)","Detector",5,4000,2500,1);
        add(Param::SmoothingQuality,"smoothing_quality","Smoothing Q","Detector",.1,2,.707,.001);
        add(Param::FilterLevels,"filter_levels","Filter stages","Detector",1,8,2,1,nullptr,true);
        add(Param::DetectorSplit,"detector_split","Peak sensitivity","Detector",1,100,30,1);
        add(Param::Feedback,"feedback","Period feedback","Detector",0,.95,.3);
        add(Param::FftSize,"fft_size","FFT size","Detector",0,5,3,1,"128|256|512|1024|2048|4096",true);
        add(Param::MorphDensity,"morph_density","Grains per second","Morphizer",1,300,80,1);
        add(Param::MorphRandomDensity,"morph_random_density","Density randomness","Morphizer",0,1,0);
        add(Param::MorphDuration,"morph_duration","Grain duration (s)","Morphizer",.005,.5,.1,.001);
        add(Param::MorphRandomDuration,"morph_random_duration","Duration randomness","Morphizer",0,1,0);
        add(Param::MorphFrequency,"morph_frequency","Pitch ratio","Morphizer",.25,4,1);
        add(Param::MorphRandomFrequency,"morph_random_frequency","Pitch randomness","Morphizer",0,1,0);
        add(Param::MorphGliss,"morph_gliss","Glissando ratio","Morphizer",.25,4,1);
        add(Param::MorphRandomGliss,"morph_random_gliss","Glissando randomness","Morphizer",0,1,0);
        add(Param::MorphWindow,"morph_window","Grain envelope","Morphizer",0,7,2,1,
            "Rectangle|Bartlett|Gaussian|Hamming|Hann|Blackman|Blackman-Harris|ADSR",true);
        add(Param::MorphPan,"morph_pan","Spatial spread","Morphizer",0,1,.5);
        add(Param::MorphSpeed,"morph_speed","Table scan speed","Morphizer",-4,4,1);
        add(Param::MorphJitter,"morph_jitter","Phase jitter (samples)","Morphizer",0,10000,1000,1);
        add(Param::MorphAmplitude,"morph_amplitude","Per-grain level","Morphizer",0,1,.25);
        add(Param::MorphReverse,"morph_reverse","Reverse grains","Morphizer",0,1,0,1);
        add(Param::MorphGrainlet,"morph_grainlet","Grainlet mode","Morphizer",0,1,0,1);
        add(Param::MorphStart,"morph_start","Selection start","Morphizer",0,.99,0);
        add(Param::MorphEnd,"morph_end","Selection end","Morphizer",.01,1,1);
        add(Param::MorphAttack,"morph_attack","Grain attack","Morphizer",.01,.5,.5);
        add(Param::MorphDecay,"morph_decay","Grain decay","Morphizer",.01,.5,.5);
        add(Param::MidiMode,"midi_mode","MIDI control","MIDI",0,2,0,1,"Off|Fundamental note|Four-voice chord");
        add(Param::MidiChannel,"midi_channel","MIDI channel","MIDI",1,16,1,1);
        for(int voice=0;voice<4;++voice) {
            const std::string group="Voice "+std::to_string(voice+1);
            auto v = [&](VoiceParam field,const char* name,const char* label,double lo,double hi,double init,double step=.01) {
                const std::string key="voice"+std::to_string(voice+1)+"_"+name;
                result[static_cast<std::size_t>(voiceParam(voice,field))]={key,label,group,lo,hi,init,step,nullptr,false};
            };
            v(VoiceParam::Enabled,"enabled","Enabled",0,1,1,1);
            v(VoiceParam::Highpass,"highpass","High-pass filter",0,1,1,1);
            v(VoiceParam::Level,"level","Level",0,1,.25);
            v(VoiceParam::Semitones,"semitones","Custom interval (st)",-24,24,voice==1?4:voice==2?-5:voice==3?-1:0,1);
            v(VoiceParam::VibratoDepth,"vibrato_depth","Vibrato depth (Hz)",0,20,0);
            v(VoiceParam::VibratoRate,"vibrato_rate","Vibrato rate (Hz)",.1,15,4.1+voice*.1,.1);
            v(VoiceParam::RandomVibrato,"random_vibrato","Vibrato randomness",0,10,0);
            v(VoiceParam::Pan,"pan","Pan (L to R)",0,1,.5);
            v(VoiceParam::Formant,"formant","Formant ratio",.5,2,1);
        }
        return result;
    }();
    return info;
}
Parameters::Parameters() {
    const auto& info=parameterInfo();
    for(std::size_t i=0;i<values.size();++i) values[i]=info[i].initial;
}
void Parameters::set(Param id,double value) {
    const auto& info=parameterInfo()[static_cast<std::size_t>(id)];
    if(!std::isfinite(value)) value=info.initial;
    value=std::clamp(value,info.minimum,info.maximum);
    if(info.step==1) value=std::round(value);
    (*this)[id]=value;
}
void Parameters::sanitize() {
    for(std::size_t i=0;i<values.size();++i) set(static_cast<Param>(i),values[i]);
    if((*this)[Param::MorphEnd]<=(*this)[Param::MorphStart])
        (*this)[Param::MorphEnd]=std::min(1.0,(*this)[Param::MorphStart]+.01);
}
bool Parameters::needsRestart(const Parameters& other) const {
    const auto& info=parameterInfo();
    for(std::size_t i=0;i<values.size();++i)
        if(info[i].restart && values[i]!=other.values[i]) return true;
    return false;
}
} // namespace revoice
