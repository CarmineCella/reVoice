#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>

namespace revoice {
class Recorder {
    struct Impl;
    std::unique_ptr<Impl> impl_;
public:
    Recorder();
    ~Recorder();
    void start(const std::filesystem::path& path,std::uint32_t sampleRate);
    void stop();
    // Only the audio callback produces samples; disk I/O runs on a worker.
    void push(const float* stereo,std::size_t frames) noexcept;
    bool active() const noexcept;
    bool failed() const noexcept;
    std::uint64_t droppedFrames() const noexcept;
};
} // namespace revoice
