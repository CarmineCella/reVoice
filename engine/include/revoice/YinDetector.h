// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include <array>
#include <cstddef>
#include <vector>

namespace revoice {
// Streaming YIN. Uses only the C++ standard library. All storage is allocated
// during construction; process(), reset() and gate() do not allocate or lock.
class YinDetector {
    struct Biquad {
        double b0=0,b1=0,b2=0,a1=0,a2=0,z1=0,z2=0;
        double process(double sample) noexcept;
    };
    std::array<Biquad,2> filters_{};
    std::vector<double> ring_,samples_,difference_;
    std::size_t cursor_=0,filled_=0,untilAnalysis_=0,hop_=0,window_=0,minLag_=0,maxLag_=0;
    unsigned decimation_=1,decimationPhase_=0;
    double analysisRate_=0,minimum_=0,maximum_=0,gate_=0,pitch_=0,confidence_=0;
    void analyze() noexcept;
public:
    YinDetector(double sampleRate,double minimum=60,double maximum=1400,double rmsGate=.003);
    double process(double sample) noexcept;
    double frequency() const noexcept { return pitch_; }
    double confidence() const noexcept { return confidence_; }
    double minimum() const noexcept { return minimum_; }
    double maximum() const noexcept { return maximum_; }
    double latencySeconds() const noexcept { return double(ring_.size()+hop_)/analysisRate_; }
    void gate(double rms) noexcept;
    void reset() noexcept;
};
} // namespace revoice
