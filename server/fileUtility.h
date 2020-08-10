//
// Created by giuseppe on 8/10/20.
//

#ifndef PDS_PROJECT_FILEUTILITY_H
#define PDS_PROJECT_FILEUTILITY_H


#include <string>
#include <vector>
#include "symbol.h"

class fileUtility {

public:
    static std::vector<symbol> readFile(const std::string& filename);
    static void writeFile(const std::string& filename, const std::vector<symbol>& symbols);
    static bool is_empty(std::ifstream& pFile);
};

#endif //PDS_PROJECT_FILEUTILITY_H
