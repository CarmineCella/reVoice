// SPDX-License-Identifier: BSD-2-Clause
#include "VstProcessor.h"
#include "PluginModel.h"
#include "Ids.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstevents.h"
#include <algorithm>
#include <cmath>

namespace revoice::plugin {
using namespace Steinberg;
using namespace Steinberg::Vst;
VstProcessor::VstProcessor() {
    static_assert(std::atomic<double>::is_always_lock_free);
    static_assert(std::atomic<std::uint64_t>::is_always_lock_free);
    setControllerClass(FUID::fromTUID(ControllerID));
    for(std::size_t i=0;i<parameterCount;++i) {
        state_[i].store(parameters_.values[i]);incomingState_[i].store(parameters_.values[i]);
    }
    latency_.store(uint32(std::floor(44100*parameters_[Param::DelayMs]/1000)-1));
    (void)parameterId(Param::Bypass); // Construct immutable lookup tables outside process().
}
tresult PLUGIN_API VstProcessor::initialize(FUnknown* context) {
    const auto result=AudioEffect::initialize(context);if(result!=kResultOk) return result;
    addAudioInput(STR16("Vocal input"),SpeakerArr::kStereo);
    addAudioOutput(STR16("Harmony output"),SpeakerArr::kStereo);
    addEventInput(STR16("MIDI input"),16);
    return kResultOk;
}
tresult PLUGIN_API VstProcessor::setBusArrangements(SpeakerArrangement* inputs,int32 ins,SpeakerArrangement* outputs,int32 outs) {
    if(ins!=1 || outs!=1 || !inputs || !outputs ||
       (inputs[0]!=SpeakerArr::kMono && inputs[0]!=SpeakerArr::kStereo) ||
       (outputs[0]!=SpeakerArr::kMono && outputs[0]!=SpeakerArr::kStereo)) return kResultFalse;
    return AudioEffect::setBusArrangements(inputs,ins,outputs,outs);
}
tresult PLUGIN_API VstProcessor::canProcessSampleSize(int32 size) {return size==kSample32?kResultTrue:kResultFalse;}
tresult PLUGIN_API VstProcessor::setupProcessing(ProcessSetup& setup) {
    if(setup.symbolicSampleSize!=kSample32 || !std::isfinite(setup.sampleRate) || setup.sampleRate<8000 ||
       setup.sampleRate>192000 || setup.maxSamplesPerBlock<1 || setup.maxSamplesPerBlock>1048576) return kInvalidArgument;
    const auto result=AudioEffect::setupProcessing(setup);
    if(result==kResultOk) {
        rate_.store(setup.sampleRate);latency_.store(uint32(std::max(0.0,std::floor(setup.sampleRate*parameterInfo()[std::size_t(Param::DelayMs)].initial/1000)-1)));
    }
    return result;
}
tresult PLUGIN_API VstProcessor::setActive(TBool active) {
    try {
        if(active) {
            std::uint64_t serial=0;
            if(incomingState(parameters_,serial) && serial!=appliedSerial_.load(std::memory_order_relaxed)) appliedSerial_.store(serial,std::memory_order_release);
            else for(std::size_t i=0;i<parameterCount;++i) parameters_.values[i]=state_[i].load(std::memory_order_relaxed);
            parameters_.sanitize();parameters_.set(Param::Source,0);parameters_.set(Param::Detector,0);
            for(std::size_t i=0;i<parameterCount;++i) state_[i].store(parameters_.values[i],std::memory_order_relaxed);
            Parameters engine=parameters_;engine.set(Param::Bypass,0);
            processor_=std::make_unique<revoice::Processor>(processSetup.sampleRate,engine);
            const auto size=std::size_t(std::max(32,processSetup.maxSamplesPerBlock));
            mono_.assign(size,0);wet_.assign(size*2,0);dryLeft_.assign(size,0);dryRight_.assign(size,0);
            delayLeft_.assign(processor_->latencySamples()+1,0);delayRight_.assign(delayLeft_.size(),0);
            delayCursor_=0;bypassMix_=parameters_[Param::Bypass]!=0?1:0;
            latency_.store(uint32(processor_->latencySamples()));
        } else processor_.reset();
        return AudioEffect::setActive(active);
    } catch(...) {processor_.reset();return kInternalError;}
}
tresult PLUGIN_API VstProcessor::setProcessing(TBool playing) {
    if(!playing && processor_) for(unsigned char channel=0;channel<16;++channel) processor_->midi(0xb0|channel,123,0);
    return kResultOk;
}
uint32 PLUGIN_API VstProcessor::getLatencySamples() {
    return latency_.load(std::memory_order_relaxed);
}
uint32 PLUGIN_API VstProcessor::getTailSamples() {
    const double seconds=state_[std::size_t(Param::ReverbMix)].load()>0?2*state_[std::size_t(Param::ReverbTime)].load()+.5:.5;
    return uint32(seconds*rate_.load(std::memory_order_relaxed))+getLatencySamples();
}
bool VstProcessor::incomingState(Parameters& parameters,std::uint64_t& serial) const noexcept {
    const auto before=stateSerial_.load(std::memory_order_acquire);
    if(before&1) return false; // A host state write is in progress; never wait on audio.
    Parameters snapshot;
    for(std::size_t i=0;i<parameterCount;++i) snapshot.values[i]=incomingState_[i].load(std::memory_order_acquire);
    if(stateSerial_.load(std::memory_order_acquire)!=before) return false;
    parameters=snapshot;serial=before;return true;
}
void VstProcessor::applyParameters() {
    parameters_.sanitize();
    for(std::size_t i=0;i<parameterCount;++i) state_[i].store(parameters_.values[i],std::memory_order_relaxed);
    if(processor_) {Parameters engine=parameters_;engine.set(Param::Bypass,0);processor_->apply(engine);}
}
void VstProcessor::render(ProcessData& data,int offset,int frames) noexcept {
    auto& out=data.outputs[0];
    const auto* in=data.numInputs>0 && data.inputs?&data.inputs[0]:nullptr;
    for(int i=0;i<frames;++i) {
        const int sample=offset+i;
        float left=in && in->numChannels>0 && in->channelBuffers32 && in->channelBuffers32[0] && !(in->silenceFlags&1)?in->channelBuffers32[0][sample]:0;
        float right=left;
        if(in && in->numChannels>1) right=in->channelBuffers32 && in->channelBuffers32[1] && !(in->silenceFlags&2)?in->channelBuffers32[1][sample]:0;
        if(!std::isfinite(left)) left=0;if(!std::isfinite(right)) right=0;
        mono_[i]=in && in->numChannels>1?.5f*left+.5f*right:left;
        delayLeft_[delayCursor_]=left;delayRight_[delayCursor_]=right;
        delayCursor_=(delayCursor_+1)%delayLeft_.size();
        dryLeft_[i]=delayLeft_[delayCursor_];dryRight_[i]=delayRight_[delayCursor_];
    }
    processor_->process(mono_.data(),wet_.data(),std::size_t(frames));
    for(int i=0;i<frames;++i) {
        const double target=parameters_[Param::Bypass]!=0?1:0;
        bypassMix_+=std::clamp(target-bypassMix_,-1.0/128,1.0/128);
        const float left=float(wet_[2*i]*(1-bypassMix_)+dryLeft_[i]*bypassMix_);
        const float right=float(wet_[2*i+1]*(1-bypassMix_)+dryRight_[i]*bypassMix_);
        if(out.numChannels==1) {
            const float value=.5f*(left+right);out.channelBuffers32[0][offset+i]=value;
            if(value!=0) out.silenceFlags&=~uint64(1);
        } else {
            out.channelBuffers32[0][offset+i]=left;out.channelBuffers32[1][offset+i]=right;
            if(left!=0) out.silenceFlags&=~uint64(1);
            if(right!=0) out.silenceFlags&=~uint64(2);
        }
    }
}
tresult PLUGIN_API VstProcessor::process(ProcessData& data) {
    if(data.numSamples<0 || data.symbolicSampleSize!=kSample32) return kInvalidArgument;
    const auto serial=stateSerial_.load(std::memory_order_acquire);
    if(serial!=appliedSerial_.load(std::memory_order_relaxed)) {
        Parameters loaded;std::uint64_t loadedSerial=0;
        if(incomingState(loaded,loadedSerial)) {
            parameters_=loaded;applyParameters();appliedSerial_.store(loadedSerial,std::memory_order_release);
        }
    }
    struct Cursor {IParamValueQueue* queue=nullptr;Param param{};int point=0,count=0,offset=0;double value=0;};
    std::array<Cursor,parameterCount> cursors{};std::size_t used=0;
    auto advance=[&](Cursor& cursor) {
        if(cursor.point>=cursor.count || cursor.queue->getPoint(cursor.point++,cursor.offset,cursor.value)!=kResultOk) cursor.queue=nullptr;
        else cursor.offset=std::clamp(cursor.offset,0,data.numSamples);
    };
    if(data.inputParameterChanges) {
        const int count=std::min(int(parameterCount),data.inputParameterChanges->getParameterCount());
        for(int i=0;i<count;++i) {
            auto* queue=data.inputParameterChanges->getParameterData(i);if(!queue) continue;
            if(const auto id=parameterFromId(queue->getParameterId())) {
                auto& cursor=cursors[used++];cursor.queue=queue;cursor.param=*id;
                cursor.count=std::clamp(queue->getPointCount(),0,4096);advance(cursor);
            }
        }
    }
    const int eventCount=data.inputEvents?std::clamp(data.inputEvents->getEventCount(),0,4096):0;
    int eventIndex=0;Event event{};bool hasEvent=false;
    auto advanceEvent=[&] {
        hasEvent=eventIndex<eventCount && data.inputEvents->getEvent(eventIndex++,event)==kResultOk;
        if(hasEvent) event.sampleOffset=std::clamp(event.sampleOffset,0,data.numSamples);
    };
    advanceEvent();
    if(data.numOutputs>0 && data.outputs) data.outputs[0].silenceFlags=data.outputs[0].numChannels==1?1:3;
    int position=0;
    while(true) {
        bool dirty=false;
        for(std::size_t i=0;i<used;++i) while(cursors[i].queue && cursors[i].offset<=position) {
            parameters_.set(cursors[i].param,fromNormalized(cursors[i].param,cursors[i].value));
            dirty=true;advance(cursors[i]);
        }
        if(dirty) applyParameters();
        while(hasEvent && event.sampleOffset<=position) {
            if(processor_ && event.busIndex==0) {
                if(event.type==Event::kNoteOnEvent && event.noteOn.pitch>=0 && event.noteOn.pitch<128 && event.noteOn.channel>=0 && event.noteOn.channel<16) {
                    const float velocity=std::isfinite(event.noteOn.velocity)?std::clamp(event.noteOn.velocity,0.f,1.f):0;
                    processor_->midi(0x90|event.noteOn.channel,unsigned(event.noteOn.pitch),unsigned(velocity*127+.5f));
                } else if(event.type==Event::kNoteOffEvent && event.noteOff.pitch>=0 && event.noteOff.pitch<128 && event.noteOff.channel>=0 && event.noteOff.channel<16)
                    processor_->midi(0x80|event.noteOff.channel,unsigned(event.noteOff.pitch),0);
            }
            advanceEvent();
        }
        if(position>=data.numSamples) break;
        if(!processor_ || data.numOutputs<1 || !data.outputs || data.outputs[0].numChannels<1 || data.outputs[0].numChannels>2 ||
           !data.outputs[0].channelBuffers32 || !data.outputs[0].channelBuffers32[0] ||
           (data.outputs[0].numChannels==2 && !data.outputs[0].channelBuffers32[1])) return kResultFalse;
        int end=std::min(data.numSamples,position+int(mono_.size()));
        for(std::size_t i=0;i<used;++i) if(cursors[i].queue) end=std::min(end,cursors[i].offset);
        if(hasEvent) end=std::min(end,event.sampleOffset);
        render(data,position,end-position);position=end;
    }
    return kResultOk;
}
tresult PLUGIN_API VstProcessor::getState(IBStream* stream) {
    Parameters snapshot;
    std::uint64_t serial=0;
    if(!incomingState(snapshot,serial) || serial==appliedSerial_.load(std::memory_order_acquire))
        for(std::size_t i=0;i<parameterCount;++i) snapshot.values[i]=state_[i].load(std::memory_order_relaxed);
    return writeState(stream,snapshot)?kResultOk:kResultFalse;
}
tresult PLUGIN_API VstProcessor::setState(IBStream* stream) {
    Parameters loaded;if(!readState(stream,loaded)) return kResultFalse;
    stateSerial_.fetch_add(1,std::memory_order_acq_rel);
    for(std::size_t i=0;i<parameterCount;++i) incomingState_[i].store(loaded.values[i],std::memory_order_release);
    stateSerial_.fetch_add(1,std::memory_order_release);return kResultOk;
}
}
