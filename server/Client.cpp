//
// Created by gheb on 27/04/20.
//

#include "Client.h"

#include <algorithm>
#include <iostream>
#include <utility>

int Client::getId() const {
    return _siteId;
}

int Client::comparePosdx(std::vector<int> curSymPos, std::pair<int,int> curSymId, std::vector<int> newSymPos, std::pair<int,int> newSymId, int posIndex) {
    if (curSymPos.at(posIndex) < newSymPos.at(posIndex))
        return 1;
    else if (curSymPos.at(posIndex) == newSymPos.at(posIndex)) {
        if (newSymPos.size() > posIndex + 1 &&
            curSymPos.size() <= posIndex + 1) //newSymPos[posIndex+1] != null && curSymPos[posIndex+1] == null
            return 1; // correct position found
        else if (newSymPos.size() <= posIndex + 1 &&
                 curSymPos.size() > posIndex + 1) //newSymPos[posIndex+1] == null && curSymPos[posIndex+1] != null
            return -1; //curSymPos > newSymPos  -> make another cycle taking the next symbol from _symbols
        else if (newSymPos.size() > posIndex + 1 &&
                 curSymPos.size() > posIndex + 1) //newSymPos[posIndex+1] != null && curSymPos[posIndex+1] != null
            return comparePosdx(curSymPos, curSymId, newSymPos,
                                newSymId, posIndex + 1); //call recursively this function using next index for posIndex
        else //newSymPos[posIndex+1] == null && curSymPos[posIndex+1] == null
            return newSymId > curSymId ? 1 : -1;
    } else
        return -1; //make another cycle taking the next symbol from _symbols
}

int Client::comparePos(std::vector<int> curSymPos, std::pair<int,int> curSymId, std::vector<int> newSymPos, std::pair<int,int> newSymId, int posIndex) {
    if(curSymPos.at(posIndex) > newSymPos.at(posIndex))
        return 1; //correct position found
    else if (curSymPos.at(posIndex) == newSymPos.at(posIndex)) {
        if (newSymPos.size() > posIndex + 1 &&
            curSymPos.size() <= posIndex + 1) //newSymPos[posIndex+1] != null && curSymPos[posIndex+1] == null
            return -1; //newSymPos > curSymPos -> make another cycle taking the next symbol from _symbols
        else if (newSymPos.size() <= posIndex + 1 &&
                 curSymPos.size() > posIndex + 1) //newSymPos[posIndex+1] == null && curSymPos[posIndex+1] != null
            return 1; //correct position found
        else if (newSymPos.size() > posIndex + 1 &&
                 curSymPos.size() > posIndex + 1) //newSymPos[posIndex+1] != null && curSymPos[posIndex+1] != null
            return comparePos(curSymPos, curSymId, newSymPos,
                              newSymId,posIndex + 1); //call recursively this function using next index for posIndex
        else //newSymPos[posIndex+1] == null && curSymPos[posIndex+1] == null
            return newSymId.first < curSymId.first ? 1 : -1;
    } else
        return -1; //make another cycle taking the next symbol from _symbols
}

int Client::process(int type, int indexEditor, const std::vector<Symbol>& roomSymbols, const Symbol& newSymbol) {
    /* Insertion */
    if (type == 0) {
        int symbols_index = 0, pos_index = 0;
        int startIndex = roomSymbols.size();

        //get first index
        if (indexEditor > roomSymbols.size()/2) { //LOOP FROM RIGHT TO LEFT
            std::cout << std::endl << "RIGHT TO LEFT: " << startIndex << std::endl << std::endl;
            for (auto s = roomSymbols.crbegin(); s != roomSymbols.crend(); s++) {
                startIndex--;
                int retValue = comparePosdx(s->getPos(), s->getId(), newSymbol.getPos(), newSymbol.getId(), pos_index);

                if (retValue == -1)
                    continue;
                else if (retValue == 1) {
                    startIndex ++;
                    break;
                }
            }
        }
        else { //LOOP FROM LEFT TO RIGHT
            std::cout << std::endl << "LEFT TO RIGHT: " << startIndex << std::endl << std::endl;
            for (const auto &s: roomSymbols) {
                symbols_index++;
                int retValue = comparePos(s.getPos(), s.getId(), newSymbol.getPos(), newSymbol.getId(), pos_index);

                if (retValue == -1)
                    continue;
                else if (retValue == 1) {
                    startIndex = symbols_index - 1;
                    break;
                }
            }
        }
        return startIndex;
    }
}

int Client::process(int type, int indexEditor, const std::vector<Symbol>& roomSymbols, const std::vector<Symbol>& newSymbols) {
    /* Insertion range */
    if(type == 6) {
        int symbols_index = 0, pos_index = 0;
        int startIndex = roomSymbols.size();

        //get first index
        if(indexEditor > roomSymbols.size()/2) {  //LOOP FROM RIGHT TO LEFT
            for (auto s = roomSymbols.crbegin(); s != roomSymbols.crend(); s++) {
                startIndex--;
                int retValue = comparePosdx(s->getPos(), s->getId(), newSymbols.at(0).getPos(), newSymbols.at(0).getId(), pos_index);
                if (retValue == -1)
                    continue;
                else if (retValue == 1) {
                    startIndex ++;
                    break;
                }
            }
        }
        else { //LOOP FROM LEFT TO RIGHT
            for (const auto &s: roomSymbols) {
                symbols_index++;
                int retValue = comparePos(s.getPos(), s.getId(), newSymbols.at(0).getPos(), newSymbols.at(0).getId(), pos_index);
                if (retValue == -1)
                    continue;
                else if (retValue == 1) {
                    startIndex = symbols_index - 1;
                    break;
                }
            }
        }
        return startIndex;
    }
}

int Client::getIndexById(const std::vector<Symbol>& roomSymbols, std::pair<int, int> id) {
    auto it = std::find_if(roomSymbols.begin(), roomSymbols.end(), [id](const Symbol& s) {return s.getId() == id;});
    if (it != roomSymbols.end()) {
        int index = it - roomSymbols.begin();
        return index;
    }
    return -1;
}

std::vector<Symbol> Client::getSymbols() {
    return _symbols;
}

std::string Client::getCurrentFile() {
    return this->currentFile;
}

std::string Client::getUsername() {
    return this->username;
}

std::string Client::getColor() {
    return this->color;
}

std::string Client::to_string() {
    std::string my_string;
    for(const auto& s: _symbols)
        my_string.push_back(s.getLetter());
    return my_string;
}

void Client::setSiteId(int edId) {
    this->_siteId = edId;
}

void Client::setSymbols(std::vector<Symbol> symbols) {
    this->_symbols = std::move(symbols);
}

void Client::setCurrentFile(std::string uri) {
    this->currentFile = std::move(uri);
}

void Client::setUsername(std::string userName) {
    this->username = std::move(userName);
}

void Client::setColor(std::string color) {
    this->color = std::move(color);
}