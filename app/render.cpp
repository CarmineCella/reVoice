#include "revoice/Processor.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc,char** argv) {
    try {
        if(argc<2 || std::string(argv[1])=="--help") {
            std::cout << "reVoice offline renderer 0.1.0\n"
                "Usage: revoice-render input.wav output.wav [transpose-semitones]\n"
                "       revoice-render --tone output.wav [frequency-hz] [seconds]\n";
            return argc<2?1:0;
        }
        revoice::Parameters p; revoice::AudioFile file;
        const bool tone=std::string(argv[1])=="--tone";
        if(argc<3 || argc>(tone?5:4)) throw std::runtime_error("Invalid arguments; use --help");
        double rate=48000,seconds=2;
        std::string output=argv[2];
        if(tone) {
            p.set(revoice::Param::Source,1);
            if(argc>3) p.set(revoice::Param::SourceFrequency,std::stod(argv[3]));
            if(argc>4) seconds=std::stod(argv[4]);
        } else {
            file=revoice::readWav(std::filesystem::u8path(argv[1])); rate=file.sampleRate;
            seconds=double(file.mono.size())/rate;
            p.set(revoice::Param::Source,3); p.set(revoice::Param::LoopFile,0);
            if(argc>3) p.set(revoice::Param::Transpose,std::stod(argv[3]));
        }
        if(!std::isfinite(seconds) || seconds<=0 || seconds>3600) throw std::runtime_error("Duration must be between 0 and 3600 seconds");
        revoice::Processor processor(rate,p,tone?nullptr:&file);
        revoice::WavWriter writer(std::filesystem::u8path(output),static_cast<std::uint32_t>(rate));
        std::array<float,512> block{};
        auto remaining=static_cast<std::size_t>(std::ceil(seconds*rate));
        while(remaining) {
            const auto frames=std::min<std::size_t>(remaining,block.size()/2);
            processor.process(nullptr,block.data(),frames); writer.write(block.data(),frames*2);
            remaining-=frames;
        }
        writer.close();
        std::cout << "Rendered " << seconds << " seconds at " << rate << " Hz to " << output << '\n';
        return 0;
    } catch(const std::exception& error) { std::cerr << "reVoice: " << error.what() << '\n'; return 1; }
}
