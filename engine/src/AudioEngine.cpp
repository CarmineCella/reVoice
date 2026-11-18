#include "revoice/AudioEngine.h"
#include "revoice/Processor.h"
#include "revoice/Recorder.h"
#include "revoice/SpscQueue.h"
#include <portaudio.h>
#include <portmidi.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <stdexcept>
#include <thread>

namespace revoice {
namespace {
void checkAudio(PaError result,const char* action) {
    if(result!=paNoError) throw std::runtime_error(std::string(action)+": "+Pa_GetErrorText(result));
}
void checkMidi(PmError result,const char* action) {
    if(result<0) throw std::runtime_error(std::string(action)+": "+Pm_GetErrorText(result));
}
PmTimestamp midiTime(void*) {
    return static_cast<PmTimestamp>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
}
}
struct AudioEngine::Impl {
    static_assert(std::atomic<float>::is_always_lock_free);
    static_assert(std::atomic<double>::is_always_lock_free);
    struct Midi {unsigned char status=0,note=0,velocity=0;};
    PaStream* stream=nullptr;
    PortMidiStream* midiIn=nullptr;
    PortMidiStream* midiOut=nullptr;
    bool midiAvailable=false,playing=false;
    std::atomic<bool> midiRun{false},panicRequested{false},callbackError{false};
    std::thread midiWorker;
    std::unique_ptr<Processor> processor;
    Recorder recorder;
    Parameters controlParameters;
    SpscQueue<Parameters,16> changes;
    SpscQueue<Midi,256> midi;
    AudioSettings settings;
    std::atomic<float> input{0},left{0},right{0},cpu{0};
    std::atomic<double> pitch{0};
    std::atomic<std::uint64_t> xruns{0},droppedMidi{0};
    double latency=0;
    Impl() {
        checkAudio(Pa_Initialize(),"Cannot initialize audio");
        midiAvailable=Pm_Initialize()>=0;
    }
    ~Impl(){if(midiAvailable) Pm_Terminate();Pa_Terminate();}
    static int callback(const void* in,void* out,unsigned long frames,
                        const PaStreamCallbackTimeInfo*,PaStreamCallbackFlags flags,void* data) noexcept {
        auto& s=*static_cast<Impl*>(data);
        auto* output=static_cast<float*>(out);
        if(!output) return paAbort;
        std::fill(output,output+2*frames,0.0f);
        if(flags&(paInputOverflow|paOutputUnderflow)) s.xruns.fetch_add(1,std::memory_order_relaxed);
        try {
            Parameters update=s.controlParameters; // audio thread does not write controlParameters
            // Fixed queue capacities bound the maximum work even during floods.
            for(int n=0;n<15 && s.changes.pop(update);++n) s.processor->apply(update);
            if(s.panicRequested.exchange(false)) {
                for(unsigned char channel=0;channel<16;++channel) s.processor->midi(0xb0|channel,123,0);
            }
            Midi message;
            for(int n=0;n<128 && s.midi.pop(message);++n)
                s.processor->midi(message.status,message.note,message.velocity);
            s.processor->process(static_cast<const float*>(in),output,frames);
            s.input.store(s.processor->inputPeak(),std::memory_order_relaxed);
            s.left.store(s.processor->leftPeak(),std::memory_order_relaxed);
            s.right.store(s.processor->rightPeak(),std::memory_order_relaxed);
            s.pitch.store(s.processor->pitch(),std::memory_order_relaxed);
            s.recorder.push(output,frames);
            s.cpu.store(static_cast<float>(Pa_GetStreamCpuLoad(s.stream)),std::memory_order_relaxed);
        }catch(...){std::fill(output,output+2*frames,0.0f);s.callbackError=true;}
        return paContinue;
    }
};
AudioEngine::AudioEngine():impl_(std::make_unique<Impl>()){}
AudioEngine::~AudioEngine(){stop();}
std::vector<Device> AudioEngine::inputs()const {
    std::vector<Device> devices{{-1,"System default"}};
    const int count=Pa_GetDeviceCount();
    for(int i=0;i<count;++i) {const auto* d=Pa_GetDeviceInfo(i);if(d && d->maxInputChannels>0)
        devices.push_back({i,std::string(d->name)+" ("+Pa_GetHostApiInfo(d->hostApi)->name+")"});}
    return devices;
}
std::vector<Device> AudioEngine::outputs()const {
    std::vector<Device> devices{{-1,"System default"}};
    const int count=Pa_GetDeviceCount();
    for(int i=0;i<count;++i) {const auto* d=Pa_GetDeviceInfo(i);if(d && d->maxOutputChannels>=2)
        devices.push_back({i,std::string(d->name)+" ("+Pa_GetHostApiInfo(d->hostApi)->name+")"});}
    return devices;
}
std::vector<Device> AudioEngine::midiInputs()const {
    std::vector<Device> devices{{-1,"Disabled"}};
    if(impl_->midiAvailable) for(int i=0;i<Pm_CountDevices();++i) {const auto* d=Pm_GetDeviceInfo(i);if(d && d->input) devices.push_back({i,d->name});}
    return devices;
}
std::vector<Device> AudioEngine::midiOutputs()const {
    std::vector<Device> devices{{-1,"Disabled"}};
    if(impl_->midiAvailable) for(int i=0;i<Pm_CountDevices();++i) {const auto* d=Pm_GetDeviceInfo(i);if(d && d->output) devices.push_back({i,d->name});}
    return devices;
}
void AudioEngine::start(const AudioSettings& settings,const Parameters& requested,const AudioFile* source) {
    stop();auto& s=*impl_;s.settings=settings;
    Parameters parameters=requested;parameters.sanitize();
    if(settings.frames<32 || settings.frames>4096) throw std::runtime_error("Audio buffer must contain 32 to 4096 frames");
    s.processor=std::make_unique<Processor>(settings.sampleRate,parameters,source);
    s.controlParameters=parameters;s.changes.reset();s.midi.reset();s.xruns=0;s.callbackError=false;s.droppedMidi=0;s.panicRequested=false;
    try {
        const int out=settings.output<0?Pa_GetDefaultOutputDevice():settings.output;
        const auto* output=Pa_GetDeviceInfo(out);
        if(!output || output->maxOutputChannels<2) throw std::runtime_error("No stereo output device is available; select an output in Audio / MIDI");
        PaStreamParameters outputParameters{out,2,paFloat32,output->defaultLowOutputLatency,nullptr};
        PaStreamParameters inputParameters{};
        const bool live=parameters[Param::Source]==0;
        if(live) {
            const int in=settings.input<0?Pa_GetDefaultInputDevice():settings.input;
            const auto* input=Pa_GetDeviceInfo(in);
            if(!input || input->maxInputChannels<1) throw std::runtime_error("No input device is available; select an input or choose a test tone / WAV source");
            inputParameters={in,1,paFloat32,input->defaultLowInputLatency,nullptr};
        }
        checkAudio(Pa_IsFormatSupported(live?&inputParameters:nullptr,&outputParameters,settings.sampleRate),"Unsupported device or sample-rate combination");
        checkAudio(Pa_OpenStream(&s.stream,live?&inputParameters:nullptr,&outputParameters,settings.sampleRate,
            settings.frames,paClipOff|paDitherOff,&Impl::callback,&s),"Cannot open audio stream");
        if(const auto* info=Pa_GetStreamInfo(s.stream)) s.latency=1000*(info->inputLatency+info->outputLatency)+parameters[Param::DelayMs];
        if(settings.midiInput>=0 || settings.midiOutput>=0) {
            if(!s.midiAvailable) throw std::runtime_error("MIDI initialization failed");
            if(settings.midiInput>=0) checkMidi(Pm_OpenInput(&s.midiIn,settings.midiInput,nullptr,512,midiTime,nullptr),"Cannot open MIDI input");
            if(settings.midiOutput>=0) checkMidi(Pm_OpenOutput(&s.midiOut,settings.midiOutput,nullptr,512,midiTime,nullptr,0),"Cannot open MIDI output");
            if(s.midiIn) {
                Pm_SetFilter(s.midiIn,PM_FILT_ACTIVE|PM_FILT_SYSEX|PM_FILT_CLOCK);
                s.midiRun=true;
                s.midiWorker=std::thread([&s] {
                    while(s.midiRun.load(std::memory_order_acquire)) {
                        PmEvent events[32];const int count=Pm_Read(s.midiIn,events,32);
                        if(count>0) for(int i=0;i<count;++i) {
                            const auto message=events[i].message;
                            if(!s.midi.push({static_cast<unsigned char>(Pm_MessageStatus(message)),static_cast<unsigned char>(Pm_MessageData1(message)),static_cast<unsigned char>(Pm_MessageData2(message))}))
                                s.droppedMidi.fetch_add(1,std::memory_order_relaxed);
                            if(s.settings.midiThru && s.midiOut) Pm_Write(s.midiOut,&events[i],1);
                        }
                        else if(count<0) s.droppedMidi.fetch_add(1,std::memory_order_relaxed);
                        std::this_thread::sleep_for(std::chrono::milliseconds(2));
                    }
                });
            }
        }
        checkAudio(Pa_StartStream(s.stream),"Cannot start audio");s.playing=true;
    }catch(...){stop();throw;}
}
void AudioEngine::stop() {
    auto& s=*impl_;
    if(s.stream) {Pa_StopStream(s.stream);Pa_CloseStream(s.stream);s.stream=nullptr;}
    s.playing=false;s.recorder.stop();
    s.midiRun=false;if(s.midiWorker.joinable()) s.midiWorker.join();
    if(s.midiIn) {Pm_Close(s.midiIn);s.midiIn=nullptr;}
    if(s.midiOut) {Pm_Close(s.midiOut);s.midiOut=nullptr;}
    s.processor.reset();s.changes.reset();s.midi.reset();s.input=0;s.left=0;s.right=0;s.pitch=0;s.cpu=0;
}
bool AudioEngine::running()const noexcept{return impl_->playing;}
bool AudioEngine::update(const Parameters& parameters) {
    if(!running()) return false;
    if(parameters.needsRestart(impl_->controlParameters)) return false;
    // controlParameters is immutable while the callback runs. Queue snapshots
    // are the only parameters shared across the GUI/audio boundary.
    return impl_->changes.push(parameters);
}
void AudioEngine::panic(){impl_->panicRequested=true;}
void AudioEngine::startRecording(const std::filesystem::path& path) {
    if(!running()) throw std::runtime_error("Start the audio engine before recording");
    impl_->recorder.start(path,static_cast<std::uint32_t>(impl_->settings.sampleRate));
}
void AudioEngine::stopRecording(){impl_->recorder.stop();}
bool AudioEngine::recording()const noexcept{return impl_->recorder.active();}
EngineStats AudioEngine::stats()const noexcept {
    const auto& s=*impl_;EngineStats result;
    result.input=s.input.load();result.left=s.left.load();result.right=s.right.load();result.cpu=s.cpu.load();
    result.pitch=s.pitch.load();result.latencyMs=s.latency;result.xruns=s.xruns.load();
    result.droppedRecordingFrames=s.recorder.droppedFrames();result.droppedMidiMessages=s.droppedMidi.load();
    result.callbackFailed=s.callbackError.load();result.recordingFailed=s.recorder.failed();return result;
}
} // namespace revoice
