// SPDX-License-Identifier: BSD-2-Clause
#include "PluginModel.h"
#include "base/source/fstreamer.h"
#include <algorithm>
#include <cmath>

namespace revoice::plugin {
std::uint32_t parameterId(Param id) {
    static const auto ids=[] {
        std::array<std::uint32_t,parameterCount> result{};
        for(std::size_t i=0;i<parameterCount;++i) {
            std::uint32_t hash=2166136261u;
            for(unsigned char c:parameterInfo()[i].key) {hash^=c;hash*=16777619u;}
            result[i]=hash&0x7fffffffu;
        }
        return result;
    }();
    return ids[static_cast<std::size_t>(id)];
}
bool exposed(Param id) {
    const auto& info=parameterInfo()[static_cast<std::size_t>(id)];
    // Host audio is always the source. Structural settings stay fixed while
    // active; no processor construction or file I/O can occur in process().
    if(info.restart || id==Param::SourceFrequency || id==Param::LoopFile) return false;
    if(info.group=="Detector" && id!=Param::YinGate) return false;
    return true;
}
std::optional<Param> parameterFromId(std::uint32_t id) {
    for(std::size_t i=0;i<parameterCount;++i) {
        const auto p=static_cast<Param>(i);
        if(exposed(p) && parameterId(p)==id) return p;
    }
    return {};
}
bool gainParameter(Param id) {
    if(id==Param::SourceLevel || id==Param::HarmonyLevel || id==Param::MorphLevel || id==Param::OutputGain) return true;
    return id>=Param::VoiceStart && (static_cast<std::size_t>(id)-static_cast<std::size_t>(Param::VoiceStart))%9==static_cast<std::size_t>(VoiceParam::Level);
}
double fromNormalized(Param id,double n) {
    const auto& info=parameterInfo()[static_cast<std::size_t>(id)];
    n=std::isfinite(n)?std::clamp(n,0.0,1.0):toNormalized(id,info.initial);
    if(gainParameter(id)) return n<=0?0:std::pow(10.0,(-60+n*(60+20*std::log10(info.maximum)))/20);
    const double value=info.minimum+n*(info.maximum-info.minimum);
    return info.step==1?std::round(value):value;
}
double toNormalized(Param id,double value) {
    const auto& info=parameterInfo()[static_cast<std::size_t>(id)];
    if(!std::isfinite(value)) value=info.initial;
    if(gainParameter(id)) return value<=0?0:std::clamp((20*std::log10(value)+60)/(60+20*std::log10(info.maximum)),0.0,1.0);
    return std::clamp((value-info.minimum)/(info.maximum-info.minimum),0.0,1.0);
}
bool readState(Steinberg::IBStream* stream,Parameters& parameters) {
    if(!stream) return false;
    Steinberg::IBStreamer reader(stream,kLittleEndian);
    Steinberg::int32 magic=0,version=0,count=0;
    if(!reader.readInt32(magic) || magic!=0x52564331 || !reader.readInt32(version) || version!=1 ||
       !reader.readInt32(count) || count<0 || count>int(parameterCount)) return false;
    Parameters loaded;
    for(int i=0;i<count;++i) {
        Steinberg::int32 id=0;double value=0;
        if(!reader.readInt32(id) || !reader.readDouble(value)) return false;
        if(const auto p=parameterFromId(std::uint32_t(id))) loaded.set(*p,value);
    }
    loaded.set(Param::Source,0);loaded.set(Param::Detector,0);loaded.sanitize();parameters=loaded;
    return true;
}
bool writeState(Steinberg::IBStream* stream,const Parameters& parameters) {
    if(!stream) return false;
    Steinberg::IBStreamer writer(stream,kLittleEndian);
    Steinberg::int32 count=0;
    for(std::size_t i=0;i<parameterCount;++i) if(exposed(static_cast<Param>(i))) ++count;
    if(!writer.writeInt32(0x52564331) || !writer.writeInt32(1) || !writer.writeInt32(count)) return false;
    for(std::size_t i=0;i<parameterCount;++i) {
        const auto p=static_cast<Param>(i);
        if(exposed(p) && (!writer.writeInt32(Steinberg::int32(parameterId(p))) || !writer.writeDouble(parameters[p]))) return false;
    }
    return true;
}
}
