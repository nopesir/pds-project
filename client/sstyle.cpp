
#include "sstyle.h"
#include <utility>

SymbolStyle::SymbolStyle()
        : _isBold(false), _isItalic(false), _isUnderlined(false), _fontFamily(DEFAULT_FONT_FAMILY), _fontSize(DEFAULT_FONT_SIZE),
          _alignment(DEFAULT_ALIGNMENT), _color(DEFAULT_COLOR) {}

SymbolStyle::SymbolStyle(bool isBold, bool isItalic, bool isUnderlined, std::string fontFamily, int fontSize, int alignment, std::string color)
    : _isBold(isBold), _isItalic(isItalic), _isUnderlined(isUnderlined), _fontFamily(std::move(fontFamily)), _fontSize(fontSize),
      _alignment(alignment), _color(std::move(color)) {}

std::string SymbolStyle::getFontFamily() const {
    return _fontFamily;
}

int SymbolStyle::getFontSize() const {
    return _fontSize;
}

int SymbolStyle::getAlignment() const {
    return _alignment;
}

std::string SymbolStyle::getColor() const {
    return _color;
}

bool SymbolStyle::isBold() const {
    return _isBold;
}

bool SymbolStyle::isItalic() const {
    return _isItalic;
}

bool SymbolStyle::isUnderlined() const {
    return _isUnderlined;
}

void SymbolStyle::setBold(bool val) {
    this->_isBold = val;
}

void SymbolStyle::setItalic(bool val) {
    this->_isItalic = val;
}

void SymbolStyle::setUnderlined(bool val) {
    this->_isUnderlined = val;
}

void SymbolStyle::setFontFamily(std::string fontFamily) {
    this->_fontFamily = std::move(fontFamily);
}

void SymbolStyle::setFontSize(int value) {
    this->_fontSize = value;
}

void SymbolStyle::setAlignment(int value) {
    this->_alignment = value;
}

void SymbolStyle::setColor(std::string value) {
    this->_color = std::move(value);
}

