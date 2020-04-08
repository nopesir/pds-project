#ifndef TEMPLATESYMBOL_H
#define TEMPLATESYMBOL_H

#include <vector>
#include <array>
#include "sstyle.h"

/* This class is like a "fake" symbol, that is created by the client and sent to the server, that creates a real symbol from it,
 * sending it to the client. */
class TemplateSymbol {

private:
    int _index;
    wchar_t _letter;
    SymbolStyle _style;

public:
    /* Constructors */
    TemplateSymbol(int index, wchar_t letter, SymbolStyle style);

    /* Getters */
    int getIndex() const;
    wchar_t getLetter() const;
    SymbolStyle getStyle() const;

    /* Setters */
    void setIndex(int index);
    void setLetter(wchar_t letter);
    void setStyle(SymbolStyle style);
};

#endif // TEMPLATESYMBOL_H
