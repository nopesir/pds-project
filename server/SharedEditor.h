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
    std::set<std::shared_ptr<Client>> _clients;
    enum { max_recent_msgs = 100 };
    std::deque<Message> recent_msgs_;
    std::map<std::string, std::vector<Symbol>> se_map;
    /*all variables for handling messages and files*/
public:
    static SharedEditor& getInstance() {
        static SharedEditor instance;
        return instance;
    }
    SharedEditor(SharedEditor const&) = delete;
    void operator=(SharedEditor const&) = delete;
    void join(const std::shared_ptr<Client>& participant);
    void leave(const std::shared_ptr<Client>& participant);
    void deliver(const Message& msg); //deliver to the clients
    void deliverToAll(const Message& msg, const int& edId, const std::string& curFile, bool includeThisEditor=false); //deliver to the clients except the client with id 'edId'
    std::map<std::string, std::vector<Symbol>> getMap();
    void updateMap(const std::string& key, const std::vector<Symbol>& symbols);
    void addEntryInMap(const std::string& key, const std::vector<Symbol>& symbols);
    void insertInSymbolMap(const std::string &key, int index, const Symbol& s);
    void eraseInSymbolMap(const std::string &key, int index);
    void formatInSymbolMap(const std::string &key, int index, int format);
    void changeFontSizeInSymbolMap(const std::string &key, int index, int fontSize);
    void changeFontFamilyInSymbolMap(const std::string &key, int index, const std::string& fontFamily);
    void changeAlignmentInSymbolMap(const std::string &key, int index, int alignment);
    void updateSymbolsMap(const std::string &key, int index, const std::vector<Symbol>& symbols);
    void setMap(const std::map<std::string, std::vector<Symbol>>& m);
    std::vector<Symbol> getSymbolMap(const std::string& filename, bool canReadFromFile);
};


#endif //PROJECT_SHAREDEDITOR_H
