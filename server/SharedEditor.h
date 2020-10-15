//
// Created by gheb on 26/04/20.
//

#ifndef PROJECT_SHAREDEDITOR_H
#define PROJECT_SHAREDEDITOR_H

/*
 *
 * This class will handle the algorithm of the shared editor:
 *      +handling the users through the sessions
 *      +handling the messages
 *      +handling the files
 */

#include <list>
#include <memory>
#include <map>
#include <deque>
#include <set>
#include "Client.h"
#include "Message.h"
#include "Symbol.h"

/*room*/
class SharedEditor {
private:
    SharedEditor(){}
    std::set<std::shared_ptr<Client>> clients;
    enum { max_recent_msgs = 100 };
    std::deque<Message> recent_msgs;
    std::map<std::string, std::vector<Symbol>> file_map;
public:
    static SharedEditor& getInstance() {
        static SharedEditor instance;
        return instance;
    }
    /*Singleton behaviour*/
    SharedEditor(SharedEditor const&) = delete;
    void operator=(SharedEditor const&) = delete;

    void join(const std::shared_ptr<Client>& client);
    void leave(const std::shared_ptr<Client>& client);
    void broadcast_deliver(const Message& msg); //deliver to ALL the clients (editor functions)
    void deliver_to_all(const Message& msg, const int& ed_id, const std::string& curr_file, bool include_this= false); //deliver to the clients except the client with id 'ed_id'
    std::map<std::string, std::vector<Symbol>> get_map();
    void update_file(const std::string& file, const std::vector<Symbol>& symbols);
    void add_file(const std::string& file, const std::vector<Symbol>& symbols);
    void insert_in_file(const std::string &file, int index, const Symbol& s);
    void erase_from_file(const std::string &file, int index);
    void format_in_file(const std::string &file, int index, int format);
    void ch_font_sz_in_file(const std::string &file, int index, int font_sz);
    void ch_font_fam_in_file(const std::string &file, int index, const std::string& family);
    void ch_alignment_in_file(const std::string &key, int index, int alignment);
    std::vector<Symbol> get_file(const int& ed_id,const std::string& filename, bool get_from_disk);
};


#endif //PROJECT_SHAREDEDITOR_H
