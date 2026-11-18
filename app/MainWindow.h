#pragma once
#include "revoice/AudioEngine.h"
#include <FL/Fl_Double_Window.H>
#include <array>
#include <memory>
#include <string>
#include <vector>

class Fl_Widget;
class Fl_Box;
class Fl_Button;
class Fl_Choice;
class Fl_Check_Button;
class Fl_Progress;
class Fl_Scroll;
class Fl_Group;

class MainWindow final : public Fl_Double_Window {
    struct Binding {MainWindow* owner=nullptr;revoice::Param id{};};
    revoice::Parameters parameters_,sent_;
    revoice::AudioSettings settings_;
    revoice::AudioFile file_;
    std::string filePath_;
    std::unique_ptr<revoice::AudioEngine> engine_;
    std::array<Fl_Widget*,revoice::parameterCount> controls_{};
    std::array<Binding,revoice::parameterCount> bindings_{};
    std::vector<revoice::Device> inputs_,outputs_,midiInputs_,midiOutputs_;
    Fl_Button *start_=nullptr,*record_=nullptr;
    Fl_Box *status_=nullptr,*sourceFile_=nullptr;
    Fl_Progress *inputMeter_=nullptr,*leftMeter_=nullptr,*rightMeter_=nullptr;
    Fl_Choice *input_=nullptr,*output_=nullptr,*midiInput_=nullptr,*midiOutput_=nullptr,*rate_=nullptr,*buffer_=nullptr;
    Fl_Check_Button* thru_=nullptr;
    bool persistence_=true,pending_=false,recordErrorShown_=false;
    void makeParameterTab(const char* title,const std::vector<std::string>& groups);
    void makeParameterControls(Fl_Scroll* scroll,const std::vector<std::string>& groups);
    void makeDevicesTab();
    void syncControls();
    void parameterChanged(revoice::Param id);
    void applyParameters();
    void startAudio();
    void toggleAudio();
    void toggleRecording();
    void chooseSource();
    void loadPreset();
    void savePreset();
    void loadPreferences();
    void savePreferences();
    void applyDevices();
    void closeWindow();
    void tick();
    static void timer(void*);
    static void parameterCallback(Fl_Widget*,void*);
public:
    explicit MainWindow(bool persistence=true);
    ~MainWindow() override;
};
