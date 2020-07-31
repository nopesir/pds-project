//
// Created by gheb on 31/07/20.
//

#ifndef PROJECT_SYMBOL_H
#define PROJECT_SYMBOL_H


#include <vector>
#include "SymbolStyle.h"

class Symbol {

private:
    wchar_t _letter;
    std::pair<int, int> _id;
    std::vector<int> _pos;
    SymbolStyle _style;

public:
    /* Constructors */
    Symbol() = default;
    Symbol(wchar_t character, std::pair<int, int> id, std::vector<int> pos);
    Symbol(wchar_t character, std::pair<int, int> id, std::vector<int> pos, SymbolStyle style);

    /* Getters */
    std::vector<int> getPos() const;
    wchar_t getLetter() const;
    std::pair<int, int> getId() const;
    SymbolStyle getStyle() const;

    /* Setters */
    void setLetter(wchar_t letter);
    void setId(std::pair<int,int> id);
    void setPos(std::vector<int> pos);
    void setStyle(SymbolStyle style);

};


#endif //PROJECT_SYMBOL_H
