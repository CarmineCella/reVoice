// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include "revoice/Processor.h"
#include "public.sdk/source/vst/vstaudioeffect.h"
#include <array>
#include <atomic>
#include <memory>
#include <vector>

namespace revoice::plugin {
class VstProcessor final : public Steinberg::Vst::AudioEffect {
    Parameters parameters_;
    std::array<std::atomic<double>,parameterCount> state_{};
    std::atomic<std::uint64_t> stateSerial_{0};
    std::uint64_t appliedSerial_=0;
    std::unique_ptr<revoice::Processor> processor_;
    std::vector<float> mono_,wet_,dryLeft_,dryRight_,delayLeft_,delayRight_;
    std::size_t delayCursor_=0;
    double bypassMix_=0;
    void applyParameters();
    void render(Steinberg::Vst::ProcessData& data,int offset,int frames) noexcept;
public:
    VstProcessor();
    static Steinberg::FUnknown* create(void*) {return static_cast<Steinberg::Vst::IAudioProcessor*>(new VstProcessor);}
    Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) override;
    Steinberg::tresult PLUGIN_API setBusArrangements(Steinberg::Vst::SpeakerArrangement* inputs,Steinberg::int32 numIns,
        Steinberg::Vst::SpeakerArrangement* outputs,Steinberg::int32 numOuts) override;
    Steinberg::tresult PLUGIN_API canProcessSampleSize(Steinberg::int32 size) override;
    Steinberg::tresult PLUGIN_API setupProcessing(Steinberg::Vst::ProcessSetup& setup) override;
    Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool active) override;
    Steinberg::tresult PLUGIN_API setProcessing(Steinberg::TBool processing) override;
    Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override;
    Steinberg::uint32 PLUGIN_API getLatencySamples() override;
    Steinberg::uint32 PLUGIN_API getTailSamples() override;
    Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream* stream) override;
    Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream* stream) override;
};
}
