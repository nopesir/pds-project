//
// Created by gheb on 26/04/20.
//

#include <iostream>
#include "SharedEditor.h"
#include "SymbolStyle.h"
#include "fileUtility.h"


void SharedEditor::join(const std::shared_ptr<Client>& participant) {
    _clients.insert(participant);
    std::cout << "participant #" << participant->getId() << " joined the room" << std::endl;
}

void SharedEditor::leave(const std::shared_ptr<Client>& participant) {
    _clients.erase(participant);
    std::cout << "participant leaved the room" << std::endl;
}

void SharedEditor::deliver(const Message &msg) {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
        recent_msgs_.pop_front();

    for (const auto& p: _clients)
        p->deliver(msg);
}

void SharedEditor::deliverToAll(const Message &msg, const int& edId, const std::string& curFile, bool includeThisEditor) {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
        recent_msgs_.pop_front();

    if(!includeThisEditor) {
        for (const auto& p: _clients) {
            if (p->getId() != edId && p->getCurrentFile() == curFile) //don't send the message to the same client and don't send to clients having other file opened
                p->deliver(msg);
        }
    } else {
        for (const auto& p: _clients) {
            if (p->getCurrentFile() == curFile) { //don't send the message to the clients having other file opened
                p->deliver(msg);
            }
        }
    }
}

std::vector<Symbol> SharedEditor::getSymbolMap(const std::string& filename, bool canReadFromFile) {
    if(se_map.empty()) //server has nothing in RAM
        return std::vector<Symbol>();
    if(se_map.at(filename).empty()) {//server has not in RAM the vector symbols for this filename
        return canReadFromFile ? fileUtility::readFile(R"(..\Filesystem\)" + filename + ".txt") : std::vector<Symbol>();
    }
    else //server has already in RAM this vector symbols
        return se_map.at(filename);
}

std::map<std::string, std::vector<Symbol>> SharedEditor::getMap() {
    return this->se_map;
}

void SharedEditor::updateMap(const std::string &key, const std::vector<Symbol>& symbols) {
    this->se_map[key] = symbols; //overwrite symbols in that key(uri)
}

void SharedEditor::insertInSymbolMap(const std::string &key, int index, const Symbol& s) {
    this->se_map[key].insert(this->se_map[key].begin() + index, s);
}

void SharedEditor::eraseInSymbolMap(const std::string &key, int index) {
    this->se_map[key].erase(this->se_map[key].begin() + index);
}

void SharedEditor::formatInSymbolMap(const std::string &key, int index, int format) {
    SymbolStyle style = this->se_map[key].at(index).getStyle();
    if(format == Client::MAKE_BOLD)
        style.setBold(true);
    else if(format == Client::MAKE_ITALIC)
        style.setItalic(true);
    else if(format == Client::MAKE_UNDERLINE)
        style.setUnderlined(true);
    else if(format == Client::UNMAKE_BOLD)
        style.setBold(false);
    else if(format == Client::UNMAKE_ITALIC)
        style.setItalic(false);
    else if(format == Client::UNMAKE_UNDERLINE)
        style.setUnderlined(false);
    this->se_map[key].at(index).setStyle(style);
}

void SharedEditor::changeFontSizeInSymbolMap(const std::string &key, int index, int fontSize) {
    SymbolStyle style = this->se_map[key].at(index).getStyle();
    style.setFontSize(fontSize);
    this->se_map[key].at(index).setStyle(style);
}

void SharedEditor::changeFontFamilyInSymbolMap(const std::string &key, int index, const std::string& fontFamily) {
    SymbolStyle style = this->se_map[key].at(index).getStyle();
    style.setFontFamily(fontFamily);
    this->se_map[key].at(index).setStyle(style);
}

void SharedEditor::changeAlignmentInSymbolMap(const std::string &key, int index, int alignment) {
    SymbolStyle style = this->se_map[key].at(index).getStyle();
    style.setAlignment(alignment);
    this->se_map[key].at(index).setStyle(style);
}

void SharedEditor::updateSymbolsMap(const std::string &key, int index, const std::vector<Symbol>& symbols) {
    this->se_map[key].insert(this->se_map[key].begin() + index, symbols.begin(), symbols.end());
}

void SharedEditor::setMap(const std::map<std::string, std::vector<Symbol>>& m) {
    this->se_map = m;
}

void SharedEditor::addEntryInMap(const std::string &key, const std::vector<Symbol> &symbols) {
    this->se_map.emplace(key, symbols);
}




