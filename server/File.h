//
// Created by giuseppe on 8/2/20.
//


#include <string>

#ifndef PDS_PROJECT_FILE_H
#define PDS_PROJECT_FILE_H


class File {
private:
    std::string idfile;
    std::string filename;
    std::string owner;
    std::string timestamp;

public:
    File(std::string idfile, std::string filename, std::string owner, std::string timestamp);
    std::string getIdFile() const;
    std::string getFileName() const;
    std::string getOwner() const;
    std::string getTimeStamp() const;
};


#endif //PDS_PROJECT_FILE_H
