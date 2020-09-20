//
// Created by gheb on 27/04/20.
//

#ifndef PROJECT_CLIENT_H
#define PROJECT_CLIENT_H

#include <string>
#include <vector>
#include "Symbol.h"
#include "Message.h"

/*
 *
 * This class will handle the local changes of a client
 * It will be Instantiated as Session (which extends client) in order to avoid mutual inclusions
 * between Session and SharedEditor
 *
 *
 * */
class Client {
private:
    int ed_id;
    std::string curr_file;
    std::string username;
    std::string color;
    std::vector<Symbol> file_symbols;
    int pos_cmp_reverse(std::vector<int> curr_sym_pos, std::pair<int,int> curr_sym_id, std::vector<int> new_sym_pos, std::pair<int,int> new_sym_id, int pos_index);
    int pos_cmp(std::vector<int> curr_sym_pos, std::pair<int,int> curr_sym_id, std::vector<int> new_sym_pos, std::pair<int,int> new_sym_id, int pos_index);
public:
    enum formatType {MAKE_BOLD=0, MAKE_ITALIC=1, MAKE_UNDERLINE=2, UNMAKE_BOLD=3, UNMAKE_ITALIC=4, UNMAKE_UNDERLINE=5, FORMAT_UNKNOWN=6};
    virtual ~Client() = default;
    virtual void deliver(const Message& msg) = 0;
    int get_id() const;
    int process(int cursor_index, const std::vector<Symbol> &file_symbols, const Symbol &new_symbol);
    int get_index_by_id(const std::vector<Symbol>& file_symbols, std::pair<int, int> id);
    std::string to_string();
    std::vector<Symbol> get_symbols();
    void set_symbols(std::vector<Symbol> symbols);
    void set_curr_file(std::string uri);
    void set_username(std::string userName);
    void set_color(std::string color);
    void set_ed_id(int edId);
    std::string get_curr_file();
    std::string get_username();
    std::string get_color();

};


#endif //PROJECT_CLIENT_H
