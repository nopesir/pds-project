#include "crdt.h"
#include <algorithm>
#include <iostream>
#include <utility>

std::vector<int> crdt::generatePos(int index) {
    const std::vector<int> posBefore = _symbols[index-1].getPos();
    const std::vector<int> posAfter = _symbols[index].getPos();
    return generatePosBetween(posBefore, posAfter);
}

std::vector<int> crdt::generatePosBetween(std::vector<int> pos1, std::vector<int> pos2, std::vector<int> newPos) {
    int id1 = pos1.at(0);
    int id2 = pos2.at(0);

    if(id2 - id1 == 0) { // [1] [1 0] or [1 0] [1 1]
        newPos.push_back(id1);
        pos1.erase(pos1.begin());
        pos2.erase(pos2.begin());
        if(pos1.empty()) {
            newPos.push_back(pos2.front()-1); // [1] [1 0] -> [1 -1]
            return newPos;
        } else
            return generatePosBetween(pos1, pos2, newPos); // [1 0] [1 1] -> recall and enter third if
    }
    else if(id2 - id1 > 1) { // [0] [3]
        newPos.push_back(pos1.front()+1); // [0] [3] -> [1]
        return newPos;
    }
    else if(id2 - id1 == 1) { // [1] [2] or [1 1] [2]
        newPos.push_back(id1);
        pos1.erase(pos1.begin());
        if(pos1.empty()) {
            newPos.push_back(0); // [1] [2] -> [1 0]
            return newPos;
        } else {
            newPos.push_back(pos1.front()+1); // [1 1] [2] -> [1 2]
            return newPos;
        }
    }
}

int crdt::comparePosdx(std::vector<int> curSymPos, std::pair<int,int> curSymId, std::vector<int> newSymPos, std::pair<int,int> newSymId, int posIndex) {
    int newSymPosSize = static_cast<int>(newSymPos.size());
    int curSymPosSize = static_cast<int>(curSymPos.size());
    int curSymPosCurIndex = static_cast<int>(curSymPos.at(posIndex));
    int newSymPosCurIndex = static_cast<int>(newSymPos.at(posIndex));

    if (curSymPosCurIndex < newSymPosCurIndex)
        return 1;
    else if (curSymPosCurIndex == newSymPosCurIndex) {
        if (newSymPosSize > posIndex + 1 &&
            curSymPosSize <= posIndex + 1) //newSymPos[posIndex+1] != null && curSymPos[posIndex+1] == null
            return 1; // correct position found
        else if (newSymPosSize <= posIndex + 1 &&
                 curSymPosSize > posIndex + 1) //newSymPos[posIndex+1] == null && curSymPos[posIndex+1] != null
            return -1; //curSymPos > newSymPos  -> make another cycle taking the next symbol from _symbols
        else if (newSymPosSize > posIndex + 1 &&
                 curSymPosSize > posIndex + 1) //newSymPos[posIndex+1] != null && curSymPos[posIndex+1] != null
            return comparePosdx(curSymPos, curSymId, newSymPos,
                               newSymId, posIndex + 1); //call recursively this function using next index for posIndex
        else //newSymPos[posIndex+1] == null && curSymPos[posIndex+1] == null
            return newSymId > curSymId ? 1 : -1;
    } else
        return -1; //make another cycle taking the next symbol from _symbols
}

int crdt::comparePos(std::vector<int> curSymPos, std::pair<int,int> curSymId, std::vector<int> newSymPos, std::pair<int,int> newSymId, int posIndex) {
    int newSymPosSize = static_cast<int>(newSymPos.size());
    int curSymPosSize = static_cast<int>(curSymPos.size());
    int curSymPosCurIndex = static_cast<int>(curSymPos.at(posIndex));
    int newSymPosCurIndex = static_cast<int>(newSymPos.at(posIndex));

    if(curSymPosCurIndex > newSymPosCurIndex)
        return 1; //correct position found
    else if (curSymPosCurIndex == newSymPosCurIndex) {
        if (newSymPosSize > posIndex + 1 &&
            curSymPosSize <= posIndex + 1) //newSymPos[posIndex+1] != null && curSymPos[posIndex+1] == null
            return -1; //newSymPos > curSymPos -> make another cycle taking the next symbol from _symbols
        else if (newSymPosSize <= posIndex + 1 &&
                 curSymPosSize > posIndex + 1) //newSymPos[posIndex+1] == null && curSymPos[posIndex+1] != null
            return 1; //correct position found
        else if (newSymPosSize > posIndex + 1 &&
                 curSymPosSize > posIndex + 1) //newSymPos[posIndex+1] != null && curSymPos[posIndex+1] != null
            return comparePos(curSymPos, curSymId, newSymPos,
                              newSymId, posIndex + 1); //call recursively this function using next index for posIndex
        else //newSymPos[posIndex+1] == null && curSymPos[posIndex] == null
            return newSymId.first < curSymId.first ? 1 : -1;
    } else
        return -1; //make another cycle taking the next symbol from _symbols
}

Symbol crdt::localInsert(int index, wchar_t value, SymbolStyle style) noexcept(false) {
    std::vector<int> pos;

    if(_symbols.empty()) {
        pos = {0};
        index = 0;
    } else if(index > static_cast<int>(_symbols.size())-1) {
        pos = {_symbols.back().getPos().at(0) + 1}; //last element will not have fraction -> pos will be [x] not [x,y]
        index = static_cast<int>(_symbols.size());
    } else if(index == 0) {
        pos = {_symbols.front().getPos().at(0) - 1}; //put negative pos
    } else
        pos = generatePos(index);

    Symbol s(value, std::make_pair(_siteId, ++_counter), pos, std::move(style));
    _symbols.insert(_symbols.begin() + index, s);

    return s;
}

std::vector<Symbol> crdt::localInsert(int startIndex, std::vector<Symbol> symbols) noexcept(false) {
    std::vector<int> pos;

    //generate initial pos and initial index
    if(_symbols.empty()) {
        pos = {0};
        startIndex = 0;
    } else if(startIndex > static_cast<int>(_symbols.size())-1) {
        pos = {_symbols.back().getPos().at(0) + 1}; //last element will not have fraction -> pos will be [x] not [x,y]
        startIndex = static_cast<int>(_symbols.size());
    } else if(startIndex == 0) {
        pos = {_symbols.front().getPos().at(0) - 1}; //put negative pos
    } else
        pos = generatePos(startIndex);

    std::vector<Symbol> symbolVector;
    bool firstTime = true;
    bool secondTime = true;
    int counter = 0;

    std::for_each(symbols.begin(), symbols.end(), [&firstTime, &secondTime, &counter, &pos, &symbolVector, this](const Symbol& s) {
        //get values
        wchar_t value = s.getLetter();
        SymbolStyle style = s.getStyle();

        //generate next pos
        if(firstTime)
            firstTime = false;
        else {
            if(secondTime) {
                pos.push_back(counter++);
                secondTime = false;
            }
            else
                pos.back() = counter++;
        }

        //insert symbol
        Symbol sym(value, std::make_pair(_siteId, ++_counter), pos, std::move(style));
        symbolVector.push_back(sym);
    });
    _symbols.insert(_symbols.begin() + startIndex, symbolVector.begin(), symbolVector.end());
    return std::move(symbolVector);
}

std::vector<sId> crdt::localErase(int startIndex, int endIndex) noexcept(false) {
    //create vector of id to be sent (in removal we need only id, not entire symbol)
    std::vector<sId> symbolsId;
    std::for_each(_symbols.begin() + startIndex, _symbols.begin() + endIndex, [&symbolsId](const Symbol& s) {
        symbolsId.push_back(s.getId());
    });

    //erase local symbols
    _symbols.erase(_symbols.begin() + startIndex, _symbols.begin() + endIndex);
    return symbolsId;
}

std::vector<sId> crdt::localFormat(int startIndex, int endIndex, int format) noexcept(false) {
    //create vector of id to be sent (in removal we need only id, not entire symbol)
    std::vector<sId> symbolsId;

    std::for_each(_symbols.begin() + startIndex, _symbols.begin() + endIndex, [&symbolsId, format](Symbol& s) {
        //put id in symbolsId
        symbolsId.push_back(s.getId());

        //change format of current symbol
        SymbolStyle style = s.getStyle();

        if(format == MAKE_BOLD)
            style.setBold(true);
        else if(format == MAKE_ITALIC)
            style.setItalic(true);
        else if(format == MAKE_UNDERLINE)
            style.setUnderlined(true);
        else if(format == UNMAKE_BOLD)
            style.setBold(false);
        else if(format == UNMAKE_ITALIC)
            style.setItalic(false);
        else if(format == UNMAKE_UNDERLINE)
            style.setUnderlined(false);

        //set new format to the current symbol
        s.setStyle(style);
    });
    return symbolsId;
}

std::vector<sId> crdt::localFontSizeChange(int startIndex, int endIndex, int fontSize) noexcept(false) {
    //create vector of id to be sent (in removal we need only id, not entire symbol)
    std::vector<sId> symbolsId;

    std::for_each(_symbols.begin() + startIndex, _symbols.begin() + endIndex, [&symbolsId, fontSize](Symbol& s) {
        //put id in symbolsId
        symbolsId.push_back(s.getId());

        //set new font size to the current symbol
        SymbolStyle style = s.getStyle();
        style.setFontSize(fontSize);
        s.setStyle(style);
    });
    return symbolsId;
}

std::vector<sId> crdt::localFontFamilyChange(int startIndex, int endIndex, const std::string& fontFamily) noexcept(false) {
    //create vector of id to be sent (in removal we need only id, not entire symbol)
    std::vector<sId> symbolsId;

    std::for_each(_symbols.begin() + startIndex, _symbols.begin() + endIndex, [&symbolsId, fontFamily](Symbol& s) {
        //put id in symbolsId
        symbolsId.push_back(s.getId());

        //set new font size to the current symbol
        SymbolStyle style = s.getStyle();
        style.setFontFamily(fontFamily);
        s.setStyle(style);
    });
    return symbolsId;
}

std::vector<sId> crdt::localAlignmentChange(int startIndex, int endIndex, int alignment) noexcept(false) {
    //create vector of id to be sent (in removal we need only id, not entire symbol)
    std::vector<sId> symbolsId;
    std::for_each(_symbols.begin() + startIndex, _symbols.begin() + endIndex, [&symbolsId, alignment](Symbol& s) {
        //put id in symbolsId
        symbolsId.push_back(s.getId());

        //set new font size to the current symbol
        SymbolStyle style = s.getStyle();
        style.setAlignment(alignment);
        s.setStyle(style);
    });
    return symbolsId;
}

int crdt::process(int type, int indexEditor, Symbol newSym) {
    /* Insertion */
    if (type == 0) {
        int symbols_index = 0, pos_index = 0;
        int startIndex = static_cast<int>(_symbols.size());

        //get first index
        if (indexEditor > static_cast<int>(_symbols.size()/2)) { //LOOP FROM RIGHT TO LEFT
            for (auto s = _symbols.crbegin(); s != _symbols.crend(); s++) {
                startIndex--;
                int retValue = comparePosdx(s->getPos(), s->getId(), newSym.getPos(), newSym.getId(), pos_index);

                if (retValue == -1)
                    continue;
                else if (retValue == 1) {
                    startIndex ++;
                    break;
                }
            }
        }
        else { //LOOP FROM LEFT TO RIGHT
            for (const auto &s: _symbols) {
                symbols_index++;
                int retValue = comparePos(s.getPos(), s.getId(), newSym.getPos(), newSym.getId(), pos_index);

                if (retValue == -1)
                    continue;
                else if (retValue == 1) {
                    startIndex = symbols_index - 1;
                    break;
                }
            }
        }
        //update symbols vector
        _symbols.insert(_symbols.begin() + startIndex, newSym);
        return startIndex;
    }
}

int crdt::process(int type, int indexEditor, std::vector<Symbol> newSymbols) {
    /* Insertion range */
    if(type == 6) {
        int symbols_index = 0, pos_index = 0;
        int startIndex = static_cast<int>(_symbols.size());

        //get first index
        if(indexEditor > static_cast<int>(_symbols.size()/2)) {  //LOOP FROM RIGHT TO LEFT
            for (auto s = _symbols.crbegin(); s != _symbols.crend(); s++) {
                startIndex--;
                int retValue = comparePosdx(s->getPos(), s->getId(), newSymbols.at(0).getPos(),
                                            newSymbols.at(0).getId(), pos_index);
                if (retValue == -1)
                    continue;
                else if (retValue == 1) {
                    startIndex ++;
                    break;
                }
            }
        }
        else { //LOOP FROM LEFT TO RIGHT
            for (const auto &s: _symbols) {
                symbols_index++;
                int retValue = comparePos(s.getPos(), s.getId(), newSymbols.at(0).getPos(),
                                          newSymbols.at(0).getId(), pos_index);
                if (retValue == -1)
                    continue;
                else if (retValue == 1) {
                    startIndex = symbols_index - 1;
                    break;
                }
            }
        }
        //update symbols vector
        _symbols.insert(_symbols.begin() + startIndex, newSymbols.begin(), newSymbols.end());
        return startIndex;
    }
}

int crdt::processErase(sId id) {
    auto it = std::find_if(_symbols.begin(), _symbols.end(), [id](const Symbol& s) {return s.getId() == id;});
    if (it != _symbols.end()) {
        int index = it - _symbols.begin();

        //update symbols vector
        _symbols.erase(_symbols.begin() + index);
        return index;
    }
    return -1;
}

int crdt::processFormat(sId id, int format) {
    auto it = std::find_if(_symbols.begin(), _symbols.end(), [id](const Symbol& s) {return s.getId() == id;});
    if (it != _symbols.end()) {
        int index = it - _symbols.begin();
        SymbolStyle style = _symbols.at(index).getStyle();

        if(format == MAKE_BOLD)
            style.setBold(true);
        else if(format == MAKE_ITALIC)
            style.setItalic(true);
        else if(format == MAKE_UNDERLINE)
            style.setUnderlined(true);
        else if(format == UNMAKE_BOLD)
            style.setBold(false);
        else if(format == UNMAKE_ITALIC)
            style.setItalic(false);
        else if(format == UNMAKE_UNDERLINE)
            style.setUnderlined(false);

        //update symbols vector
        _symbols.at(index).setStyle(style);
        return index;
    }
    return -1;
}

int crdt::processFontSize(sId id, int fontSize) {
    auto it = std::find_if(_symbols.begin(), _symbols.end(), [id](const Symbol& s) {return s.getId() == id;});
    if (it != _symbols.end()) {
        int index = it - _symbols.begin();
        SymbolStyle style = _symbols.at(index).getStyle();
        style.setFontSize(fontSize);

        //update symbols vector
        _symbols.at(index).setStyle(style);
        return index;
    }
    return -1;
}

int crdt::processFontFamily(sId id, const std::string& fontFamily) {
    auto it = std::find_if(_symbols.begin(), _symbols.end(), [id](const Symbol& s) {return s.getId() == id;});
    if (it != _symbols.end()) {
        int index = it - _symbols.begin();
        SymbolStyle style = _symbols.at(index).getStyle();
        style.setFontFamily(fontFamily);

        //update symbols vector
        _symbols.at(index).setStyle(style);
        return index;
    }
    return -1;
}

int crdt::processAlignment(sId id, int alignment) {
    //check also if alignments are different because server send to all clients (including me) in case of ALIGNMENT_UPDATE
    auto it = std::find_if(_symbols.begin(), _symbols.end(), [id, alignment](const Symbol& s) {
        return s.getId() == id /*&& s.getStyle().getAlignment() != alignment*/;});
    if (it != _symbols.end()) {
        int index = it - _symbols.begin();
        SymbolStyle style = _symbols.at(index).getStyle();
        style.setAlignment(alignment);

        //update symbols vector
        _symbols.at(index).setStyle(style);
        return index;
    }
    return -1;
}

int crdt::getSiteId() {
    return this->_siteId;
}

int crdt::getCounter() {
    return this->_counter;
}

std::vector<Symbol> crdt::getSymbols() {
    return _symbols;
}

void crdt::setSiteId(int id) {
    this->_siteId = id;
}

void crdt::setCounter(int counter) {
    this->_counter = counter;
}

void crdt::setSymbols(std::vector<Symbol> symbols) {
    this->_symbols = std::move(symbols);
}

void crdt::updateAlignmentSymbols(int index, int alignment) {
    SymbolStyle style = this->_symbols.at(index).getStyle();
    style.setAlignment(alignment);
    this->_symbols.at(index).setStyle(style);
}
