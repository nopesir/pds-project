//
// Created by gheb on 27/04/20.
//

#include "Client.h"

#include <algorithm>
#include <iostream>
#include <utility>

int Client::get_id() const {
    return ed_id;
}

int Client::pos_cmp_reverse(std::vector<int> curr_sym_pos, std::pair<int,int> curr_sym_id, std::vector<int> new_sym_pos, std::pair<int,int> new_sym_id, int pos_index) {
    if (curr_sym_pos.at(pos_index) < new_sym_pos.at(pos_index))
        return 1;
    else if (curr_sym_pos.at(pos_index) == new_sym_pos.at(pos_index)) {
        if (new_sym_pos.size() > pos_index + 1 &&
            curr_sym_pos.size() <= pos_index + 1)
            return 1;
        else if (new_sym_pos.size() <= pos_index + 1 &&
                 curr_sym_pos.size() > pos_index + 1)
            return -1;
        else if (new_sym_pos.size() > pos_index + 1 &&
                 curr_sym_pos.size() > pos_index + 1)
            return pos_cmp_reverse(curr_sym_pos, curr_sym_id, new_sym_pos,
                                   new_sym_id, pos_index + 1);
        else
            return new_sym_id > curr_sym_id ? 1 : -1;
    } else
        return -1;
}

int Client::pos_cmp(std::vector<int> curr_sym_pos, std::pair<int,int> curr_sym_id, std::vector<int> new_sym_pos, std::pair<int,int> new_sym_id, int pos_index) {
    if(curr_sym_pos.at(pos_index) > new_sym_pos.at(pos_index))
        return 1;
    else if (curr_sym_pos.at(pos_index) == new_sym_pos.at(pos_index)) {
        if (new_sym_pos.size() > pos_index + 1 &&
            curr_sym_pos.size() <= pos_index + 1)
            return -1;
        else if (new_sym_pos.size() <= pos_index + 1 &&
                 curr_sym_pos.size() > pos_index + 1)
            return 1;
        else if (new_sym_pos.size() > pos_index + 1 &&
                 curr_sym_pos.size() > pos_index + 1)
            return pos_cmp(curr_sym_pos, curr_sym_id, new_sym_pos,
                           new_sym_id, pos_index + 1);
        else
            return new_sym_id.first < curr_sym_id.first ? 1 : -1;
    } else
        return -1;
}

int Client::process(int cursor_index, const std::vector<Symbol> &file_symbols, const Symbol &new_symbol) {
    int index = 0;
    int start_index = file_symbols.size();

    //get first index
    if (cursor_index > file_symbols.size() / 2) { //LOOP FROM RIGHT TO LEFT
        std::cout << std::endl << "RIGHT TO LEFT: " << start_index << std::endl << std::endl;
        for (auto s = file_symbols.crbegin(); s != file_symbols.crend(); s++) {
            start_index--;
            int ret_value = pos_cmp_reverse(s->get_pos(), s->get_id(), new_symbol.get_pos(), new_symbol.get_id(), 0);

            if (ret_value == -1)
                continue;
            else if (ret_value == 1) {
                start_index ++;
                break;
            }
        }
    }
    else { //LOOP FROM LEFT TO RIGHT
        std::cout << std::endl << "LEFT TO RIGHT: " << start_index << std::endl << std::endl;
        for (const auto &s: file_symbols) {
            index++;
            int ret_value = pos_cmp(s.get_pos(), s.get_id(), new_symbol.get_pos(), new_symbol.get_id(), 0);

            if (ret_value == -1)
                continue;
            else if (ret_value == 1) {
                start_index = index - 1;
                break;
            }
        }
    }
    return start_index;
}

int Client::get_index_by_id(const std::vector<Symbol>& file_symbols, std::pair<int, int> id) {
    auto it = std::find_if(file_symbols.begin(), file_symbols.end(), [id](const Symbol& s) {return s.get_id() == id;});
    if (it != file_symbols.end()) {
        int index = it - file_symbols.begin();
        return index;
    }
    return -1;
}

std::vector<Symbol> Client::get_symbols() {
    return file_symbols;
}

std::string Client::get_curr_file() {
    return this->curr_file;
}

std::string Client::get_username() {
    return this->username;
}

std::string Client::get_color() {
    return this->color;
}

std::string Client::to_string() {
    std::string my_string;
    for(const auto& s: file_symbols)
        my_string.push_back(s.get_char());
    return my_string;
}

void Client::set_ed_id(int edId) {
    this->ed_id = edId;
}

void Client::set_symbols(std::vector<Symbol> symbols) {
    this->file_symbols = std::move(symbols);
}

void Client::set_curr_file(std::string uri) {
    this->curr_file = std::move(uri);
}

void Client::set_username(std::string userName) {
    this->username = std::move(userName);
}

void Client::set_color(std::string color) {
    this->color = std::move(color);
}