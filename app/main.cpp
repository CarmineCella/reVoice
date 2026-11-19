#include "MainWindow.h"
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <exception>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <stdexcept>

int main(int argc,char** argv) {
    if(argc==2 && std::string(argv[1])=="--audio-smoke-test") {
        try {
            revoice::AudioEngine engine;revoice::AudioSettings settings;revoice::Parameters p;
            p.set(revoice::Param::Source,1);p.set(revoice::Param::OutputGain,0);
            engine.start(settings,p);
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
            const auto stats=engine.stats();engine.stop();
            if(stats.callbackFailed || stats.input<=0 || stats.pitch<200 || stats.pitch>240)
                throw std::runtime_error("Audio callback or pitch integration check failed");
            std::cout<<"Silent device test passed: pitch "<<stats.pitch<<" Hz, CPU "<<100*stats.cpu<<"%, latency "<<stats.latencyMs<<" ms\n";
            return 0;
        }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
    }
    const bool smoke=argc==2 && std::string(argv[1])=="--smoke-test";
    if(argc==2 && std::string(argv[1])=="--version") {std::cout<<"reVoice " REVOICE_VERSION "\n";return 0;}
    try {
        studio::applyTheme();Fl::visual(FL_RGB);Fl::lock();
        MainWindow window(!smoke);window.show();
        struct Smoke { MainWindow* window; int result = 0; } check{&window};
        if(smoke) Fl::add_timeout(.3,[](void* data){
            auto& check = *static_cast<Smoke*>(data);
            try { check.window->validateUI(); std::cout << "Studio UI smoke test passed\n"; }
            catch(const std::exception& error) { std::cerr << error.what() << '\n'; check.result = 1; }
            check.window->hide();
        },&check);
        const int result = Fl::run();
        return smoke ? check.result : result;
    }catch(const std::exception& error) {
        std::cerr<<"reVoice: "<<error.what()<<'\n';
        if(!smoke) fl_alert("%s",error.what());return 1;
    }
}
