//
// Created by giuseppe on 8/10/20.
//

#ifndef PDS_PROJECT_FILEUTILITY_H
#define PDS_PROJECT_FILEUTILITY_H


#include <string>
#include <vector>
#include "Symbol.h"

class fileUtility {

public:
    static void writeFile(const std::string& filename, const std::vector<Symbol>& symbols);
    static std::vector<Symbol> readFile(const std::string& filename);
    static bool is_empty(std::ifstream& pFile);


};

#endif //PDS_PROJECT_FILEUTILITY_H
