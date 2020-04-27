//
// Created by gheb on 27/04/20.
//

#ifndef PROJECT_CLIENT_H
#define PROJECT_CLIENT_H

#include <string>

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
    explicit Client(int id): _user_id(id){};

    std::string getFile(){return _current_file;}
    void setFile(const std::string& name){_current_file = name;}
    int getId() {return _user_id;}
//    void setID(int id){_user_id = id;}

};


#endif //PROJECT_CLIENT_H
