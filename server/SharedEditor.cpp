//
// Created by gheb on 26/04/20.
//

#include <iostream>
#include "SharedEditor.h"
#include "SymStyle.h"
#include "fileUtility.h"


void SharedEditor::join(const std::shared_ptr<Client>& client) {
    clients.insert(client);
    std::cout << "client #" << client->get_id() << " joined the room" << std::endl;
}

void SharedEditor::leave(const std::shared_ptr<Client>& client) {
    clients.erase(client);
    std::cout << "client leaved the room" << std::endl;
}

void SharedEditor::broadcast_deliver(const Message &msg) {
    recent_msgs.push_back(msg);
    while (recent_msgs.size() > max_recent_msgs)
        recent_msgs.pop_front();

    for (const auto& p: clients)
        p->deliver(msg);
}

void SharedEditor::deliver_to_all(const Message &msg, const int& ed_id, const std::string& curr_file, bool include_this) {
    recent_msgs.push_back(msg);
    while (recent_msgs.size() > max_recent_msgs)
        recent_msgs.pop_front();

    if(!include_this) {
        for (const auto& p: clients) {
            if (p->get_id() != ed_id && p->get_curr_file() == curr_file) //don't send the message to the same client and don't send to clients having other file opened
                p->deliver(msg);
        }
    } else {
        for (const auto& p: clients) {
            if (p->get_curr_file() == curr_file) { //don't send the message to the clients having other file opened
                p->deliver(msg);
            }
        }
    }
}

std::vector<Symbol> SharedEditor::get_file(const int& ed_id,const std::string& filename, bool get_from_disk) {
    if(file_map.empty()) //server has nothing in RAM
        return std::vector<Symbol>();
    if(file_map.at(filename).empty()) {//server has not in RAM the vector symbols for this filename
       int count =0;
        for (const auto& p: clients) {
            if (p->get_id() != ed_id && p->get_curr_file() == filename) {
                count++;
            }
            if(count!=0)
                get_from_disk=false;
        }

        return get_from_disk ? fileUtility::readFile(R"(../Filesystem/)" + filename + ".txt") : std::vector<Symbol>();
    }
    else //server has already in RAM this vector symbols
        return file_map.at(filename);
}

std::map<std::string, std::vector<Symbol>> SharedEditor::get_map() {
    return this->file_map;
}

void SharedEditor::update_file(const std::string &file, const std::vector<Symbol>& symbols) {
    this->file_map[file] = symbols; //overwrite symbols in that file(uri)
}

void SharedEditor::insert_in_file(const std::string &file, int index, const Symbol& s) {
    this->file_map[file].insert(this->file_map[file].begin() + index, s);
}

void SharedEditor::erase_from_file(const std::string &file, int index) {
    this->file_map[file].erase(this->file_map[file].begin() + index);
}

void SharedEditor::format_in_file(const std::string &file, int index, int format) {
    SymStyle style = this->file_map[file].at(index).get_style();
    if(format == Client::MAKE_BOLD)
        style.set_bold(true);
    else if(format == Client::MAKE_ITALIC)
        style.set_italic(true);
    else if(format == Client::MAKE_UNDERLINE)
        style.set_underlined(true);
    else if(format == Client::UNMAKE_BOLD)
        style.set_bold(false);
    else if(format == Client::UNMAKE_ITALIC)
        style.set_italic(false);
    else if(format == Client::UNMAKE_UNDERLINE)
        style.set_underlined(false);
    this->file_map[file].at(index).set_style(style);
}

void SharedEditor::ch_font_sz_in_file(const std::string &file, int index, int font_sz) {
    SymStyle style = this->file_map[file].at(index).get_style();
    style.set_font_sz(font_sz);
    this->file_map[file].at(index).set_style(style);
}

void SharedEditor::ch_font_fam_in_file(const std::string &file, int index, const std::string& family) {
    SymStyle style = this->file_map[file].at(index).get_style();
    style.set_font_family(family);
    this->file_map[file].at(index).set_style(style);
}

void SharedEditor::ch_alignment_in_file(const std::string &key, int index, int alignment) {
    SymStyle style = this->file_map[key].at(index).get_style();
    style.set_alignment(alignment);
    this->file_map[key].at(index).set_style(style);
}

void SharedEditor::add_file(const std::string &file, const std::vector<Symbol> &symbols) {
    this->file_map.emplace(file, symbols);
}




