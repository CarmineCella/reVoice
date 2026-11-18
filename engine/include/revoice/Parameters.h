#pragma once

#include <array>
#include <cstddef>
#include <string>

namespace revoice {

enum class Param : std::size_t {
    Source, SourceFrequency, InputGain, LoopFile, DelayMs,
    SourceLevel, HarmonyLevel, MorphLevel, ReverbMix, ReverbTime, OutputGain,
    Bypass, Freeze, AntiRumble, Logic, Key, Scale, Transpose,
    Window, Attack, Decay, Cycles, AutoGrains, PitchChange,
    Detector, MinimumPitch, MaximumPitch, Threshold, FilterQuality,
    SmoothingRate, SmoothingQuality, FilterLevels, DetectorSplit, Feedback, FftSize,
    MorphDensity, MorphRandomDensity, MorphDuration, MorphRandomDuration,
    MorphFrequency, MorphRandomFrequency, MorphGliss, MorphRandomGliss,
    MorphWindow, MorphPan, MorphSpeed, MorphJitter, MorphAmplitude,
    MorphReverse, MorphGrainlet, MorphStart, MorphEnd, MorphAttack, MorphDecay,
    MidiMode, MidiChannel,
    VoiceStart,
    Count = VoiceStart + 4 * 9
};
enum class VoiceParam : std::size_t {
    Enabled, Highpass, Level, Semitones, VibratoDepth, VibratoRate,
    RandomVibrato, Pan, Formant
};
constexpr Param voiceParam(int voice, VoiceParam field) {
    return static_cast<Param>(static_cast<std::size_t>(Param::VoiceStart) +
                              static_cast<std::size_t>(voice) * 9 + static_cast<std::size_t>(field));
}
constexpr std::size_t parameterCount = static_cast<std::size_t>(Param::Count);
struct ParameterInfo {
    std::string key;
    std::string label;
    std::string group;
    double minimum = 0, maximum = 1, initial = 0, step = 0.01;
    const char* choices = nullptr;
    bool restart = false;
};
const std::array<ParameterInfo, parameterCount>& parameterInfo();

struct Parameters {
    std::array<double, parameterCount> values{};
    Parameters();
    double operator[](Param id) const { return values[static_cast<std::size_t>(id)]; }
    double& operator[](Param id) { return values[static_cast<std::size_t>(id)]; }
    void set(Param id, double value);
    void sanitize();
    bool needsRestart(const Parameters& other) const;
};
} // namespace revoice
