#include "revoice/Recorder.h"
#include "revoice/SpscQueue.h"
#include "revoice/WavFile.h"
#include <array>
#include <chrono>
#include <thread>

namespace revoice {
struct Recorder::Impl {
    struct Frame {float left=0,right=0;};
    SpscQueue<Frame,131072> queue;
    std::atomic<bool> enabled{false},producerBusy{false},finish{false},error{false};
    std::atomic<std::uint64_t> dropped{0};
    std::thread worker;
    std::unique_ptr<WavWriter> writer;
};
Recorder::Recorder():impl_(std::make_unique<Impl>()) {}
Recorder::~Recorder(){stop();}
void Recorder::start(const std::filesystem::path& path,std::uint32_t rate) {
    stop();
    auto& s=*impl_;
    s.queue.reset();s.dropped=0;s.error=false;s.finish=false;
    s.writer=std::make_unique<WavWriter>(path,rate,2);
    s.worker=std::thread([&s]{
        try {
            std::array<float,8192> samples{};
            for(;;) {
                std::size_t count=0;Impl::Frame frame;
                while(count+2<=samples.size() && s.queue.pop(frame)) {
                    samples[count++]=frame.left;samples[count++]=frame.right;
                }
                if(count) s.writer->write(samples.data(),count);
                else if(s.finish.load(std::memory_order_acquire)) break;
                else std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }
            s.writer->close();
        }catch(...){s.error=true;s.enabled=false;}
    });
    s.enabled.store(true,std::memory_order_seq_cst);
}
void Recorder::stop() {
    auto& s=*impl_;
    s.enabled.store(false,std::memory_order_seq_cst);
    // A callback that observed 'enabled' before this store must finish before
    // the writer drains its final samples or the queue is reset for a new take.
    while(s.producerBusy.load(std::memory_order_seq_cst)) std::this_thread::yield();
    s.finish.store(true,std::memory_order_release);
    if(s.worker.joinable()) s.worker.join();
    s.writer.reset();
}
void Recorder::push(const float* stereo,std::size_t frames) noexcept {
    auto& s=*impl_;
    s.producerBusy.store(true,std::memory_order_seq_cst);
    if(s.enabled.load(std::memory_order_seq_cst)) {
        std::uint64_t dropped=0;
        for(std::size_t i=0;i<frames;++i)
            if(!s.queue.push({stereo[2*i],stereo[2*i+1]})) ++dropped;
        if(dropped) s.dropped.fetch_add(dropped,std::memory_order_relaxed);
    }
    s.producerBusy.store(false,std::memory_order_seq_cst);
}
bool Recorder::active()const noexcept{return impl_->enabled.load();}
bool Recorder::failed()const noexcept{return impl_->error.load();}
std::uint64_t Recorder::droppedFrames()const noexcept{return impl_->dropped.load();}
} // namespace revoice
