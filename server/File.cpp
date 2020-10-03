//
// Created by giuseppe on 8/2/20.
//

#include "File.h"



#include <utility>

File::File(std::string idfile, std::string filename, std::string owner, std::string timestamp)
        : idfile(std::move(idfile)), filename(std::move(filename)), owner(std::move(owner)), timestamp(std::move(timestamp)) {}

std::string File::getIdFile() const {
    return idfile;
}

std::string File::getFileName() const {
    return filename;
}

std::string File::getOwner() const {
    return owner;
}

std::string File::getTimeStamp() const {
    return timestamp;
}
