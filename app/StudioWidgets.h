#pragma once

#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Valuator.H>
#include <array>
#include <string>

namespace studio {
constexpr Fl_Color Background = 0x191D2000;
constexpr Fl_Color Sidebar = 0x161A1D00;
constexpr Fl_Color Surface = 0x23292C00;
constexpr Fl_Color Control = 0x1D232600;
constexpr Fl_Color Border = 0x48525500;
constexpr Fl_Color Track = 0x56606400;
constexpr Fl_Color Text = 0xF0F4F200;
constexpr Fl_Color Muted = 0xA6B2AF00;
constexpr Fl_Color Mint = 0xA1EDBB00;
constexpr Fl_Color Ink = 0x14251B00;
constexpr Fl_Color Red = 0xEC6B7000;
constexpr Fl_Boxtype InputBox = static_cast<Fl_Boxtype>(FL_FREE_BOXTYPE);

void applyTheme();
void rounded(int x, int y, int w, int h, Fl_Color fill, Fl_Color border = Border, int radius = 7);
enum class Icon { None, Studio, Voices, Morphizer, Pitch, Audio, Play, Stop, Record, Folder, Save, More, Power, Freeze, Edit };
void drawIcon(Icon icon, int x, int y, int size, Fl_Color color);

class Panel : public Fl_Flex {
protected:
    void draw() override;
public:
    explicit Panel(int direction = Fl_Flex::COLUMN);
};

class Button : public Fl_Button {
    Icon icon_;
    bool primary_ = false, navigation_ = false, hover_ = false;
protected:
    void draw() override;
public:
    Button(const char* label, Icon icon = Icon::None);
    void primary(bool value) { primary_ = value; redraw(); }
    void navigation(bool value) { navigation_ = value; redraw(); }
    void icon(Icon value) { icon_ = value; redraw(); }
    int handle(int event) override;
};

class Toggle : public Fl_Check_Button {
    bool showText_;
protected:
    void draw() override;
public:
    explicit Toggle(const char* label = nullptr, bool showText = false);
};

class Choice : public Fl_Choice {
protected:
    void draw() override;
public:
    explicit Choice(const char* label = nullptr);
};

class Menu : public Fl_Menu_Button {
protected:
    void draw() override;
public:
    Menu();
};

enum class Units { Number, Decibels, Percent, Pan, Ratio, Hertz, Semitones, Seconds };
class Value : public Fl_Valuator {
public:
    enum class Style { Slider, Dial };
private:
    Style style_;
    Units units_;
    double dragStart_ = 0;
    int dragY_ = 0;
    bool typing_ = false;
    bool dragRelative_ = false;
    double position() const;
    double fromPosition(double position) const;
    void editValue();
protected:
    void draw() override;
public:
    Value(const char* label, Style style = Style::Slider, Units units = Units::Number);
    int handle(int event) override;
    std::string formatted(double value) const;
};

class Meter : public Fl_Widget {
    float left_ = 0, right_ = 0;
    bool stereo_;
protected:
    void draw() override;
public:
    explicit Meter(bool stereo = true);
    void levels(float left, float right = 0);
};

class PitchDisplay : public Fl_Widget {
    std::array<double, 200> history_{};
    std::size_t cursor_ = 0;
    double pitch_ = 0, center_ = 69;
    bool running_ = false;
protected:
    void draw() override;
public:
    PitchDisplay();
    void sample(double pitch, bool running);
};

// Every page gets the same rectangle; hidden pages don't consume flex space.
class PageStack : public Fl_Group {
public:
    PageStack();
    void resize(int x, int y, int w, int h) override;
    void showPage(int page);
};
} // namespace studio
