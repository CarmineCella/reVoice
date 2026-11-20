#pragma once
#include "StudioWidgets.h"
#include "revoice/AudioEngine.h"
#include <FL/Fl_Double_Window.H>
#include <array>
#include <memory>
#include <string>
#include <vector>

class Fl_Box;
class MainWindow final : public Fl_Double_Window {
    struct Binding { MainWindow* owner = nullptr; revoice::Param id{}; };
    struct VoiceLink { MainWindow* owner = nullptr; int page = 0; };
    revoice::Parameters parameters_, sent_;
    revoice::AudioSettings settings_;
    revoice::AudioFile file_;
    std::string filePath_;
    std::unique_ptr<revoice::AudioEngine> engine_;
    std::array<std::vector<Fl_Widget*>, revoice::parameterCount> controls_;
    std::array<Binding, revoice::parameterCount> bindings_{};
    std::array<VoiceLink, 4> voiceLinks_{};
    std::vector<revoice::Device> inputs_, outputs_, midiInputs_, midiOutputs_;
    studio::PageStack *pages_ = nullptr, *voicePages_ = nullptr;
    std::array<studio::Button*, 6> navigation_{};
    std::array<studio::Button*, 5> voiceNavigation_{};
    studio::Button *start_ = nullptr, *record_ = nullptr;
    studio::Toggle *morphEnabled_ = nullptr, *reverbEnabled_ = nullptr, *thru_ = nullptr;
    studio::PitchDisplay* pitch_ = nullptr;
    studio::Meter *inputMeter_ = nullptr, *outputMeter_ = nullptr;
    Fl_Box *status_ = nullptr, *sourceFile_ = nullptr, *audioState_ = nullptr;
    studio::Choice *input_ = nullptr, *output_ = nullptr, *midiInput_ = nullptr,
                   *midiOutput_ = nullptr, *rate_ = nullptr, *buffer_ = nullptr, *inputChannel_ = nullptr;
    bool persistence_ = true, pending_ = false, recordErrorShown_ = false;
    int page_ = 0, voicePage_ = 0;
    double lastMorphLevel_ = .25, lastReverbMix_ = .15;
    void makeHeader();
    void makeStudio();
    void makeVoices();
    void makeDevices();
    void makeParameterPage(const char*, const char*, const std::vector<std::string>&);
    Fl_Widget* parameterControl(revoice::Param id, bool dial = false);
    Fl_Flex* parameterRow(const char* label, revoice::Param id, bool number = false);
    Fl_Widget* bind(Fl_Widget*, revoice::Param);
    void showPage(int);
    void showVoice(int);
    void syncControls();
    void parameterChanged(Fl_Widget*, revoice::Param);
    void applyParameters();
    void startAudio();
    void toggleAudio();
    void toggleRecording();
    void chooseSource();
    void loadPreset();
    void savePreset();
    void restoreHarmonyMix();
    void loadPreferences();
    void savePreferences();
    void applyDevices();
    void refreshInputChannels();
    revoice::AudioSettings selectedDevices() const;
    void closeWindow();
    void tick();
    void transportState();
    static void timer(void*);
    static void parameterCallback(Fl_Widget*, void*);
public:
    explicit MainWindow(bool persistence = true);
    ~MainWindow() override;
    int handle(int event) override;
    void validateUI();
};
