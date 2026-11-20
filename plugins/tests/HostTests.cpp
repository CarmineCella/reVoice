// SPDX-License-Identifier: BSD-2-Clause
#include "VstProcessor.h"
#include "VstController.h"
#include "PluginModel.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include "public.sdk/source/vst/hosting/parameterchanges.h"
#include "public.sdk/source/vst/hosting/eventlist.h"
#include "public.sdk/source/common/memorystream.h"
#include "pluginterfaces/base/ustring.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <new>
#include <set>
#include <thread>
#include <stdexcept>
#include <vector>

thread_local bool trackAllocations=false;
thread_local std::size_t allocationCount=0;
void* operator new(std::size_t n) {if(trackAllocations) ++allocationCount;if(void* p=std::malloc(n?n:1)) return p;throw std::bad_alloc();}
void* operator new[](std::size_t n) {return ::operator new(n);}
void operator delete(void* p) noexcept {std::free(p);}
void operator delete[](void* p) noexcept {std::free(p);}
void operator delete(void* p,std::size_t) noexcept {std::free(p);}
void operator delete[](void* p,std::size_t) noexcept {std::free(p);}

namespace {
using namespace revoice;
using namespace revoice::plugin;
using namespace Steinberg;
using namespace Steinberg::Vst;
constexpr double Pi=3.14159265358979323846;
void require(bool good,const char* message) {if(!good) throw std::runtime_error(message);}
void near(double actual,double expected,double tolerance,const char* message) {
    if(std::abs(actual-expected)>tolerance) {std::cerr<<message<<": "<<actual<<" != "<<expected<<'\n';throw std::runtime_error(message);}
}
void rewind(MemoryStream& stream) {require(stream.seek(0,IBStream::kIBSeekSet,nullptr)==kResultOk,"rewind state");}
void change(ParameterChanges& changes,Param id,double plain,int offset=0) {
    int32 index=0;auto* queue=changes.addParameterData(parameterId(id),index);
    require(queue && queue->addPoint(offset,toNormalized(id,plain),index)==kResultOk,"construct host automation");
}
struct Fixture {
    HostApplication host;
    VstProcessor processor;
    std::vector<float> left=std::vector<float>(8192),right=std::vector<float>(8192),outLeft=std::vector<float>(8192),outRight=std::vector<float>(8192);
    float* inChannels[2]={left.data(),right.data()};float* outChannels[2]={outLeft.data(),outRight.data()};
    AudioBusBuffers input{},output{};
    explicit Fixture(const Parameters& parameters=Parameters(),double rate=48000,int channels=2) {
        require(processor.initialize(&host)==kResultOk,"initialize plug-in");
        BusInfo midi{};require(processor.getBusInfo(kEvent,kInput,0,midi)==kResultOk && midi.channelCount==16,"host MIDI bus advertises all channels");
        auto in=channels==1?SpeakerArr::kMono:SpeakerArr::kStereo;auto out=SpeakerArr::kStereo;
        require(processor.setBusArrangements(&in,1,&out,1)==kResultOk,"negotiate host buses");
        ProcessSetup setup{};setup.processMode=kRealtime;setup.symbolicSampleSize=kSample32;setup.maxSamplesPerBlock=128;setup.sampleRate=rate;
        require(processor.setupProcessing(setup)==kResultOk,"prepare host processing");
        MemoryStream state;require(writeState(&state,parameters),"encode initial project state");rewind(state);
        require(processor.setState(&state)==kResultOk,"restore initial project state");
        require(processor.setActive(true)==kResultOk,"activate plug-in");
        require(processor.setProcessing(true)==kResultOk,"start host processing");
        input.numChannels=channels;input.channelBuffers32=inChannels;output.numChannels=2;output.channelBuffers32=outChannels;
    }
    ~Fixture() {processor.setProcessing(false);processor.setActive(false);processor.terminate();}
    void process(int frames,IParameterChanges* changes=nullptr,IEventList* events=nullptr,bool inPlace=false) {
        outChannels[0]=inPlace?left.data():outLeft.data();outChannels[1]=inPlace?right.data():outRight.data();
        ProcessData data{};data.processMode=kRealtime;data.symbolicSampleSize=kSample32;data.numSamples=frames;
        data.numInputs=data.numOutputs=1;data.inputs=&input;data.outputs=&output;data.inputParameterChanges=changes;data.inputEvents=events;
        allocationCount=0;trackAllocations=true;const auto result=processor.process(data);trackAllocations=false;
        require(result==kResultOk,"host process callback succeeds");require(allocationCount==0,"plug-in process must not allocate");
        for(int i=0;i<frames;++i) require(std::isfinite(outChannels[0][i]) && std::isfinite(outChannels[1][i]),"finite host audio");
    }
};
void parametersAndState() {
    HostApplication host;VstController controller;
    require(controller.initialize(&host)==kResultOk,"initialize parameter controller");
    std::set<uint32> ids;int count=0;
    for(std::size_t i=0;i<parameterCount;++i) {
        const auto id=static_cast<Param>(i);if(!exposed(id)) continue;
        ++count;require(ids.insert(parameterId(id)).second,"stable parameter IDs are unique");
        require(parameterFromId(parameterId(id))==id,"host parameter ID resolves");
        for(double n:{0.,.5,1.}) {
            String128 string{};double restored=0;
            require(controller.getParamStringByValue(parameterId(id),n,string)==kResultOk,"format host parameter");
            require(controller.getParamValueByString(parameterId(id),string,restored)==kResultOk,"parse host parameter");
            near(fromNormalized(id,restored),fromNormalized(id,n),.006,"host text conversion preserves parameter value");
        }
    }
    require(controller.getParameterCount()==count,"all exposed parameters registered");
    near(controller.normalizedParamToPlain(parameterId(Param::HarmonyLevel),controller.getParamNormalized(parameterId(Param::HarmonyLevel))),0,1e-10,"harmony defaults to zero dB in host");
    Parameters saved;saved.set(Param::HarmonyLevel,.5);saved.set(voiceParam(2,VoiceParam::Semitones),9);
    MemoryStream state;require(writeState(&state,saved),"write project state");rewind(state);
    require(controller.setComponentState(&state)==kResultOk,"controller restores project state");
    near(controller.getParamNormalized(parameterId(Param::HarmonyLevel)),toNormalized(Param::HarmonyLevel,.5),1e-10,"controller recalls harmony mix");
    controller.terminate();
    Parameters untouched;untouched.set(Param::HarmonyLevel,.2);
    MemoryStream truncated(state.getData(),state.getSize()-3);
    require(!readState(&truncated,untouched),"truncated project state rejected");near(untouched[Param::HarmonyLevel],.2,0,"bad state leaves existing parameters intact");
}
void harmonyAndMidi() {
    Parameters p;p.set(Param::SourceLevel,0);p.set(Param::ReverbMix,0);p.set(Param::Logic,0);
    for(int v=0;v<4;++v) p.set(voiceParam(v,VoiceParam::Enabled),v==0);
    p.set(voiceParam(0,VoiceParam::Semitones),12);
    Fixture f(p,48000,1);std::vector<float> captured;captured.reserve(48000);
    auto render=[&](double source,IEventList* events=nullptr) {
        captured.clear();int position=0;
        while(position<48000) {
            const int frames=std::min(257,48000-position);
            for(int i=0;i<frames;++i) f.left[i]=float(.02*std::sin(2*Pi*source*(position+i)/48000));
            f.process(frames,nullptr,position==0?events:nullptr);
            for(int i=0;i<frames;++i) captured.push_back(f.outLeft[i]+f.outRight[i]);
            position+=frames;
        }
    };
    auto power=[&](double hz) {
        constexpr int count=8192;double real=0,imag=0;
        for(int i=0;i<count;++i) {const double value=captured[captured.size()-count+i]*(.5-.5*std::cos(2*Pi*i/count));real+=value*std::cos(2*Pi*hz*i/48000);imag+=value*std::sin(2*Pi*hz*i/48000);}
        return real*real+imag*imag;
    };
    render(220);require(power(440)>.05 && power(440)>4*power(220),"VST3 adapter produces octave-up harmony from quiet host input");
    ParameterChanges changes;change(changes,voiceParam(0,VoiceParam::Semitones),0);change(changes,Param::MidiMode,1);change(changes,Param::MidiChannel,8);f.process(0,&changes);
    EventList events;Event note{};note.type=Event::kNoteOnEvent;note.busIndex=0;note.sampleOffset=17;
    note.noteOn.channel=7;note.noteOn.pitch=69;note.noteOn.velocity=.8f;events.addEvent(note);
    render(330,&events);require(power(440)>.05 && power(440)>4*power(330),"host MIDI holds A4 while the sung input changes");
    MemoryStream state;require(f.processor.getState(&state)==kResultOk,"save active project state");rewind(state);
    Parameters recalled;require(readState(&state,recalled),"decode active project state");
    near(recalled[Param::MidiMode],1,0,"automation included in project state");
    f.processor.setProcessing(false);f.processor.setActive(false);require(f.processor.setActive(true)==kResultOk,"reactivate plug-in");
    MemoryStream resumed;require(f.processor.getState(&resumed)==kResultOk,"save resumed project state");rewind(resumed);
    require(readState(&resumed,recalled),"read resumed state");near(recalled[Param::MidiMode],1,0,"reactivation preserves last automated settings");
}
void bypassAndAutomation() {
    Parameters p;p.set(Param::Bypass,1);Fixture bypass(p);
    bypass.left[0]=.5f;bypass.right[0]=-.25f;bypass.process(4096);
    const auto delay=bypass.processor.getLatencySamples();require(delay>0 && delay<4096,"latency reported to host");
    for(std::size_t i=0;i<4096;++i) {
        near(bypass.outLeft[i],i==delay?.5:0,1e-7,"bypass preserves left audio and declared latency");
        near(bypass.outRight[i],i==delay?-.25:0,1e-7,"bypass preserves right audio and declared latency");
    }
    Fixture inPlace(p);std::vector<float> expected(4096);
    for(int i=0;i<4096;++i) expected[i]=inPlace.left[i]=inPlace.right[i]=float(.1*std::sin(.02*i));
    inPlace.process(4096,nullptr,nullptr,true);
    for(std::size_t i=0;i<4096;++i) near(inPlace.left[i],i>=delay?expected[i-delay]:0,1e-7,"in-place host buffers remain correct");

    Parameters dry;dry.set(Param::SourceLevel,2);dry.set(Param::HarmonyLevel,0);dry.set(Param::ReverbMix,0);dry.set(Param::AntiRumble,0);
    Fixture automated(dry);std::fill(automated.left.begin(),automated.left.end(),.1f);std::fill(automated.right.begin(),automated.right.end(),.1f);
    ParameterChanges changes;change(changes,Param::OutputGain,0,2048);automated.process(4096,&changes);
    near(automated.outLeft[2047],.1,1e-6,"automation leaves samples before its offset intact");
    near(automated.outLeft[2175],0,1e-7,"automation takes effect at its offset with the gain ramp");
    Fixture invalid(p);std::fill(invalid.left.begin(),invalid.left.end(),std::numeric_limits<float>::quiet_NaN());
    std::fill(invalid.right.begin(),invalid.right.end(),std::numeric_limits<float>::infinity());invalid.process(4096);
    require(invalid.output.silenceFlags==3,"non-finite input becomes silent output");
}
void liveStateAndControls() {
    Fixture fixture;
    Parameters first,second;first.set(Param::HarmonyLevel,.25);first.set(Param::OutputGain,.25);
    second.set(Param::HarmonyLevel,1.5);second.set(Param::OutputGain,1.5);
    MemoryStream a,b;require(writeState(&a,first) && writeState(&b,second),"prepare concurrent state loads");
    std::atomic<bool> good{true};
    std::thread writer([&] {
        for(int i=0;i<500;++i) {
            auto& state=i%2?b:a;state.seek(0,IBStream::kIBSeekSet,nullptr);
            if(fixture.processor.setState(&state)!=kResultOk) good=false;
        }
    });
    for(int i=0;i<500;++i) fixture.process(128);
    writer.join();require(good,"host project loads succeed while rendering");fixture.process(128);
    MemoryStream recalled;require(fixture.processor.getState(&recalled)==kResultOk,"save concurrently restored state");rewind(recalled);
    Parameters result;require(readState(&recalled,result),"decode concurrent recall");
    near(result[Param::HarmonyLevel],1.5,1e-10,"last project state reaches audio thread");
    near(result[Param::OutputGain],1.5,1e-10,"project state stays coherent");
    for(bool maximum:{true,false}) {
        ParameterChanges changes;
        for(std::size_t i=0;i<parameterCount;++i) {
            const auto id=static_cast<Param>(i);if(exposed(id))
                change(changes,id,maximum?parameterInfo()[i].maximum:parameterInfo()[i].initial);
        }
        fixture.process(257,&changes); // Every exposed control must remain allocation-free.
    }
}
}
int main() {
    try {parametersAndState();harmonyAndMidi();bypassAndAutomation();liveStateAndControls();std::cout<<"Plug-in host audio, MIDI, automation, state, bypass and allocation checks passed\n";return 0;}
    catch(const std::exception& error) {trackAllocations=false;std::cerr<<error.what()<<'\n';return 1;}
}
