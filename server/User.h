//
// Created by gheb on 24/04/20.
//

#ifndef PROJECT_USER_H
#define PROJECT_USER_H


#include <string>
#include <utility>

class User {
private:
    int _user_id;
    std::string _user_name;
    std::string _current_file;
    std::string _color;
    /* std::vector<symbol> _symbols;
    int _counter = 0;
    std::vector<int> generatePos(int index);
    std::vector<int> generatePosBetween(std::vector<int> pos1, std::vector<int> pos2, std::vector<int> newPos = {});
    int comparePosdx(std::vector<int> curSymPos, std::vector<int> newSymPos, int posIndex);
    int comparePos(std::vector<int> curSymPos, std::vector<int> newSymPos, int posIndex);
    enum formatType {MAKE_BOLD=0, MAKE_ITALIC=1, MAKE_UNDERLINE=2, UNMAKE_BOLD=3, UNMAKE_ITALIC=4, UNMAKE_UNDERLINE=5, FORMAT_UNKNOWN=6
     */
public:
    User(const int id,std::string  name): _user_id(id), _user_name(std::move(name)){}
    std::string getFile(){return _current_file;}
    void setFile(const std::string& name){_current_file = name;}
    //int getId() {return _user_id;}

};


#endif //PROJECT_USER_H
