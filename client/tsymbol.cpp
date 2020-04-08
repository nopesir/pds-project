#include "tsymbol.h"
#include <utility>

TemplateSymbol::TemplateSymbol(int index, wchar_t letter, SymbolStyle style)
        : _index(index), _letter(letter), _style(std::move(style)) {}

int TemplateSymbol::getIndex() const {
    return _index;
}

wchar_t TemplateSymbol::getLetter() const {
    return _letter;
}

SymbolStyle TemplateSymbol::getStyle() const {
    return _style;
}

void TemplateSymbol::setIndex(int index) {
    this->_index = index;
}

void TemplateSymbol::setLetter(wchar_t letter) {
    this->_letter = letter;
}

void TemplateSymbol::setStyle(SymbolStyle style) {
    this->_style = std::move(style);
}
