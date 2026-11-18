#pragma once
#include "Parameters.h"
#include "WavFile.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace revoice {
struct Device {int id=-1;std::string name;};
struct AudioSettings {
    int input=-1,output=-1,midiInput=-1,midiOutput=-1;
    double sampleRate=48000;
    unsigned frames=256;
    bool midiThru=false;
};
struct EngineStats {
    float input=0,left=0,right=0,cpu=0;
    double pitch=0,latencyMs=0;
    std::uint64_t xruns=0,droppedRecordingFrames=0,droppedMidiMessages=0;
    bool callbackFailed=false,recordingFailed=false;
};
class AudioEngine {
    struct Impl;
    std::unique_ptr<Impl> impl_;
public:
    AudioEngine();
    ~AudioEngine();
    std::vector<Device> inputs()const;
    std::vector<Device> outputs()const;
    std::vector<Device> midiInputs()const;
    std::vector<Device> midiOutputs()const;
    void start(const AudioSettings& settings,const Parameters& parameters,const AudioFile* source=nullptr);
    void stop();
    bool running()const noexcept;
    bool update(const Parameters& parameters);
    void panic();
    void startRecording(const std::filesystem::path& path);
    void stopRecording();
    bool recording()const noexcept;
    EngineStats stats()const noexcept;
};
} // namespace revoice
