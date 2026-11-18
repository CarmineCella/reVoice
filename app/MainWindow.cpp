#include "MainWindow.h"
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/fl_ask.H>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>

using namespace revoice;
namespace {
std::string chooseFile(const char* title,int type,const char* filter) {
    Fl_Native_File_Chooser dialog;dialog.title(title);
    dialog.type(type);dialog.filter(filter);
    if(type==Fl_Native_File_Chooser::BROWSE_SAVE_FILE) dialog.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM|Fl_Native_File_Chooser::USE_FILTER_EXT);
    if(dialog.show()!=0 || !dialog.filename()) return {};
    return dialog.filename();
}
std::string deviceName(Fl_Choice* choice,const std::vector<Device>& devices) {
    const int index=choice->value();return index>=0 && index<int(devices.size())?devices[index].name:"";
}
int deviceId(Fl_Choice* choice,const std::vector<Device>& devices) {
    const int index=choice->value();return index>=0 && index<int(devices.size())?devices[index].id:-1;
}
// Fl_Menu_::add treats slash and ampersand as menu syntax. Device names must
// remain literal, so add each label with the data-preserving insert overload.
std::string menuEscape(const std::string& name) {
    std::string escaped;
    for(char c:name) {if(c=='/' || c=='\\' || c=='&' || c=='_') escaped+='\\';escaped+=c;}
    return escaped;
}
void fillDevices(Fl_Choice* choice,const std::vector<Device>& devices) {
    choice->clear();for(const auto& d:devices) choice->add(menuEscape(d.name).c_str());choice->value(0);
}
Fl_Choice* choiceField(int x,int y,int width,const char* label) {
    auto* caption=new Fl_Box(x,y,width,20,label);caption->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    return new Fl_Choice(x,y+24,width,28);
}
void showError(const std::exception& error){fl_alert("%s",error.what());}
}
MainWindow::MainWindow(bool persistence):Fl_Double_Window(1024,760,"reVoice " REVOICE_VERSION),persistence_(persistence) {
    engine_=std::make_unique<AudioEngine>();
    inputs_=engine_->inputs();outputs_=engine_->outputs();midiInputs_=engine_->midiInputs();midiOutputs_=engine_->midiOutputs();
    begin();
    auto* menu=new Fl_Menu_Bar(0,0,w(),28);
    menu->add("&File/&Open WAV...",FL_COMMAND+'o',[](Fl_Widget*,void* p){static_cast<MainWindow*>(p)->chooseSource();},this);
    menu->add("&File/&Load preset...",FL_COMMAND+'l',[](Fl_Widget*,void* p){static_cast<MainWindow*>(p)->loadPreset();},this);
    menu->add("&File/&Save preset...",FL_COMMAND+'s',[](Fl_Widget*,void* p){static_cast<MainWindow*>(p)->savePreset();},this,FL_MENU_DIVIDER);
    menu->add("&File/&Quit",FL_COMMAND+'q',[](Fl_Widget*,void* p){static_cast<MainWindow*>(p)->closeWindow();},this);
    menu->add("&Control/&Start or stop",FL_F+5,[](Fl_Widget*,void* p){static_cast<MainWindow*>(p)->toggleAudio();},this);
    menu->add("&Control/&All notes off",FL_Escape,[](Fl_Widget*,void* p){static_cast<MainWindow*>(p)->engine_->panic();},this);
    menu->add("&Help/&About",0,[](Fl_Widget*,void*){
        fl_message("reVoice " REVOICE_VERSION "\nReal-time vocal harmonizer\nCarmine Emanuele Cella\n\nBSD 2-Clause application; Pandora LGPL 2.1.\nBased in part on the work of the FLTK project.\nAudio: PortAudio. MIDI: PortMidi. Reverb: STK NRev.");
    },this);
    auto* title=new Fl_Box(24,38,200,38,"reVoice");title->labelsize(28);title->labelfont(FL_HELVETICA_BOLD);title->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    auto* subtitle=new Fl_Box(24,78,260,22,"Real-time vocal harmonizer");subtitle->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);subtitle->labelcolor(fl_rgb_color(90,95,100));
    start_=new Fl_Button(324,48,116,38,"Start audio");start_->callback([](Fl_Widget*,void* p){static_cast<MainWindow*>(p)->toggleAudio();},this);
    record_=new Fl_Button(452,48,116,38,"Record WAV");record_->callback([](Fl_Widget*,void* p){static_cast<MainWindow*>(p)->toggleRecording();},this);
    auto* panic=new Fl_Button(580,48,100,38,"All notes off");panic->callback([](Fl_Widget*,void* p){static_cast<MainWindow*>(p)->engine_->panic();},this);
    auto meter=[](int y,const char* label) {
        auto* m=new Fl_Progress(770,y,220,14,label);m->minimum(0);m->maximum(1);m->value(0);
        m->align(FL_ALIGN_LEFT);m->selection_color(fl_rgb_color(40,155,100));return m;
    };
    inputMeter_=meter(43,"In");leftMeter_=meter(64,"L");rightMeter_=meter(85,"R");
    auto* tabs=new Fl_Tabs(20,122,984,572);
    makeParameterTab("Source & Mix",{"Source","Mix"});
    makeParameterTab("Harmony",{"Harmony"});
    {
        auto* voices=new Fl_Group(24,154,976,536,"Voices");
        auto* voiceTabs=new Fl_Tabs(32,168,956,508);
        for(int i=1;i<=4;++i) {
            const auto label="Voice "+std::to_string(i);
            auto* group=new Fl_Group(36,200,948,468);group->copy_label(label.c_str());
            auto* scroll=new Fl_Scroll(40,207,938,455);
            makeParameterControls(scroll,{label});scroll->end();group->resizable(scroll);group->end();
        }
        voiceTabs->end();voices->resizable(voiceTabs);voices->end();
    }
    makeParameterTab("Morphizer",{"Morphizer"});
    makeParameterTab("Pitch Detector",{"Detector"});
    makeDevicesTab();
    tabs->end();
    status_=new Fl_Box(24,710,976,26,"Stopped. Choose a source and press Start audio.");status_->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    end();resizable(tabs);size_range(1024,760);
    callback([](Fl_Widget*,void* p){static_cast<MainWindow*>(p)->closeWindow();},this);
    if(persistence_) loadPreferences();
    sent_=parameters_;syncControls();
    Fl::add_timeout(.05,timer,this);
}
MainWindow::~MainWindow(){Fl::remove_timeout(timer,this);engine_->stop();}
void MainWindow::makeParameterControls(Fl_Scroll* scroll,const std::vector<std::string>& groups) {
    scroll->type(Fl_Scroll::VERTICAL);scroll->begin();
    const auto& info=parameterInfo();int index=0;
    for(std::size_t i=0;i<parameterCount;++i) {
        const auto& d=info[i];
        if(d.label.empty() || std::find(groups.begin(),groups.end(),d.group)==groups.end()) continue;
        const int x=scroll->x()+20+(index%2)*450,y=scroll->y()+16+(index/2)*68;
        const auto id=static_cast<Param>(i);bindings_[i]={this,id};
        Fl_Widget* control=nullptr;
        if(!d.choices && d.minimum==0 && d.maximum==1 && d.step==1) {
            auto* check=new Fl_Check_Button(x,y+22,420,30);check->copy_label(d.label.c_str());control=check;
        } else {
            auto* label=new Fl_Box(x,y,420,20);label->copy_label(d.label.c_str());label->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
            if(d.choices) {auto* choice=new Fl_Choice(x,y+24,420,28);choice->add(d.choices);control=choice;}
            else {
                auto* slider=new Fl_Value_Slider(x,y+24,420,28);slider->type(FL_HOR_NICE_SLIDER);
                slider->bounds(d.minimum,d.maximum);slider->step(d.step);slider->textsize(13);control=slider;
            }
        }
        control->callback(parameterCallback,&bindings_[i]);
        if(d.restart) control->tooltip("Changing this setting restarts audio. Stop recording first.");
        controls_[i]=control;++index;
    }
    if(std::find(groups.begin(),groups.end(),"Source")!=groups.end()) {
        const int y=scroll->y()+24+((index+1)/2)*68;
        auto* file=new Fl_Button(scroll->x()+20,y,160,32,"Choose WAV...");
        file->callback([](Fl_Widget*,void* p){static_cast<MainWindow*>(p)->chooseSource();},this);
        sourceFile_=new Fl_Box(scroll->x()+195,y,660,32,"No WAV selected");sourceFile_->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    }
}
void MainWindow::makeParameterTab(const char* title,const std::vector<std::string>& groups) {
    auto* group=new Fl_Group(24,154,976,536,title);
    auto* scroll=new Fl_Scroll(32,166,956,514);
    makeParameterControls(scroll,groups);scroll->end();group->resizable(scroll);group->end();
}
void MainWindow::makeDevicesTab() {
    auto* group=new Fl_Group(24,154,976,536,"Audio / MIDI");
    input_=choiceField(52,180,430,"Audio input");fillDevices(input_,inputs_);
    output_=choiceField(532,180,430,"Stereo output");fillDevices(output_,outputs_);
    rate_=choiceField(52,252,430,"Sample rate (Hz)");rate_->add("44100|48000|96000");rate_->value(1);
    buffer_=choiceField(532,252,430,"Buffer size (frames)");buffer_->add("64|128|256|512|1024");buffer_->value(2);
    midiInput_=choiceField(52,324,430,"MIDI input");fillDevices(midiInput_,midiInputs_);
    midiOutput_=choiceField(532,324,430,"MIDI output");fillDevices(midiOutput_,midiOutputs_);
    thru_=new Fl_Check_Button(52,398,430,28,"MIDI thru to selected output");
    for(Param id:{Param::MidiMode,Param::MidiChannel}) {
        const auto index=static_cast<std::size_t>(id);const auto& info=parameterInfo()[index];
        auto* choice=choiceField(id==Param::MidiMode?52:532,450,430,info.label.c_str());
        if(info.choices) choice->add(info.choices);else for(int n=1;n<=16;++n) choice->add(std::to_string(n).c_str());
        bindings_[index]={this,id};controls_[index]=choice;choice->callback(parameterCallback,&bindings_[index]);
    }
    auto* apply=new Fl_Button(52,544,190,36,"Apply audio / MIDI settings");
    apply->callback([](Fl_Widget*,void* p){static_cast<MainWindow*>(p)->applyDevices();},this);
    auto* hint=new Fl_Box(52,602,890,44,"Use headphones with live input. Smaller buffers reduce latency.\nDevice changes restart audio; recording must be stopped first.");
    hint->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    group->end();
}
void MainWindow::syncControls() {
    for(std::size_t i=0;i<parameterCount;++i) {
        auto* widget=controls_[i];if(!widget) continue;
        const auto id=static_cast<Param>(i);const auto& info=parameterInfo()[i];
        if(auto* choice=dynamic_cast<Fl_Choice*>(widget)) choice->value(int(parameters_[id]-info.minimum));
        else if(auto* check=dynamic_cast<Fl_Check_Button*>(widget)) check->value(parameters_[id]!=0);
        else if(auto* slider=dynamic_cast<Fl_Value_Slider*>(widget)) slider->value(parameters_[id]);
    }
    if(sourceFile_) sourceFile_->copy_label(filePath_.empty()?"No WAV selected":std::filesystem::u8path(filePath_).filename().u8string().c_str());
    redraw();
}
void MainWindow::parameterCallback(Fl_Widget*,void* data) {const auto& b=*static_cast<Binding*>(data);b.owner->parameterChanged(b.id);}
void MainWindow::parameterChanged(Param id) {
    const auto index=static_cast<std::size_t>(id);auto* control=controls_[index];double value=0;
    if(auto* choice=dynamic_cast<Fl_Choice*>(control)) value=choice->value()+parameterInfo()[index].minimum;
    else if(auto* check=dynamic_cast<Fl_Check_Button*>(control)) value=check->value();
    else value=static_cast<Fl_Value_Slider*>(control)->value();
    if(engine_->recording() && parameterInfo()[index].restart) {
        fl_message("Stop recording before changing this setting.");syncControls();return;
    }
    parameters_.set(id,value);parameters_.sanitize();applyParameters();
}
void MainWindow::applyParameters() {
    if(!engine_->running()) {sent_=parameters_;return;}
    if(parameters_.needsRestart(sent_)) {startAudio();return;}
    pending_=!engine_->update(parameters_);if(!pending_) sent_=parameters_;
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
void MainWindow::timer(void* data){static_cast<MainWindow*>(data)->tick();Fl::repeat_timeout(.05,timer,data);}
void MainWindow::tick() {
    if(pending_ && engine_->running()) {pending_=!engine_->update(parameters_);if(!pending_) sent_=parameters_;}
    const auto stats=engine_->stats();
    auto meter=[](Fl_Progress* widget,float peak){widget->value(std::max(double(peak),widget->value()*.85));widget->selection_color(peak>=.98?FL_RED:fl_rgb_color(40,155,100));};
    meter(inputMeter_,stats.input);meter(leftMeter_,stats.left);meter(rightMeter_,stats.right);
    if(engine_->running()) {
        char text[300];const double pitch=stats.pitch>0?stats.pitch:0;
        std::snprintf(text,sizeof(text),"%s  |  Pitch %.1f Hz  |  CPU %.1f%%  |  Latency %.1f ms  |  Dropouts %llu  |  Recording drops %llu",
            engine_->recording()?"Recording":"Running",pitch,100*stats.cpu,stats.latencyMs,
            static_cast<unsigned long long>(stats.xruns),static_cast<unsigned long long>(stats.droppedRecordingFrames));
        status_->copy_label(text);
    } else if(!stats.callbackFailed) status_->copy_label("Stopped. Choose a source and press Start audio.");
    if(stats.callbackFailed) {engine_->stop();start_->label("Start audio");record_->label("Record WAV");fl_alert("Audio processing failed. The stream has been stopped.");}
    if(stats.recordingFailed && !recordErrorShown_) {engine_->stopRecording();record_->label("Record WAV");recordErrorShown_=true;fl_alert("Recording could not be written. Check the output file and available disk space.");}
}
