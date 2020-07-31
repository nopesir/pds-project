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
    int _siteId;
    std::string currentFile;
    std::string username;
    std::string color;
    std::vector<Symbol> _symbols;
    int comparePosdx(std::vector<int> curSymPos, std::pair<int,int> curSymId, std::vector<int> newSymPos, std::pair<int,int> newSymId, int posIndex);
    int comparePos(std::vector<int> curSymPos, std::pair<int,int> curSymId, std::vector<int> newSymPos, std::pair<int,int> newSymId, int posIndex);
public:
    enum formatType {MAKE_BOLD=0, MAKE_ITALIC=1, MAKE_UNDERLINE=2, UNMAKE_BOLD=3, UNMAKE_ITALIC=4, UNMAKE_UNDERLINE=5, FORMAT_UNKNOWN=6};
    virtual ~Client() = default;
    virtual void deliver(const Message& msg) = 0;
    int getId() const;
    int process(int type, int indexEditor, const std::vector<Symbol>& roomSymbols, const Symbol& newSymbol);
    int process(int type, int indexEditor, const std::vector<Symbol>& roomSymbols, const std::vector<Symbol>& newSymbols);
    int getIndexById(const std::vector<Symbol>& roomSymbols, std::pair<int, int> id);
    std::string to_string();
    std::vector<Symbol> getSymbols();
    void setSymbols(std::vector<Symbol> symbols);
    void setCurrentFile(std::string uri);
    void setUsername(std::string userName);
    void setColor(std::string color);
    void setSiteId(int edId);
    std::string getCurrentFile();
    std::string getUsername();
    std::string getColor();

};


#endif //PROJECT_CLIENT_H
