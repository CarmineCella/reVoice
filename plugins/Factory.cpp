// SPDX-License-Identifier: BSD-2-Clause
#include "VstProcessor.h"
#include "VstController.h"
#include "Ids.h"
#include "public.sdk/source/main/pluginfactory_constexpr.h"

BEGIN_FACTORY_DEF("Carmine Emanuele Cella","https://www.carminecella.com","",2)
DEF_CLASS(revoice::plugin::ProcessorID,Steinberg::PClassInfo::kManyInstances,kVstAudioEffectClass,
    "reVoice",Steinberg::Vst::kDistributable,"Fx|Pitch Shift",REVOICE_VERSION,kVstVersionString,
    revoice::plugin::VstProcessor::create,nullptr)
DEF_CLASS(revoice::plugin::ControllerID,Steinberg::PClassInfo::kManyInstances,kVstComponentControllerClass,
    "reVoice Controller",0,"",REVOICE_VERSION,kVstVersionString,revoice::plugin::VstController::create,nullptr)
END_FACTORY
