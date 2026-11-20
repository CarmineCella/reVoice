// SPDX-License-Identifier: BSD-2-Clause
#include "VstController.h"
#include "PluginModel.h"
#include "pluginterfaces/base/ustring.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <vector>

namespace revoice::plugin {
using namespace Steinberg;
using namespace Steinberg::Vst;
namespace {
class HostParameter final : public Parameter {
    Param parameter_;
    std::vector<std::string> labels_;
public:
    explicit HostParameter(Param id):parameter_(id) {
        const auto& meta=parameterInfo()[static_cast<std::size_t>(id)];
        info.id=parameterId(id);info.flags=Steinberg::Vst::ParameterInfo::kCanAutomate;
        info.stepCount=meta.step==1?int32(meta.maximum-meta.minimum):0;
        if(meta.choices) {
            std::istringstream list(meta.choices);std::string label;
            while(std::getline(list,label,'|')) labels_.push_back(label);
            info.flags|=Steinberg::Vst::ParameterInfo::kIsList;
        } else if(meta.minimum==0 && meta.maximum==1 && meta.step==1) labels_={"Off","On"};
        if(id==Param::Bypass) info.flags|=Steinberg::Vst::ParameterInfo::kIsBypass;
        const auto title=meta.group+": "+meta.label;
        UString(info.title,128).fromAscii(title.c_str());
        UString(info.shortTitle,128).fromAscii(meta.label.c_str());
        UString(info.units,128).fromAscii(gainParameter(id)?"dB":"");
        info.defaultNormalizedValue=plugin::toNormalized(id,meta.initial);
        valueNormalized=info.defaultNormalizedValue;info.unitId=kRootUnitId;
    }
    ParamValue toPlain(ParamValue n) const override {
        const double value=fromNormalized(parameter_,n);
        return gainParameter(parameter_)?(value>0?20*std::log10(value):-60):value;
    }
    ParamValue toNormalized(ParamValue plain) const override {
        return plugin::toNormalized(parameter_,gainParameter(parameter_)?(plain<=-60?0:std::pow(10.0,plain/20)):plain);
    }
    void toString(ParamValue n,String128 string) const override {
        const auto& meta=parameterInfo()[static_cast<std::size_t>(parameter_)];
        if(!labels_.empty()) {
            const auto index=std::size_t(std::clamp(int(fromNormalized(parameter_,n)-meta.minimum),0,int(labels_.size())-1));
            UString(string,128).fromAscii(labels_[index].c_str());return;
        }
        char number[64];
        if(gainParameter(parameter_) && n<=0) std::snprintf(number,sizeof(number),"-inf");
        else std::snprintf(number,sizeof(number),meta.step==1?"%.0f":meta.step<.001?"%.4f":"%.2f",toPlain(n));
        UString(string,128).fromAscii(number);
    }
    bool fromString(const TChar* string,ParamValue& n) const override {
        char text[256]{};UString(const_cast<TChar*>(string),128).toAscii(text,256);
        const auto& meta=parameterInfo()[static_cast<std::size_t>(parameter_)];
        for(std::size_t i=0;i<labels_.size();++i) if(labels_[i]==text) {n=plugin::toNormalized(parameter_,meta.minimum+i);return true;}
        if(gainParameter(parameter_) && std::string(text)=="-inf") {n=0;return true;}
        char* end=nullptr;const double value=std::strtod(text,&end);
        if(end==text || !std::isfinite(value)) return false;
        n=toNormalized(value);return true;
    }
};
}
tresult PLUGIN_API VstController::initialize(FUnknown* context) {
    const auto result=EditController::initialize(context);if(result!=kResultOk) return result;
    for(std::size_t i=0;i<parameterCount;++i) {
        const auto id=static_cast<Param>(i);
        if(exposed(id)) parameters.addParameter(new HostParameter(id));
    }
    return kResultOk;
}
tresult PLUGIN_API VstController::setComponentState(IBStream* stream) {
    Parameters loaded;if(!readState(stream,loaded)) return kResultFalse;
    for(std::size_t i=0;i<parameterCount;++i) {
        const auto id=static_cast<Param>(i);
        if(exposed(id)) setParamNormalized(parameterId(id),plugin::toNormalized(id,loaded[id]));
    }
    return kResultOk;
}
}
