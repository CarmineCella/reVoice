#pragma once
#include "Parameters.h"
#include "WavFile.h"
#include <cstddef>
#include <memory>

namespace revoice {
class Processor {
    struct Impl;
    std::unique_ptr<Impl> impl_;
public:
    Processor(double sampleRate,const Parameters& parameters,const AudioFile* source=nullptr);
    ~Processor();
    Processor(const Processor&)=delete;
    Processor& operator=(const Processor&)=delete;
    // Call on the audio thread, or while it is stopped. Structural changes
    // require a fresh Processor, constructed outside the callback.
    void apply(const Parameters& parameters);
    // input points to the selected channel; stride supports interleaved devices.
    void process(const float* input,float* stereoOutput,std::size_t frames,std::size_t inputStride=1) noexcept;
    void midi(unsigned char status,unsigned char note,unsigned char velocity) noexcept;
    double pitch() const noexcept;
    float inputPeak() const noexcept;
    float leftPeak() const noexcept;
    float rightPeak() const noexcept;
};
} // namespace revoice
