#include "StudioWidgets.h"
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace studio {
namespace {
constexpr double Pi = 3.14159265358979323846;
Fl_Color disabled(Fl_Color color, bool active) { return active ? color : fl_color_average(color, Surface, .4f); }
void labelText(const char* text, int x, int y, int w, int h, int size = 14,
               Fl_Color color = Text, Fl_Align align = FL_ALIGN_LEFT | FL_ALIGN_INSIDE, bool bold = false) {
    fl_font(bold ? FL_HELVETICA_BOLD : FL_HELVETICA, size);
    fl_color(color);
    fl_draw(text ? text : "", x, y, w, h, align);
}
}

void applyTheme() {
    Fl::scheme("base");
    Fl::background(25, 29, 32);
    Fl::background2(29, 35, 38);
    Fl::foreground(240, 244, 242);
    Fl::set_color(FL_SELECTION_COLOR, 161, 237, 187);
    Fl::set_color(FL_INACTIVE_COLOR, 120, 131, 129);
    Fl::scrollbar_size(12);
    Fl::visible_focus(1);
    Fl::set_boxtype(InputBox, [](int x, int y, int w, int h, Fl_Color) {
        rounded(x, y, w, h, Control, Border, 5);
    }, 6, 2, 12, 4);
    fl_message_font(FL_HELVETICA, 14);
}

void rounded(int x, int y, int w, int h, Fl_Color fill, Fl_Color border, int radius) {
    if (w < 2 || h < 2) return;
    fl_color(fill);
    fl_rounded_rectf(x, y, w, h, radius);
    fl_color(border);
    fl_rounded_rect(x, y, w - 1, h - 1, radius);
}

void drawIcon(Icon icon, int x, int y, int size, Fl_Color color) {
    const int cx = x + size / 2, cy = y + size / 2;
    fl_color(color);
    fl_line_style(FL_SOLID | FL_CAP_ROUND | FL_JOIN_ROUND, 2);
    switch (icon) {
    case Icon::Studio:
        for (int i = 0; i < 5; ++i) {
            const int height = i == 2 ? size : (i % 2 ? size * 2 / 3 : size / 3);
            fl_line(x + 2 + i * (size - 4) / 4, cy - height / 2, x + 2 + i * (size - 4) / 4, cy + height / 2);
        }
        break;
    case Icon::Voices:
        fl_arc(cx - 4, y + 1, 8, 8, 0, 360);
        fl_arc(x, cy, size, size, 15, 165);
        fl_arc(x - 2, y + 2, 6, 6, 90, 270);
        fl_arc(x + size - 4, y + 2, 6, 6, -90, 90);
        break;
    case Icon::Morphizer:
        for (int row = 0; row < 3; ++row) {
            fl_begin_line();
            for (int i = 0; i <= size; ++i) fl_vertex(x + i, y + 4 + row * 6 + 2 * std::sin(2 * Pi * i / size));
            fl_end_line();
        }
        break;
    case Icon::Pitch:
        fl_line(x + 3, y + size - 3, x + 3, y + 2);
        fl_line(x + 3, y + size - 3, x + size, y + size - 3);
        fl_line(x + 5, cy + 2, cx, cy - 4, x + size - 1, cy - 1);
        break;
    case Icon::Audio:
        fl_arc(x, y + 3, size, size - 6, 0, 360);
        fl_circle(cx - 4, cy, 1); fl_circle(cx + 4, cy, 1);
        break;
    case Icon::Play:
        fl_polygon(x + 4, y + 2, x + size - 2, cy, x + 4, y + size - 2);
        break;
    case Icon::Stop: fl_rectf(x + 4, y + 4, size - 8, size - 8); break;
    case Icon::Record: fl_pie(x + 3, y + 3, size - 6, size - 6, 0, 360); break;
    case Icon::Folder:
        fl_line(x + 1, y + 5, x + 8, y + 5, x + 10, y + 8);
        fl_line(x + 10, y + 8, x + size - 1, y + 8, x + size - 1, y + size - 2);
        fl_line(x + size - 1, y + size - 2, x + 1, y + size - 2, x + 1, y + 5);
        break;
    case Icon::Save:
        fl_rect(x + 2, y + 2, size - 4, size - 4);
        fl_rect(x + 6, y + 2, size - 12, 6);
        fl_rect(x + 6, cy + 1, size - 12, size / 2 - 3);
        break;
    case Icon::More:
        for (int i = -1; i <= 1; ++i) fl_pie(cx + i * 6 - 1, cy - 1, 3, 3, 0, 360);
        break;
    case Icon::Power:
        fl_arc(x + 3, y + 3, size - 6, size - 6, 135, 405);
        fl_line(cx, y + 1, cx, cy);
        break;
    case Icon::Freeze:
        for (int i = 0; i < 3; ++i) {
            const double a = Pi * i / 3;
            const int dx = int((size / 2 - 1) * std::cos(a)), dy = int((size / 2 - 1) * std::sin(a));
            fl_line(cx - dx, cy - dy, cx + dx, cy + dy);
        }
        break;
    case Icon::Edit:
        for (int row = 0; row < 3; ++row) {
            const int yy = y + 3 + row * (size - 6) / 2;
            fl_line(x + 1, yy, x + size - 1, yy);
            fl_rectf(x + (row % 2 ? size * 2 / 3 : size / 3) - 2, yy - 3, 4, 6);
        }
        break;
    default: break;
    }
    fl_line_style(0);
}

Panel::Panel(int direction) : Fl_Flex(0, 0, 100, 100, direction) {
    box(FL_NO_BOX); color(Surface); margin(16); gap(10);
}
void Panel::draw() {
    if (need_layout()) layout();
    if (damage() & ~FL_DAMAGE_CHILD) rounded(x(), y(), w(), h(), color());
    draw_children();
}

Button::Button(const char* label, Icon icon) : Fl_Button(0, 0, 100, 36, label), icon_(icon) {
    color(Surface); labelcolor(Text); labelsize(14); when(FL_WHEN_RELEASE);
}
void Button::draw() {
    const bool selected = value() || primary_;
    Fl_Color fill = selected ? Mint : (hover_ ? fl_color_average(Surface, Text, .93f) : color());
    Fl_Color edge = selected ? Mint : (Fl::focus() == this ? Mint : Border);
    const bool recording = icon_ == Icon::Record && value();
    if (recording) { fill = 0x552A2E00; edge = Red; }
    if (navigation_ && !selected) { fill = Sidebar; edge = Sidebar; }
    rounded(x(), y(), w(), h(), disabled(fill, active_r()), disabled(edge, active_r()), 6);
    const Fl_Color text = disabled(selected && !recording ? Ink : Text, active_r());
    const bool hasLabel = label() && label()[0];
    const int iconSize = 19;
    int contentWidth = 0;
    fl_font(selected || navigation_ ? FL_HELVETICA_BOLD : FL_HELVETICA, 14);
    if (hasLabel) contentWidth = int(fl_width(label()));
    if (icon_ != Icon::None) contentWidth += iconSize + (hasLabel ? 9 : 0);
    const int start = navigation_ ? x() + 13 : x() + (w() - contentWidth) / 2;
    if (icon_ != Icon::None) drawIcon(icon_, start, y() + (h() - iconSize) / 2, iconSize,
        icon_ == Icon::Record && !value() ? Red : text);
    labelText(label(), start + (icon_ != Icon::None ? iconSize + 9 : 0), y(),
        contentWidth, h(), 14, text, FL_ALIGN_LEFT | FL_ALIGN_INSIDE, selected || navigation_);
}
int Button::handle(int event) {
    if (event == FL_ENTER || event == FL_LEAVE) { hover_ = event == FL_ENTER; redraw(); }
    return Fl_Button::handle(event);
}

Toggle::Toggle(const char* label, bool showText) : Fl_Check_Button(0, 0, 40, 28, label), showText_(showText) {
    color(Surface); labelcolor(Text); when(FL_WHEN_CHANGED);
    align(FL_ALIGN_INSIDE);
}
void Toggle::draw() {
    fl_color(color()); fl_rectf(x(), y(), w(), h());
    const int left = x() + (showText_ ? 0 : std::max(0, (w() - 34) / 2));
    const int top = y() + (h() - 20) / 2;
    rounded(left, top, 34, 20, disabled(value() ? Mint : Track, active_r()),
        Fl::focus() == this ? Text : (value() ? Mint : Track), 10);
    fl_color(disabled(value() ? Text : Muted, active_r()));
    fl_pie(left + (value() ? 17 : 3), top + 3, 14, 14, 0, 360);
    if (showText_) labelText(label(), left + 45, y(), w() - 45, h(), 13, disabled(Text, active_r()));
}

Choice::Choice(const char* label) : Fl_Choice(0, 0, 100, 34, label) {
    color(Control); textcolor(Text); textfont(FL_HELVETICA); textsize(13);
    selection_color(Mint); down_box(FL_FLAT_BOX); box(FL_FLAT_BOX);
    align(FL_ALIGN_INSIDE);
}
void Choice::draw() {
    rounded(x(), y(), w(), h(), Control, Fl::focus() == this ? Mint : Border, 5);
    fl_push_clip(x() + 10, y() + 1, std::max(0, w() - 38), h() - 2);
    labelText(mvalue() ? mvalue()->label() : "", x() + 11, y(), w() - 38, h(), textsize(), disabled(Text, active_r()));
    fl_pop_clip();
    fl_color(disabled(Muted, active_r())); fl_line_style(FL_SOLID, 1);
    fl_line(x() + w() - 23, y() + h() / 2 - 2, x() + w() - 18, y() + h() / 2 + 3,
            x() + w() - 13, y() + h() / 2 - 2); fl_line_style(0);
}
Menu::Menu() : Fl_Menu_Button(0, 0, 38, 36) {
    color(Control); textcolor(Text); textsize(14); selection_color(Mint); down_box(FL_FLAT_BOX);
    tooltip("Presets, source files, and application options");
}
void Menu::draw() {
    rounded(x(), y(), w(), h(), Control, Fl::focus() == this ? Mint : Border, 5);
    drawIcon(Icon::More, x() + (w() - 20) / 2, y() + (h() - 20) / 2, 20, Text);
}

Value::Value(const char* label, Style style, Units units)
    : Fl_Valuator(0, 0, 100, style == Style::Dial ? 118 : 32, label), style_(style), units_(units) {
    color(Surface); when(FL_WHEN_CHANGED); step(.01);
    align(FL_ALIGN_INSIDE);
    tooltip("Drag to adjust. Arrow keys fine-tune. Double-click the value to type a number.");
}
double Value::position() const {
    if (maximum() <= minimum()) return 0;
    if (units_ == Units::Decibels) {
        const double upper = 20 * std::log10(maximum());
        const double db = value() > 0 ? 20 * std::log10(value()) : -60;
        return std::clamp((db + 60) / (upper + 60), 0.0, 1.0);
    }
    return std::clamp((value() - minimum()) / (maximum() - minimum()), 0.0, 1.0);
}
double Value::fromPosition(double pos) const {
    pos = std::clamp(pos, 0.0, 1.0);
    if (units_ == Units::Decibels) {
        if (pos <= .001) return 0;
        return std::pow(10.0, (-60 + pos * (60 + 20 * std::log10(maximum()))) / 20);
    }
    return minimum() + pos * (maximum() - minimum());
}
std::string Value::formatted(double value) const {
    char text[64];
    switch (units_) {
    case Units::Decibels:
        if (value <= .00001) return "-inf dB";
        std::snprintf(text, sizeof(text), "%.1f dB", 20 * std::log10(value)); break;
    case Units::Percent: std::snprintf(text, sizeof(text), "%.0f%%", value * 100); break;
    case Units::Pan:
        if (std::abs(value - .5) < .005) return "C";
        std::snprintf(text, sizeof(text), "%s %.0f", value < .5 ? "L" : "R", std::abs(value - .5) * 200); break;
    case Units::Ratio: std::snprintf(text, sizeof(text), "%.2fx", value); break;
    case Units::Hertz: std::snprintf(text, sizeof(text), "%.0f Hz", value); break;
    case Units::Semitones: std::snprintf(text, sizeof(text), "%+.0f st", value); break;
    case Units::Seconds: std::snprintf(text, sizeof(text), "%.2f s", value); break;
    default:
        std::snprintf(text, sizeof(text), step() >= 1 ? "%.0f" : step() < .01 ? "%.3f" : "%.2f", value); break;
    }
    return text;
}
void Value::draw() {
    fl_color(color()); fl_rectf(x(), y(), w(), h());
    const double pos = position(); const bool enabled = active_r();
    if (style_ == Style::Slider) {
        const int trackX = x() + 8, trackW = std::max(8, w() - 88), cy = y() + h() / 2;
        fl_color(disabled(Track, enabled)); fl_rounded_rectf(trackX, cy - 2, trackW, 4, 2);
        fl_color(disabled(Mint, enabled)); fl_rounded_rectf(trackX, cy - 2, std::max(0, int(pos * trackW)), 4, 2);
        fl_color(disabled(Text, enabled)); fl_pie(trackX + int(pos * trackW) - 6, cy - 6, 12, 12, 0, 360);
        labelText(formatted(value()).c_str(), x() + w() - 76, y(), 73, h(), 12,
            disabled(Text, enabled), FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
    } else {
        const int radius = std::min(w() / 2 - 12, (h() - 38) / 2);
        if (radius < 8) return;
        const int cx = x() + w() / 2, cy = y() + radius + 7;
        const double angle = (225 - pos * 270) * Pi / 180;
        fl_line_style(FL_SOLID | FL_CAP_ROUND, 4);
        fl_color(disabled(Track, enabled)); fl_arc(cx - radius, cy - radius, 2 * radius, 2 * radius, -45, 225);
        if (pos > .001) {
            fl_color(disabled(Mint, enabled)); fl_arc(cx - radius, cy - radius, 2 * radius, 2 * radius, 225 - pos * 270, 225);
        }
        fl_line_style(FL_SOLID | FL_CAP_ROUND, 2); fl_color(disabled(Text, enabled));
        fl_line(cx + int((radius - 4) * std::cos(angle)), cy - int((radius - 4) * std::sin(angle)),
                cx + int((radius - 15) * std::cos(angle)), cy - int((radius - 15) * std::sin(angle)));
        fl_line_style(0);
        labelText(formatted(value()).c_str(), x(), y() + h() - 26, w(), 24, 14, disabled(Text, enabled), FL_ALIGN_CENTER);
    }
    if (Fl::focus() == this) { fl_color(Mint); fl_rounded_rect(x(), y(), w() - 1, h() - 1, 4); }
}
void Value::editValue() {
    if (typing_) return;
    typing_ = true;
    double display = value();
    if (units_ == Units::Decibels) display = value() > 0 ? 20 * std::log10(value()) : -60;
    else if (units_ == Units::Percent) display *= 100;
    else if (units_ == Units::Pan) display = (display - .5) * 200;
    char initial[64]; std::snprintf(initial, sizeof(initial), "%.4g", display);
    const char* input = fl_input("%s", initial, label() ? label() : "Value");
    if (input) {
        char* end = nullptr; double v = std::strtod(input, &end);
        while (end && *end == ' ') ++end;
        if (end && end != input && *end == '\0' && std::isfinite(v)) {
            if (units_ == Units::Decibels) v = v <= -60 ? 0 : std::pow(10.0, v / 20);
            else if (units_ == Units::Percent) v /= 100;
            else if (units_ == Units::Pan) v = .5 + v / 200;
            handle_push(); handle_drag(clamp(round(v))); handle_release();
        }
    }
    typing_ = false;
}
int Value::handle(int event) {
    switch (event) {
    case FL_ENTER: case FL_LEAVE: return 1;
    case FL_FOCUS: case FL_UNFOCUS: redraw(); return 1;
    case FL_PUSH:
        if (Fl::event_button() != FL_LEFT_MOUSE) return 0;
        take_focus(); handle_push(); dragStart_ = position(); dragY_ = Fl::event_y();
        dragRelative_ = style_ == Style::Dial || Fl::event_x() >= x() + w() - 78;
        if (Fl::event_clicks() > 0) { editValue(); return 1; }
        if (style_ == Style::Slider && Fl::event_x() < x() + w() - 78)
            handle_drag(clamp(round(fromPosition(double(Fl::event_x() - x() - 8) / std::max(8, w() - 88)))));
        return 1;
    case FL_DRAG:
        if (dragRelative_) {
            const double speed = Fl::event_state(FL_SHIFT) ? 1200.0 : 180.0;
            handle_drag(clamp(round(fromPosition(dragStart_ + (dragY_ - Fl::event_y()) / speed))));
        } else handle_drag(clamp(round(fromPosition(double(Fl::event_x() - x() - 8) / std::max(8, w() - 88)))));
        return 1;
    case FL_RELEASE: handle_release(); return 1;
    case FL_MOUSEWHEEL:
        if (Fl::focus() != this) return 0;
        handle_push(); handle_drag(clamp(round(increment(value(), -Fl::event_dy())))); handle_release(); return 1;
    case FL_KEYDOWN: {
        const int key = Fl::event_key();
        if (key == FL_Enter || key == FL_KP_Enter) { editValue(); return 1; }
        int direction = key == FL_Up || key == FL_Right ? 1 : key == FL_Down || key == FL_Left ? -1 : 0;
        if (!direction) return 0;
        handle_push(); handle_drag(clamp(round(increment(value(), direction * (Fl::event_state(FL_SHIFT) ? 10 : 1))))); handle_release(); return 1;
    }
    default: return 0;
    }
}

Meter::Meter(bool stereo) : Fl_Widget(0, 0, stereo ? 44 : 18, 100), stereo_(stereo) { color(Surface); }
void Meter::levels(float left, float right) {
    left_ = std::max(left, left_ * .84f); right_ = std::max(right, right_ * .84f); redraw();
}
void Meter::draw() {
    fl_color(color()); fl_rectf(x(), y(), w(), h());
    const int columns = stereo_ ? 2 : 1, barW = std::min(15, (w() - 6) / columns - 5);
    const int top = y() + 18, segments = std::max(4, (h() - 34) / 7), base = top + segments * 7;
    for (int col = 0; col < columns; ++col) {
        const int xx = x() + (w() - columns * (barW + 6)) / 2 + col * (barW + 6);
        labelText(stereo_ ? (col ? "R" : "L") : "In", xx - 5, y(), barW + 10, 16, 10, Muted, FL_ALIGN_CENTER);
        const double value = col ? right_ : left_;
        const double db = value > .00001 ? 20 * std::log10(value) : -60;
        const int active = int(std::clamp((db + 48) / 48, 0.0, 1.0) * segments);
        for (int row = 0; row < segments; ++row) {
            fl_color(row < active ? (row > segments - 3 ? (value >= .99 ? Red : 0xE2D99300) : Mint) : 0x343D4000);
            fl_rectf(xx, base - (row + 1) * 7, barW, 5);
        }
    }
}

PitchDisplay::PitchDisplay() : Fl_Widget(0, 0, 600, 126) { color(Surface); }
void PitchDisplay::sample(double pitch, bool running) {
    if (running && !running_) history_.fill(0);
    running_ = running;
    pitch_ = running && std::isfinite(pitch) && pitch > 20 ? pitch : 0;
    if (pitch_ > 0) {
        const double midi = 69 + 12 * std::log2(pitch_ / 440);
        if (std::abs(midi - center_) > 2) center_ = std::round(midi);
        history_[cursor_] = midi;
    } else history_[cursor_] = 0;
    cursor_ = (cursor_ + 1) % history_.size(); redraw();
}
void PitchDisplay::draw() {
    rounded(x(), y(), w(), h(), color());
    labelText("DETECTED PITCH", x() + 20, y() + 17, 230, 20, 11, Muted, FL_ALIGN_LEFT | FL_ALIGN_INSIDE, true);
    std::string note = "--"; char frequency[64] = "Awaiting input";
    if (pitch_ > 0) {
        static const char* names[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
        const int midi = int(std::round(69 + 12 * std::log2(pitch_ / 440)));
        note = std::string(names[(midi % 12 + 12) % 12]) + std::to_string(midi / 12 - 1);
        std::snprintf(frequency, sizeof(frequency), "%.1f Hz", pitch_);
    }
    labelText(note.c_str(), x() + 18, y() + 46, 100, 52, 38, Text, FL_ALIGN_LEFT | FL_ALIGN_INSIDE, true);
    labelText(frequency, x() + 120, y() + 57, 145, 32, pitch_ > 0 ? 20 : 13, pitch_ > 0 ? Text : Muted);
    const int gx = x() + 292, gy = y() + 20, gw = w() - 313, gh = h() - 47;
    if (gw < 30) return;
    fl_color(0x39434700); fl_rect(gx, gy, gw, gh);
    for (int i = 1; i < 5; ++i) fl_line(gx + i * gw / 5, gy, gx + i * gw / 5, gy + gh);
    fl_color(0x313B3E00); fl_line(gx, gy + gh / 2, gx + gw, gy + gh / 2);
    fl_push_clip(gx + 1, gy + 1, gw - 2, gh - 2);
    fl_color(Mint); fl_line_style(FL_SOLID | FL_CAP_ROUND, 2);
    bool connected = false;
    for (std::size_t i = 0; i < history_.size(); ++i) {
        const double pitch = history_[(cursor_ + i) % history_.size()];
        if (!running_ || pitch <= 0 || std::abs(pitch - center_) > 3) {
            if (connected) fl_end_line(); connected = false; continue;
        }
        if (!connected) { fl_begin_line(); connected = true; }
        fl_vertex(gx + double(i) / (history_.size() - 1) * gw, gy + gh / 2 - (pitch - center_) * gh / 4);
    }
    if (connected) fl_end_line(); fl_line_style(0); fl_pop_clip();
    labelText("10 seconds", gx, gy + gh + 7, gw, 15, 10, Muted, FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
    if (!running_) labelText("Start audio to see pitch history", gx, gy, gw, gh, 12, Muted, FL_ALIGN_CENTER);
}

PageStack::PageStack() : Fl_Group(0, 0, 600, 600) { color(Background); box(FL_FLAT_BOX); }
void PageStack::resize(int x, int y, int w, int h) {
    Fl_Widget::resize(x, y, w, h);
    for (int i = 0; i < children(); ++i) child(i)->resize(x, y, w, h);
    redraw();
}
void PageStack::showPage(int page) {
    for (int i = 0; i < children(); ++i) { if (i == page) child(i)->show(); else child(i)->hide(); }
    redraw();
}
} // namespace studio
