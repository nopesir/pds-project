//
// Created by giuseppe on 8/2/20.
//


#include <string>

#ifndef PDS_PROJECT_FILE_H
#define PDS_PROJECT_FILE_H


class File {
private:
    std::string _idfile;
    std::string _filename;
    std::string _owner;
    std::string _timestamp;

public:
    File(std::string idfile, std::string filename, std::string owner, std::string timestamp);
    std::string getidfile() const;
    std::string getfilename() const;
    std::string getowner() const;
    std::string gettimestamp() const;
};


#endif //PDS_PROJECT_FILE_H
