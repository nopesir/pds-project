#include "symbol.h"
#include <utility>

Symbol::Symbol(wchar_t character, std::pair<int, int> id, std::vector<int> pos)
    : _letter(character), _id(std::move(id)), _pos(std::move(pos)), _style() {}

Symbol::Symbol(wchar_t character, SymbolStyle style)
    : _letter(character), _style(std::move(style)) {}

Symbol::Symbol(wchar_t character, std::pair<int, int> id, std::vector<int> pos, SymbolStyle style)
    : _letter(character), _id(std::move(id)), _pos(std::move(pos)), _style(std::move(style)) {}

std::vector<int> Symbol::getPos() const {
    return _pos;
}

wchar_t Symbol::getLetter() const {
    return _letter;
}

std::pair<int, int> Symbol::getId() const {
    return _id;
}

SymbolStyle Symbol::getStyle() const {
    return _style;
}

void Symbol::setLetter(wchar_t letter) {
    this->_letter = letter;
}

void Symbol::setId(std::pair<int, int> id) {
    this->_id = id;
}

void Symbol::setPos(std::vector<int> pos) {
    this->_pos = std::move(pos);
}

void Symbol::setStyle(SymbolStyle style) {
    this->_style = std::move(style);
}


