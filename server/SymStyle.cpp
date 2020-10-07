//
// Created by gheb on 31/07/20.
//

#include "SymStyle.h"


SymStyle::SymStyle()
        : is_bold(false), is_italic(false), is_underlined(false), font_family(DEFAULT_FONT_FAMILY), font_sz(DEFAULT_FONT_SIZE),
          alignment(DEFAULT_ALIGNMENT), color(DEFAULT_COLOR) {}

SymStyle::SymStyle(bool isBold, bool isItalic, bool isUnderlined, std::string fontFamily, int fontSize, int alignment, std::string color)
        : is_bold(isBold), is_italic(isItalic), is_underlined(isUnderlined), font_family(std::move(fontFamily)), font_sz(fontSize),
          alignment(alignment), color(std::move(color)) {}

std::string SymStyle::get_font_family() const {
    return font_family;
}

int SymStyle::get_font_sz() const {
    return font_sz;
}

int SymStyle::get_alignment() const {
    return alignment;
}

std::string SymStyle::getColor() const {
    return color;
}

bool SymStyle::isBold() const {
    return is_bold;
}

bool SymStyle::isItalic() const {
    return is_italic;
}

bool SymStyle::isUnderlined() const {
    return is_underlined;
}

void SymStyle::set_bold(bool bold) {
    this->is_bold = bold;
}

void SymStyle::set_italic(bool italic) {
    this->is_italic = italic;
}

void SymStyle::set_underlined(bool underlined) {
    this->is_underlined = underlined;
}

void SymStyle::set_font_family(std::string family) {
    this->font_family = std::move(family);
}

void SymStyle::set_font_sz(int size) {
    this->font_sz = size;
}

void SymStyle::set_alignment(int value) {
    this->alignment = value;
}

void SymStyle::set_color(std::string clr) {
    this->color = std::move(clr);
}