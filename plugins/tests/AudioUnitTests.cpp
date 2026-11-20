// SPDX-License-Identifier: BSD-2-Clause
#include "PluginModel.h"
#include <AudioToolbox/AudioToolbox.h>
#include <CoreFoundation/CoreFoundation.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
using namespace revoice;
constexpr double Pi=3.14159265358979323846;
void check(OSStatus status,const char* what) {if(status!=noErr) {std::cerr<<what<<": "<<status<<'\n';throw std::runtime_error(what);}}
void require(bool value,const char* what) {if(!value) throw std::runtime_error(what);}
struct Input {double phase=0,frequency=220;};
OSStatus input(void* context,AudioUnitRenderActionFlags*,const AudioTimeStamp*,UInt32,UInt32 count,AudioBufferList* buffers) {
    auto& state=*static_cast<Input*>(context);
    for(UInt32 i=0;i<count;++i) {
        const float value=float(.02*std::sin(2*Pi*state.phase));
        state.phase+=state.frequency/48000;if(state.phase>=1) state.phase-=1;
        for(UInt32 channel=0;channel<buffers->mNumberBuffers;++channel)
            if(buffers->mBuffers[channel].mData) static_cast<float*>(buffers->mBuffers[channel].mData)[i]=value;
    }
    return noErr;
}
void parameter(AudioUnit unit,Param id,double plain) {
    // The SDK AU wrapper represents its VST3 parameter values in [0, 1].
    check(AudioUnitSetParameter(unit,plugin::parameterId(id),kAudioUnitScope_Global,0,
        AudioUnitParameterValue(plugin::toNormalized(id,plain)),0),"set AU parameter");
}
struct Unit {
    CFBundleRef bundle=nullptr;AudioUnit unit=nullptr;
    ~Unit() {if(unit) {AudioUnitUninitialize(unit);AudioComponentInstanceDispose(unit);}if(bundle) CFRelease(bundle);}
};
void run(const char* path,bool midi) {
    Unit instance;
    auto url=CFURLCreateFromFileSystemRepresentation(nullptr,reinterpret_cast<const UInt8*>(path),std::char_traits<char>::length(path),true);
    require(url,"create AU bundle URL");instance.bundle=CFBundleCreate(nullptr,url);CFRelease(url);
    require(instance.bundle && CFBundleLoadExecutable(instance.bundle),"load AU bundle");
    auto factory=reinterpret_cast<AudioComponentFactoryFunction>(CFBundleGetFunctionPointerForName(instance.bundle,CFSTR("AUWrapperFactory")));
    require(factory,"AU factory exported");
    AudioComponentDescription description{};description.componentType=midi?kAudioUnitType_MusicEffect:kAudioUnitType_Effect;
    description.componentSubType=midi?0x7256634d:0x72566365;description.componentManufacturer=0x436d4365; // rVcM or rVce / CmCe
    // Process-private registration: no copying into the user's plug-in folders.
    auto component=AudioComponentRegister(&description,CFSTR("Carmine Cella: reVoice"),0x000300,factory);
    require(component,"register test Audio Unit");check(AudioComponentInstanceNew(component,&instance.unit),"create Audio Unit");
    AudioStreamBasicDescription format{};format.mSampleRate=48000;format.mFormatID=kAudioFormatLinearPCM;
    format.mFormatFlags=kAudioFormatFlagIsFloat|kAudioFormatFlagIsPacked|kAudioFormatFlagIsNonInterleaved;
    format.mBytesPerPacket=format.mBytesPerFrame=4;format.mFramesPerPacket=1;format.mChannelsPerFrame=1;format.mBitsPerChannel=32;
    check(AudioUnitSetProperty(instance.unit,kAudioUnitProperty_StreamFormat,kAudioUnitScope_Input,0,&format,sizeof(format)),"set AU mono input");
    format.mChannelsPerFrame=2;
    check(AudioUnitSetProperty(instance.unit,kAudioUnitProperty_StreamFormat,kAudioUnitScope_Output,0,&format,sizeof(format)),"set AU stereo output");
    UInt32 capacity=512;check(AudioUnitSetProperty(instance.unit,kAudioUnitProperty_MaximumFramesPerSlice,kAudioUnitScope_Global,0,&capacity,sizeof(capacity)),"set AU buffer limit");
    Input source;AURenderCallbackStruct callback{input,&source};
    check(AudioUnitSetProperty(instance.unit,kAudioUnitProperty_SetRenderCallback,kAudioUnitScope_Input,0,&callback,sizeof(callback)),"set AU input callback");
    parameter(instance.unit,Param::SourceLevel,0);parameter(instance.unit,Param::ReverbMix,0);parameter(instance.unit,Param::Logic,0);
    for(int voice=0;voice<4;++voice) parameter(instance.unit,voiceParam(voice,VoiceParam::Enabled),voice==0);
    parameter(instance.unit,voiceParam(0,VoiceParam::Semitones),12);
    check(AudioUnitInitialize(instance.unit),"initialize AU renderer");
    double latency=0;UInt32 size=sizeof(latency);
    check(AudioUnitGetProperty(instance.unit,kAudioUnitProperty_Latency,kAudioUnitScope_Global,0,&latency,&size),"query AU latency");
    require(latency>0 && latency<.2,"AU exposes its processing latency");
    std::vector<float> left(512),right(512),captured;captured.reserve(48000);
    struct Buffers {UInt32 count;AudioBuffer buffers[2];} buffers{};
    buffers.count=2;for(auto& b:buffers.buffers) {b.mNumberChannels=1;b.mDataByteSize=512*sizeof(float);}
    buffers.buffers[0].mData=left.data();buffers.buffers[1].mData=right.data();
    double sampleTime=0;
    auto render=[&] {
        captured.clear();int position=0;
        while(position<48000) {
            const UInt32 count=UInt32(std::min(257,48000-position));
            for(auto& b:buffers.buffers) b.mDataByteSize=count*sizeof(float);
            AudioTimeStamp timestamp{};timestamp.mFlags=kAudioTimeStampSampleTimeValid;timestamp.mSampleTime=sampleTime;
            AudioUnitRenderActionFlags flags=0;
            check(AudioUnitRender(instance.unit,&flags,&timestamp,0,count,reinterpret_cast<AudioBufferList*>(&buffers)),"render AU audio");
            for(UInt32 i=0;i<count;++i) {require(std::isfinite(left[i]) && std::isfinite(right[i]),"finite AU output");captured.push_back(left[i]+right[i]);}
            position+=int(count);sampleTime+=count;
        }
    };
    auto power=[&](double hz) {
        double real=0,imag=0;constexpr int count=8192;
        for(int i=0;i<count;++i) {const double value=captured[captured.size()-count+i]*(.5-.5*std::cos(2*Pi*i/count));real+=value*std::cos(2*Pi*hz*i/48000);imag+=value*std::sin(2*Pi*hz*i/48000);}
        return real*real+imag*imag;
    };
    render();require(power(440)>.05 && power(440)>4*power(220),"AU renders octave-up harmony from quiet input");
    CFPropertyListRef saved=nullptr;size=sizeof(saved);
    check(AudioUnitGetProperty(instance.unit,kAudioUnitProperty_ClassInfo,kAudioUnitScope_Global,0,&saved,&size),"save AU project state");
    parameter(instance.unit,Param::HarmonyLevel,0);render();require(power(440)<1e-10,"AU mix automation mutes harmony");
    check(AudioUnitSetProperty(instance.unit,kAudioUnitProperty_ClassInfo,kAudioUnitScope_Global,0,&saved,sizeof(saved)),"restore AU project state");CFRelease(saved);
    render();require(power(440)>.05,"AU project recall restores audible harmony");
    if(midi) {
        parameter(instance.unit,voiceParam(0,VoiceParam::Semitones),0);parameter(instance.unit,Param::MidiMode,1);source.frequency=330;
        check(MusicDeviceMIDIEvent(instance.unit,0x90,69,100,17),"send AU MIDI note");render();
        require(power(440)>.05 && power(440)>4*power(330),"AU MIDI input informs harmonization");
    }
}
}
int main(int argc,char** argv) {
    try {require(argc==3,"pass regular and MIDI AU component paths");run(argv[1],false);run(argv[2],true);std::cout<<"AU effect/MIDI bundle audio, automation, state and latency checks passed\n";return 0;}
    catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
}
