// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include "public.sdk/source/vst/vsteditcontroller.h"
namespace revoice::plugin {
class VstController final : public Steinberg::Vst::EditController {
public:
    static Steinberg::FUnknown* create(void*) { return static_cast<Steinberg::Vst::IEditController*>(new VstController); }
    Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) override;
    Steinberg::tresult PLUGIN_API setComponentState(Steinberg::IBStream* state) override;
};
}
