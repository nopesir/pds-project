//
// Created by gheb on 31/07/20.
//

#ifndef PROJECT_SYMBOL_H
#define PROJECT_SYMBOL_H


#include <vector>
#include "SymStyle.h"

class Symbol {

private:
    wchar_t ch;
    std::pair<int, int> id;
    std::vector<int> pos;
    SymStyle style;

public:
    /* Constructors */
    Symbol() = default;
    Symbol(wchar_t character, std::pair<int, int> id, std::vector<int> pos);
    Symbol(wchar_t character, std::pair<int, int> id, std::vector<int> pos, SymStyle style);

    /* Getters */
    std::vector<int> get_pos() const;
    wchar_t get_char() const;
    std::pair<int, int> get_id() const;
    SymStyle get_style() const;

    /* Setters */
    void set_char(wchar_t letter);
    void set_id(std::pair<int,int> id);
    void set_pos(std::vector<int> pos);
    void set_style(SymStyle style);

};


#endif //PROJECT_SYMBOL_H
