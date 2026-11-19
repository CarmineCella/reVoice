#include "MainWindow.h"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Value_Input.H>
#include <FL/fl_ask.H>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>

using namespace revoice;
namespace {
Fl_Box* text(const char* label, int size = 14, bool bold = false, Fl_Color color = studio::Text) {
    auto* box = new Fl_Box(0, 0, 100, 24, label);
    box->box(FL_NO_BOX); box->labelsize(size); box->labelfont(bold ? FL_HELVETICA_BOLD : FL_HELVETICA);
    box->labelcolor(color); box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE); return box;
}
Fl_Flex* row(int gap = 10) {
    auto* flex = new Fl_Flex(0, 0, 100, 36, Fl_Flex::ROW);
    flex->box(FL_NO_BOX); flex->color(studio::Surface); flex->gap(gap); return flex;
}
Fl_Flex* column(int gap = 8) {
    auto* flex = new Fl_Flex(0, 0, 100, 100, Fl_Flex::COLUMN);
    flex->box(FL_NO_BOX); flex->color(studio::Surface); flex->gap(gap); return flex;
}
Fl_Box* spacer() { return new Fl_Box(0, 0, 1, 1); }
void separator(Fl_Flex* parent) {
    auto* line = new Fl_Box(0, 0, 1, 1); line->box(FL_FLAT_BOX); line->color(studio::Border); parent->fixed(line, 1);
}
std::string menuEscape(const std::string& name) {
    std::string result;
    for (char c : name) { if (c == '/' || c == '\\' || c == '&' || c == '_') result += '\\'; result += c; }
    return result;
}
void fillDevices(studio::Choice* choice, const std::vector<Device>& devices) {
    for (const auto& device : devices) choice->add(menuEscape(device.name).c_str());
    choice->value(0);
}
int deviceId(Fl_Choice* choice, const std::vector<Device>& devices) {
    const int i = choice->value(); return i >= 0 && i < int(devices.size()) ? devices[i].id : -1;
}
std::string deviceName(Fl_Choice* choice, const std::vector<Device>& devices) {
    const int i = choice->value(); return i >= 0 && i < int(devices.size()) ? devices[i].name : "";
}
std::string chooseFile(const char* title, int type, const char* filter) {
    Fl_Native_File_Chooser dialog; dialog.title(title); dialog.type(type); dialog.filter(filter);
    if (type == Fl_Native_File_Chooser::BROWSE_SAVE_FILE)
        dialog.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | Fl_Native_File_Chooser::USE_FILTER_EXT);
    if (dialog.show() != 0 || !dialog.filename()) return {};
    return dialog.filename();
}
studio::Units unitsFor(Param id) {
    if (id == Param::SourceLevel || id == Param::HarmonyLevel || id == Param::MorphLevel || id == Param::OutputGain)
        return studio::Units::Decibels;
    if (id == Param::ReverbMix) return studio::Units::Percent;
    if (id == Param::ReverbTime || id == Param::MorphDuration) return studio::Units::Seconds;
    if (id == Param::SourceFrequency) return studio::Units::Hertz;
    if (id == Param::MorphFrequency || id == Param::MorphGliss) return studio::Units::Ratio;
    if (id == Param::Transpose) return studio::Units::Semitones;
    const auto n = static_cast<std::size_t>(id), start = static_cast<std::size_t>(Param::VoiceStart);
    if (n >= start) {
        const auto field = static_cast<VoiceParam>((n - start) % 9);
        if (field == VoiceParam::Level) return studio::Units::Decibels;
        if (field == VoiceParam::Pan) return studio::Units::Pan;
        if (field == VoiceParam::Formant) return studio::Units::Ratio;
        if (field == VoiceParam::Semitones) return studio::Units::Semitones;
    }
    return studio::Units::Number;
}
class ParameterScroll final : public Fl_Scroll {
    Fl_Flex* content_;
    int height_ = 100;
public:
    ParameterScroll() : Fl_Scroll(0, 0, 600, 500) {
        type(Fl_Scroll::VERTICAL); box(FL_FLAT_BOX); color(studio::Background);
        scrollbar.color(studio::Background); scrollbar.selection_color(studio::Track);
        content_ = column(10); content_->color(studio::Background);
    }
    Fl_Flex* content() { return content_; }
    void finish(int height) { height_ = height; content_->end(); end(); }
    void resize(int x, int y, int w, int h) override {
        Fl_Scroll::resize(x, y, w, h);
        content_->resize(x + 1, y + 1 - yposition(), std::max(100, w - 18), height_);
    }
};
void showError(const std::exception& error) { fl_alert("%s", error.what()); }
}

MainWindow::MainWindow(bool persistence)
    : Fl_Double_Window(1320, 860, "reVoice " REVOICE_VERSION), persistence_(persistence) {
    color(studio::Background);
    for (std::size_t i = 0; i < bindings_.size(); ++i) bindings_[i] = {this, static_cast<Param>(i)};
    engine_ = std::make_unique<AudioEngine>();
    inputs_ = engine_->inputs(); outputs_ = engine_->outputs();
    midiInputs_ = engine_->midiInputs(); midiOutputs_ = engine_->midiOutputs();
    begin();
    auto* shell = new Fl_Flex(0, 0, w(), h(), Fl_Flex::COLUMN);
    shell->box(FL_FLAT_BOX); shell->color(studio::Background);
    makeHeader(); shell->fixed(shell->child(0), 84);
    auto* body = row(0); body->color(studio::Background);
    auto* sidebar = column(8); sidebar->box(FL_FLAT_BOX); sidebar->color(studio::Sidebar);
    sidebar->margin(12, 20, 12, 16); body->fixed(sidebar, 168);
    static const char* labels[] = {"Studio", "Voices", "Morphizer", "Pitch detector", "Audio / MIDI", "Input settings"};
    static const studio::Icon icons[] = {studio::Icon::Studio, studio::Icon::Voices, studio::Icon::Morphizer,
        studio::Icon::Pitch, studio::Icon::Audio, studio::Icon::Edit};
    for (int i = 0; i < 5; ++i) {
        auto* button = new studio::Button(labels[i], icons[i]); button->navigation(true);
        button->user_data(reinterpret_cast<void*>(static_cast<std::intptr_t>(i)));
        button->callback([](Fl_Widget* widget, void*) {
            static_cast<MainWindow*>(widget->window())->showPage(int(reinterpret_cast<std::intptr_t>(widget->user_data())));
        });
        navigation_[i] = button; sidebar->fixed(button, 46);
    }
    spacer(); separator(sidebar);
    navigation_[5] = new studio::Button(labels[5], icons[5]); navigation_[5]->navigation(true);
    navigation_[5]->callback([](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->showPage(5); }, this);
    sidebar->fixed(navigation_[5], 42); sidebar->end();
    pages_ = new studio::PageStack();
    makeStudio(); makeVoices();
    makeParameterPage("Morphizer", "Asynchronous grains, pitch movement, and texture.", {"Morphizer"});
    makeParameterPage("Pitch detector", "Tune the detector to your voice. Structural changes restart audio.", {"Detector"});
    makeDevices();
    makeParameterPage("Source & mix", "Set your input, source playback, and master mix.", {"Source", "Mix"});
    pages_->end(); body->end();
    auto* footer = row(); footer->box(FL_FLAT_BOX); footer->color(studio::Sidebar); footer->margin(22, 0, 22, 0);
    status_ = text("Stopped  |  Choose a source and start audio", 12, false, studio::Muted);
    footer->end(); shell->fixed(footer, 38); shell->end();
    end(); resizable(shell); size_range(1180, 760);
    callback([](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->closeWindow(); }, this);
    if (persistence_) loadPreferences();
    sent_ = parameters_; syncControls(); showPage(0); showVoice(0); transportState();
    int sx, sy, sw, sh; Fl::screen_work_area(sx, sy, sw, sh);
    size(std::min(w(), std::max(1180, sw - 60)), std::min(h(), std::max(760, sh - 80)));
    position(sx + std::max(0, (sw - w()) / 2), sy + std::max(0, (sh - h()) / 2));
    Fl::add_timeout(.05, timer, this);
}
MainWindow::~MainWindow() { Fl::remove_timeout(timer, this); engine_->stop(); }
Fl_Widget* MainWindow::bind(Fl_Widget* widget, Param id) {
    const auto i = static_cast<std::size_t>(id);
    controls_[i].push_back(widget); widget->callback(parameterCallback, &bindings_[i]);
    if (parameterInfo()[i].restart) widget->tooltip("Changing this setting restarts audio. Stop recording first.");
    return widget;
}
Fl_Widget* MainWindow::parameterControl(Param id, bool dial) {
    const auto& info = parameterInfo()[static_cast<std::size_t>(id)];
    if (!dial && (info.choices || id == Param::MidiChannel)) {
        auto* choice = new studio::Choice(info.label.c_str());
        if (info.choices) choice->add(info.choices);
        else for (int i = int(info.minimum); i <= int(info.maximum); ++i) choice->add(std::to_string(i).c_str());
        return bind(choice, id);
    }
    if (!dial && info.minimum == 0 && info.maximum == 1 && info.step == 1)
        return bind(new studio::Toggle(info.label.c_str(), true), id);
    auto* value = new studio::Value(info.label.c_str(), dial ? studio::Value::Style::Dial : studio::Value::Style::Slider, unitsFor(id));
    value->bounds(info.minimum, info.maximum); value->step(info.step); return bind(value, id);
}
Fl_Flex* MainWindow::parameterRow(const char* label, Param id, bool number) {
    auto* line = row(6); line->fixed(text(label, 12), number ? 62 : 44);
    if (number) {
        const auto& info = parameterInfo()[static_cast<std::size_t>(id)];
        auto* input = new Fl_Value_Input(0, 0, 100, 30);
        input->bounds(info.minimum, info.maximum); input->step(info.step); input->soft(0);
        input->box(studio::InputBox); input->color(studio::Control); input->textcolor(studio::Text); input->textsize(13);
        input->input.box(studio::InputBox); input->input.color(studio::Control); input->input.textcolor(studio::Text); input->input.selection_color(studio::Mint);
        bind(input, id);
    } else parameterControl(id);
    line->end(); return line;
}
void MainWindow::makeHeader() {
    auto* header = row(10); header->color(studio::Background); header->margin(22, 20, 22, 20);
    header->fixed(text("reVoice", 30, true), 146); separator(header);
    parameterControl(Param::Logic)->tooltip("Harmony preset. Choose Custom chord to edit each voice interval.");
    auto* load = new studio::Button("", studio::Icon::Folder); load->tooltip("Load preset (Cmd/Ctrl+L)");
    load->callback([](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->loadPreset(); }, this); header->fixed(load, 36);
    auto* save = new studio::Button("", studio::Icon::Save); save->tooltip("Save preset (Cmd/Ctrl+S)");
    save->callback([](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->savePreset(); }, this); header->fixed(save, 36);
    header->fixed(parameterControl(Param::Source), 140);
    audioState_ = text("Stopped", 12, true, studio::Muted); header->fixed(audioState_, 68);
    start_ = new studio::Button("Start audio", studio::Icon::Play); start_->primary(true); header->fixed(start_, 116);
    start_->callback([](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->toggleAudio(); }, this);
    record_ = new studio::Button("Record", studio::Icon::Record); header->fixed(record_, 96);
    record_->callback([](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->toggleRecording(); }, this);
    auto* menu = new studio::Menu(); header->fixed(menu, 36);
    menu->add("Open WAV source...", FL_COMMAND + 'o', [](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->chooseSource(); }, this);
    menu->add("Load preset...", FL_COMMAND + 'l', [](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->loadPreset(); }, this);
    menu->add("Save preset...", FL_COMMAND + 's', [](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->savePreset(); }, this, FL_MENU_DIVIDER);
    menu->add("Input settings", 0, [](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->showPage(5); }, this);
    menu->add("All notes off", FL_F + 10, [](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->engine_->panic(); }, this, FL_MENU_DIVIDER);
    menu->add("About reVoice", 0, [](Fl_Widget*, void*) {
        fl_message("reVoice " REVOICE_VERSION "\nReal-time vocal harmonizer\nCarmine Emanuele Cella\n\nBSD 2-Clause application; Pandora LGPL 2.1.\nBased in part on the work of the FLTK project.\nAudio: PortAudio. MIDI: PortMidi. Reverb: STK NRev.");
    });
    menu->add("Quit", FL_COMMAND + 'q', [](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->closeWindow(); }, this);
    header->end();
}
void MainWindow::makeStudio() {
    auto* page = column(14); page->color(studio::Background); page->margin(20);
    pitch_ = new studio::PitchDisplay(); page->fixed(pitch_, 120);
    auto* voices = row(12); voices->color(studio::Background);
    for (int voice = 0; voice < 4; ++voice) {
        auto* card = new studio::Panel(); card->margin(14); card->gap(8);
        auto* heading = row(4); auto* title = text("", 19, true);
        title->copy_label(("Voice " + std::to_string(voice + 1)).c_str());
        auto* enabled = new studio::Toggle("Enabled"); bind(enabled, voiceParam(voice, VoiceParam::Enabled));
        heading->fixed(enabled, 38); heading->end(); card->fixed(heading, 30);
        auto* interval = row(6); interval->fixed(text("Interval", 12), 48);
        auto* choice = new studio::Choice("Voice interval");
        static const char* names[] = {"Unison", "m2", "M2", "m3", "M3", "4th", "Tritone", "5th", "m6", "M6", "m7", "M7"};
        for (int st = -24; st <= 24; ++st) {
            char label[80];
            if (st == 0) std::snprintf(label, sizeof(label), "0 st (Unison)");
            else std::snprintf(label, sizeof(label), "%+d st (%s)", st, std::abs(st) % 12 == 0 ? "Octave" : names[std::abs(st) % 12]);
            choice->add(label);
        }
        bind(choice, voiceParam(voice, VoiceParam::Semitones));
        choice->tooltip("Custom interval. Select Custom chord in the header to edit.");
        interval->end(); card->fixed(interval, 32);
        card->fixed(parameterRow("Level", voiceParam(voice, VoiceParam::Level)), 32);
        card->fixed(parameterRow("Pan", voiceParam(voice, VoiceParam::Pan)), 32);
        card->fixed(parameterRow("Formant x", voiceParam(voice, VoiceParam::Formant), true), 32);
        spacer(); separator(card);
        auto* edit = new studio::Button("Advanced edit", studio::Icon::Edit); voiceLinks_[voice] = {this, voice + 1};
        edit->callback([](Fl_Widget*, void* p) { const auto& link = *static_cast<VoiceLink*>(p); link.owner->showPage(1); link.owner->showVoice(link.page); }, &voiceLinks_[voice]);
        card->fixed(edit, 34); card->end();
    }
    voices->end();
    auto* mix = new studio::Panel(Fl_Flex::ROW); mix->margin(14); mix->gap(12); page->fixed(mix, 176);
    auto* original = column(4); original->fixed(text("Original", 17, true), 24);
    auto* dry = row(2); parameterControl(Param::SourceLevel, true); inputMeter_ = new studio::Meter(false); dry->fixed(inputMeter_, 24); dry->end(); original->end();
    separator(mix);
    auto* harmony = column(4); harmony->fixed(text("Harmony", 17, true), 24); parameterControl(Param::HarmonyLevel, true); harmony->end();
    separator(mix);
    auto* morph = column(4); auto* morphTitle = row(4); text("Morphizer", 17, true);
    morphEnabled_ = new studio::Toggle("Enable morphizer"); morphTitle->fixed(morphEnabled_, 34); morphTitle->end(); morph->fixed(morphTitle, 24);
    morphEnabled_->callback([](Fl_Widget* w, void* p) {
        auto& self = *static_cast<MainWindow*>(p);
        if (!static_cast<studio::Toggle*>(w)->value()) { self.lastMorphLevel_ = std::max(.01, self.parameters_[Param::MorphLevel]); self.parameters_.set(Param::MorphLevel, 0); }
        else self.parameters_.set(Param::MorphLevel, self.lastMorphLevel_);
        self.syncControls(); self.applyParameters();
    }, this);
    auto* grains = row(2);
    auto* amount = column(0); amount->fixed(text("Amount", 11, false, studio::Muted), 18); parameterControl(Param::MorphLevel, true); amount->end();
    auto* ratio = column(0); ratio->fixed(text("Pitch ratio", 11, false, studio::Muted), 18); parameterControl(Param::MorphFrequency, true); ratio->end();
    grains->end(); morph->end();
    separator(mix);
    auto* reverb = column(4); auto* reverbTitle = row(4); text("Reverb", 17, true);
    reverbEnabled_ = new studio::Toggle("Enable reverb"); reverbTitle->fixed(reverbEnabled_, 34); reverbTitle->end(); reverb->fixed(reverbTitle, 24);
    reverbEnabled_->callback([](Fl_Widget* w, void* p) {
        auto& self = *static_cast<MainWindow*>(p);
        if (!static_cast<studio::Toggle*>(w)->value()) { self.lastReverbMix_ = std::max(.01, self.parameters_[Param::ReverbMix]); self.parameters_.set(Param::ReverbMix, 0); }
        else self.parameters_.set(Param::ReverbMix, self.lastReverbMix_);
        self.syncControls(); self.applyParameters();
    }, this);
    auto* effects = row(2);
    auto* time = column(0); time->fixed(text("Time", 11, false, studio::Muted), 18); parameterControl(Param::ReverbTime, true); time->end();
    auto* wet = column(0); wet->fixed(text("Mix", 11, false, studio::Muted), 18); parameterControl(Param::ReverbMix, true); wet->end();
    effects->end(); reverb->end();
    separator(mix);
    auto* output = column(4); output->fixed(text("Output", 17, true), 24);
    auto* master = row(2); parameterControl(Param::OutputGain, true); outputMeter_ = new studio::Meter(); master->fixed(outputMeter_, 44); master->end(); output->end();
    mix->end(); page->end();
}
void MainWindow::makeParameterPage(const char* title, const char* subtitle, const std::vector<std::string>& groups) {
    auto* page = column(12); page->color(studio::Background); page->margin(20);
    page->fixed(text(title, 25, true), 36); page->fixed(text(subtitle, 12, false, studio::Muted), 25);
    auto* scroll = new ParameterScroll(); auto* content = scroll->content();
    const auto& info = parameterInfo();
    for (const auto& group : groups) {
        auto* heading = text("", 16, true); heading->copy_label(group.c_str()); content->fixed(heading, 28);
        Fl_Flex* fields = nullptr; int count = 0;
        for (std::size_t i = 0; i < parameterCount; ++i) {
            if (info[i].group != group || info[i].label.empty()) continue;
            if (count % 2 == 0) fields = row(12);
            auto* cell = new studio::Panel(); cell->margin(12, 8, 12, 8); cell->gap(3);
            const auto id = static_cast<Param>(i);
            if (!info[i].choices && info[i].minimum == 0 && info[i].maximum == 1 && info[i].step == 1) parameterControl(id);
            else { cell->fixed(text(info[i].label.c_str(), 12, false, studio::Muted), 18); parameterControl(id); }
            cell->end(); ++count;
            if (count % 2 == 0) { fields->end(); content->fixed(fields, 76); }
        }
        if (count % 2) { spacer(); fields->end(); content->fixed(fields, 76); }
    }
    int height = 0;
    for (int i = 0; i < content->children(); ++i) height += content->fixed(content->child(i)) + 10;
    scroll->finish(height + 12);
    if (std::find(groups.begin(), groups.end(), "Source") != groups.end()) {
        auto* source = row(12); auto* open = new studio::Button("Open WAV", studio::Icon::Folder); source->fixed(open, 140);
        open->callback([](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->chooseSource(); }, this);
        sourceFile_ = text("No WAV selected", 12, false, studio::Muted); source->end(); page->fixed(source, 38);
    }
    page->end();
}
void MainWindow::makeVoices() {
    auto* page = column(12); page->color(studio::Background); page->margin(0);
    auto* tabs = row(10); tabs->margin(20, 14, 20, 0); page->fixed(tabs, 56);
    static const char* names[] = {"Harmony", "Voice 1", "Voice 2", "Voice 3", "Voice 4"};
    for (int i = 0; i < 5; ++i) {
        auto* button = new studio::Button(names[i]); voiceNavigation_[i] = button;
        button->user_data(reinterpret_cast<void*>(static_cast<std::intptr_t>(i)));
        button->callback([](Fl_Widget* widget, void*) {
            static_cast<MainWindow*>(widget->window())->showVoice(int(reinterpret_cast<std::intptr_t>(widget->user_data())));
        });
    }
    tabs->end(); voicePages_ = new studio::PageStack();
    makeParameterPage("Harmony", "Choose a preset or custom intervals, then shape each voice.", {"Harmony"});
    for (int i = 1; i <= 4; ++i) {
        const auto name = "Voice " + std::to_string(i);
        makeParameterPage(name.c_str(), "Level, interval, vibrato, panning, formant preservation, and filtering.", {name});
    }
    voicePages_->end(); page->end();
}
void MainWindow::makeDevices() {
    auto* page = column(12); page->margin(20); page->color(studio::Background);
    page->fixed(text("Audio / MIDI", 25, true), 36);
    page->fixed(text("Choose devices and a buffer size. Stop recording before changing devices.", 12, false, studio::Muted), 25);
    auto* panel = new studio::Panel(); panel->margin(18); panel->gap(14);
    auto deviceField = [](const char* label) {
        auto* field = column(7); field->fixed(text(label, 12, false, studio::Muted), 18);
        auto* choice = new studio::Choice(label); field->end(); return choice;
    };
    auto* first = row(20); input_ = deviceField("Audio input"); fillDevices(input_, inputs_);
    output_ = deviceField("Stereo output"); fillDevices(output_, outputs_); first->end(); panel->fixed(first, 62);
    auto* second = row(20); rate_ = deviceField("Sample rate"); rate_->add("44100 Hz|48000 Hz|96000 Hz"); rate_->value(1);
    buffer_ = deviceField("Buffer size"); buffer_->add("64 frames|128 frames|256 frames|512 frames|1024 frames"); buffer_->value(2);
    second->end(); panel->fixed(second, 62);
    auto* third = row(20); midiInput_ = deviceField("MIDI input"); fillDevices(midiInput_, midiInputs_);
    midiOutput_ = deviceField("MIDI output"); fillDevices(midiOutput_, midiOutputs_); third->end(); panel->fixed(third, 62);
    auto* fourth = row(20);
    auto* mode = column(7); mode->fixed(text("MIDI control", 12, false, studio::Muted), 18); parameterControl(Param::MidiMode); mode->end();
    auto* channel = column(7); channel->fixed(text("MIDI channel", 12, false, studio::Muted), 18); parameterControl(Param::MidiChannel); channel->end();
    fourth->end(); panel->fixed(fourth, 62);
    thru_ = new studio::Toggle("MIDI thru to selected output", true); panel->fixed(thru_, 34);
    auto* actions = row(); auto* apply = new studio::Button("Apply audio / MIDI settings", studio::Icon::Audio);
    apply->primary(true); actions->fixed(apply, 248); apply->callback([](Fl_Widget*, void* p) { static_cast<MainWindow*>(p)->applyDevices(); }, this);
    spacer(); actions->end(); panel->fixed(actions, 40); spacer(); panel->end();
    page->fixed(text("Smaller buffers reduce latency. Use headphones with live microphone input.", 12, false, studio::Muted), 26); page->end();
}
void MainWindow::showPage(int page) {
    page_ = std::clamp(page, 0, 5); pages_->showPage(page_);
    for (int i = 0; i < int(navigation_.size()); ++i) { navigation_[i]->value(i == page_); navigation_[i]->redraw(); }
}
void MainWindow::showVoice(int page) {
    voicePage_ = std::clamp(page, 0, 4); voicePages_->showPage(voicePage_);
    for (int i = 0; i < 5; ++i) { voiceNavigation_[i]->value(i == voicePage_); voiceNavigation_[i]->redraw(); }
}
void MainWindow::syncControls() {
    for (std::size_t i = 0; i < parameterCount; ++i) for (auto* widget : controls_[i]) {
        if (auto* choice = dynamic_cast<Fl_Choice*>(widget)) choice->value(int(parameters_.values[i] - parameterInfo()[i].minimum));
        else if (auto* toggle = dynamic_cast<Fl_Button*>(widget)) toggle->value(parameters_.values[i] != 0);
        else if (auto* value = dynamic_cast<Fl_Valuator*>(widget)) value->value(parameters_.values[i]);
        if (i >= static_cast<std::size_t>(Param::VoiceStart) && (i - static_cast<std::size_t>(Param::VoiceStart)) % 9 == static_cast<std::size_t>(VoiceParam::Semitones)) {
            if (parameters_[Param::Logic] == 0 && parameters_[Param::MidiMode] != 2) widget->activate(); else widget->deactivate();
        }
        widget->redraw();
    }
    morphEnabled_->value(parameters_[Param::MorphLevel] > 0); reverbEnabled_->value(parameters_[Param::ReverbMix] > 0);
    morphEnabled_->redraw(); reverbEnabled_->redraw();
    if (sourceFile_) sourceFile_->copy_label(filePath_.empty() ? "No WAV selected" : std::filesystem::u8path(filePath_).filename().u8string().c_str());
}
void MainWindow::parameterCallback(Fl_Widget* widget, void* data) {
    const auto& binding = *static_cast<Binding*>(data); binding.owner->parameterChanged(widget, binding.id);
}
void MainWindow::parameterChanged(Fl_Widget* widget, Param id) {
    double value = 0;
    if (auto* choice = dynamic_cast<Fl_Choice*>(widget)) value = choice->value() + parameterInfo()[static_cast<std::size_t>(id)].minimum;
    else if (auto* toggle = dynamic_cast<Fl_Button*>(widget)) value = toggle->value();
    else value = static_cast<Fl_Valuator*>(widget)->value();
    if (engine_->recording() && parameterInfo()[static_cast<std::size_t>(id)].restart) {
        fl_message("Stop recording before changing this setting."); syncControls(); return;
    }
    if (id == Param::Source && value == 3 && file_.mono.empty()) { chooseSource(); syncControls(); return; }
    parameters_.set(id, value); parameters_.sanitize(); syncControls(); applyParameters();
}
void MainWindow::applyParameters() {
    if (!engine_->running()) { sent_ = parameters_; return; }
    if (parameters_.needsRestart(sent_)) { startAudio(); return; }
    pending_ = !engine_->update(parameters_); if (!pending_) sent_ = parameters_;
}

void MainWindow::startAudio() {
    try {
        if(parameters_[Param::Source]==3 && file_.mono.empty() && !filePath_.empty()) file_=readWav(std::filesystem::u8path(filePath_));
        engine_->start(settings_,parameters_,file_.mono.empty()?nullptr:&file_);
        sent_=parameters_;pending_=false;start_->label("Stop audio");record_->activate();
    }catch(const std::exception& error){start_->label("Start audio");status_->copy_label(error.what());showError(error);}
}
void MainWindow::toggleAudio() {
    if(engine_->running()) {engine_->stop();start_->label("Start audio");record_->label("Record WAV");}
    else startAudio();
}
void MainWindow::toggleRecording() {
    try {
        if(engine_->recording()) {engine_->stopRecording();record_->label("Record WAV");}
        else {
            if(!engine_->running()) {fl_message("Start audio before recording.");return;}
            auto file=chooseFile("Record stereo WAV",Fl_Native_File_Chooser::BROWSE_SAVE_FILE,"WAV audio\t*.wav");
            if(file.empty()) return;
            if(std::filesystem::u8path(file).extension().empty()) file+=".wav";
            engine_->startRecording(std::filesystem::u8path(file));recordErrorShown_=false;record_->label("Stop recording");
        }
    }catch(const std::exception& error){showError(error);}
}
void MainWindow::chooseSource() {
    if(engine_->recording()){fl_message("Stop recording before loading a source.");return;}
    const auto path=chooseFile("Open WAV source",Fl_Native_File_Chooser::BROWSE_FILE,"WAV audio\t*.wav");if(path.empty()) return;
    try {
        auto file=readWav(std::filesystem::u8path(path));
        file_=std::move(file);filePath_=path;parameters_.set(Param::Source,3);syncControls();
        if(engine_->running()) startAudio();else sent_=parameters_;
    }catch(const std::exception& error){showError(error);}
}
void MainWindow::loadPreset() {
    if(engine_->recording()){fl_message("Stop recording before loading a preset.");return;}
    const auto path=chooseFile("Load reVoice preset",Fl_Native_File_Chooser::BROWSE_FILE,"reVoice preset\t*.rvpreset");if(path.empty()) return;
    try {
        std::ifstream in(std::filesystem::u8path(path));if(!in) throw std::runtime_error("Cannot read preset");
        in.imbue(std::locale::classic());std::string line;
        if(!std::getline(in,line) || line!="reVoice preset 1") throw std::runtime_error("Unsupported preset format");
        Parameters loaded;const auto& info=parameterInfo();
        while(std::getline(in,line)) {
            const auto equal=line.find('=');if(equal==std::string::npos) continue;
            const auto key=line.substr(0,equal);std::istringstream number(line.substr(equal+1));number.imbue(std::locale::classic());double value;
            if(!(number>>value)) throw std::runtime_error("Invalid preset value");
            for(std::size_t i=0;i<parameterCount;++i) if(info[i].key==key) {loaded.set(static_cast<Param>(i),value);break;}
        }
        loaded.sanitize();parameters_=loaded;syncControls();applyParameters();
    }catch(const std::exception& error){showError(error);}
}
void MainWindow::savePreset() {
    auto path=chooseFile("Save reVoice preset",Fl_Native_File_Chooser::BROWSE_SAVE_FILE,"reVoice preset\t*.rvpreset");if(path.empty()) return;
    if(std::filesystem::u8path(path).extension().empty()) path+=".rvpreset";
    try {
        std::ofstream out(std::filesystem::u8path(path));out.imbue(std::locale::classic());
        out<<"reVoice preset 1\n"<<std::setprecision(17);
        const auto& info=parameterInfo();for(std::size_t i=0;i<parameterCount;++i) out<<info[i].key<<'='<<parameters_.values[i]<<'\n';
        if(!out) throw std::runtime_error("Cannot write preset");
    }catch(const std::exception& error){showError(error);}
}
void MainWindow::applyDevices() {
    if(engine_->recording()){fl_message("Stop recording before changing devices.");return;}
    static const double rates[]={44100,48000,96000};static const unsigned buffers[]={64,128,256,512,1024};
    settings_.input=deviceId(input_,inputs_);settings_.output=deviceId(output_,outputs_);
    settings_.midiInput=deviceId(midiInput_,midiInputs_);settings_.midiOutput=deviceId(midiOutput_,midiOutputs_);
    settings_.sampleRate=rates[std::clamp(rate_->value(),0,2)];settings_.frames=buffers[std::clamp(buffer_->value(),0,4)];settings_.midiThru=thru_->value()!=0;
    if(engine_->running()) startAudio();else status_->copy_label("Audio / MIDI settings saved. Press Start audio when ready.");
}
void MainWindow::loadPreferences() {
    Fl_Preferences prefs(Fl_Preferences::USER,"CarmineCella","reVoice");
    const auto& info=parameterInfo();for(std::size_t i=0;i<parameterCount;++i) {double value; prefs.get(info[i].key.c_str(),value,info[i].initial);parameters_.set(static_cast<Param>(i),value);}
    parameters_.sanitize();char path[4096]{};prefs.get("source_file",path,"",sizeof(path));filePath_=path;
    auto device=[&](const char* key,Fl_Choice* widget,const std::vector<Device>& devices) {
        char name[1024]{};prefs.get(key,name,"",sizeof(name));
        for(std::size_t i=0;i<devices.size();++i) if(devices[i].name==name) {widget->value(int(i));break;}
    };
    device("audio_input",input_,inputs_);device("audio_output",output_,outputs_);device("midi_input",midiInput_,midiInputs_);device("midi_output",midiOutput_,midiOutputs_);
    int rate,buffer,thru;prefs.get("sample_rate_index",rate,1);prefs.get("buffer_index",buffer,2);prefs.get("midi_thru",thru,0);
    rate_->value(std::clamp(rate,0,2));buffer_->value(std::clamp(buffer,0,4));thru_->value(thru!=0);applyDevices();
}
void MainWindow::savePreferences() {
    Fl_Preferences prefs(Fl_Preferences::USER,"CarmineCella","reVoice");
    const auto& info=parameterInfo();for(std::size_t i=0;i<parameterCount;++i) prefs.set(info[i].key.c_str(),parameters_.values[i]);
    prefs.set("source_file",filePath_.c_str());
    prefs.set("audio_input",deviceName(input_,inputs_).c_str());prefs.set("audio_output",deviceName(output_,outputs_).c_str());
    prefs.set("midi_input",deviceName(midiInput_,midiInputs_).c_str());prefs.set("midi_output",deviceName(midiOutput_,midiOutputs_).c_str());
    prefs.set("sample_rate_index",rate_->value());prefs.set("buffer_index",buffer_->value());prefs.set("midi_thru",thru_->value());
    prefs.flush();
}
void MainWindow::closeWindow(){engine_->stop();if(persistence_) savePreferences();hide();}

void MainWindow::transportState() {
    const bool running = engine_->running(), recording = engine_->recording();
    start_->label(running ? "Stop audio" : "Start audio");
    start_->icon(running ? studio::Icon::Stop : studio::Icon::Play); start_->primary(!running);
    record_->label(recording ? "Stop rec" : "Record"); record_->value(recording); record_->redraw();
    audioState_->copy_label(running ? "Audio on" : "Stopped");
    audioState_->labelcolor(running ? studio::Mint : studio::Muted); audioState_->redraw();
}
void MainWindow::timer(void* data) { static_cast<MainWindow*>(data)->tick(); Fl::repeat_timeout(.05, timer, data); }
void MainWindow::tick() {
    if (pending_ && engine_->running()) { pending_ = !engine_->update(parameters_); if (!pending_) sent_ = parameters_; }
    const auto stats = engine_->stats();
    pitch_->sample(stats.pitch, engine_->running()); inputMeter_->levels(stats.input); outputMeter_->levels(stats.left, stats.right);
    char status[360];
    std::snprintf(status, sizeof(status), "%.1f kHz    |    Buffer %u    |    Latency %.1f ms    |    CPU %.1f%%    |    Dropouts %llu%s%s",
        settings_.sampleRate / 1000, settings_.frames, engine_->running() ? stats.latencyMs : 0, 100 * stats.cpu,
        static_cast<unsigned long long>(stats.xruns), engine_->recording() ? "    |    RECORDING" : "",
        stats.droppedRecordingFrames ? "    |    Recording dropped frames" : "");
    status_->copy_label(status); transportState();
    if (stats.callbackFailed) { engine_->stop(); transportState(); fl_alert("Audio processing failed. The stream has been stopped."); }
    if (stats.recordingFailed && !recordErrorShown_) {
        engine_->stopRecording(); transportState(); recordErrorShown_ = true;
        fl_alert("Recording could not be written. Check the output file and available disk space.");
    }
}
int MainWindow::handle(int event) {
    if (event == FL_SHORTCUT) {
        const int key = Fl::event_key();
        if (key == FL_F + 5) { toggleAudio(); return 1; }
        if (key == FL_F + 10 || key == FL_Escape) { engine_->panic(); return 1; }
        if (Fl::event_state(FL_COMMAND)) {
            if (key == 'o') { chooseSource(); return 1; } if (key == 'l') { loadPreset(); return 1; }
            if (key == 's') { savePreset(); return 1; } if (key == 'q') { closeWindow(); return 1; }
        }
    }
    return Fl_Double_Window::handle(event);
}
void MainWindow::validateUI() {
    if (engine_->running()) throw std::runtime_error("UI smoke test must not open audio");
    const Parameters original = parameters_;
    for (std::size_t i = 0; i < parameterCount; ++i)
        if (controls_[i].empty()) throw std::runtime_error("Missing UI control for " + parameterInfo()[i].key);
    for (int page = 0; page < 6; ++page) {
        navigation_[page]->do_callback();
        if (page_ != page) throw std::runtime_error("Sidebar callback selected the wrong page");
        for (int i = 0; i < pages_->children(); ++i)
            if (bool(pages_->child(i)->visible()) != (i == page)) throw std::runtime_error("Navigation did not switch pages");
    }
    auto* gain = dynamic_cast<Fl_Valuator*>(controls_[static_cast<std::size_t>(Param::OutputGain)].front());
    gain->value(.5); gain->do_callback();
    if (parameters_[Param::OutputGain] != .5) throw std::runtime_error("Output control did not update its parameter");
    for (auto* widget : controls_[static_cast<std::size_t>(Param::OutputGain)])
        if (auto* mirror = dynamic_cast<Fl_Valuator*>(widget)) if (mirror->value() != .5) throw std::runtime_error("Mirrored mix controls disagree");
    auto* logic = static_cast<Fl_Choice*>(controls_[static_cast<std::size_t>(Param::Logic)].front());
    logic->value(0); logic->do_callback();
    auto* interval = static_cast<Fl_Choice*>(controls_[static_cast<std::size_t>(voiceParam(0, VoiceParam::Semitones))].front());
    if (!interval->active()) throw std::runtime_error("Custom intervals are not editable");
    interval->value(12); interval->do_callback();
    if (parameters_[voiceParam(0, VoiceParam::Semitones)] != -12) throw std::runtime_error("Interval selection is incorrectly mapped");
    for (int voice = 0; voice < 5; ++voice) {
        voiceNavigation_[voice]->do_callback();
        if (voicePage_ != voice) throw std::runtime_error("Voice callback selected the wrong page");
    }
    parameters_ = original; syncControls(); showPage(0); showVoice(0);
}
