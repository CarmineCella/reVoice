#pragma once
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>

namespace revoice {
struct AudioFile {
    std::uint32_t sampleRate=0;
    std::vector<float> mono;
};
// Reads PCM 8/16/24/32 or IEEE float 32/64 WAV, downmixing channels to mono.
// Parsing and allocation happen outside the audio callback.
AudioFile readWav(const std::filesystem::path& path);
class WavWriter {
    std::ofstream file_;
    std::uint32_t sampleRate_=0;
    std::uint16_t channels_=0;
    std::uint64_t samples_=0;
    void header();
public:
    WavWriter(const std::filesystem::path& path,std::uint32_t sampleRate,std::uint16_t channels=2);
    ~WavWriter();
    WavWriter(const WavWriter&)=delete;
    WavWriter& operator=(const WavWriter&)=delete;
    void write(const float* interleaved,std::size_t sampleCount);
    void close();
};
} // namespace revoice
