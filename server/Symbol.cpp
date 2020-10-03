//
// Created by gheb on 31/07/20.
//

#include "Symbol.h"


Symbol::Symbol(wchar_t character, std::pair<int, int> id, std::vector<int> pos)
        : ch(character), id(std::move(id)), pos(std::move(pos)), style() {}

Symbol::Symbol(wchar_t character, std::pair<int, int> id, std::vector<int> pos, SymStyle style)
        : ch(character), id(std::move(id)), pos(std::move(pos)), style(std::move(style)) {}

std::vector<int> Symbol::get_pos() const {
    return pos;
}

wchar_t Symbol::get_char() const {
    return ch;
}

std::pair<int, int> Symbol::get_id() const {
    return id;
}

SymStyle Symbol::get_style() const {
    return style;
}

void Symbol::set_char(wchar_t letter) {
    this->ch = letter;
}

void Symbol::set_id(std::pair<int, int> id) {
    this->id = id;
}

void Symbol::set_pos(std::vector<int> pos) {
    this->pos = std::move(pos);
}

void Symbol::set_style(SymStyle style) {
    this->style = std::move(style);
}

