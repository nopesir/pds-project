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
#include "Client.h"

class SharedEditor {
private:
    std::list<std::shared_ptr<Client>> _clients;
    /*all variables for handling messages and files*/
public:
    //SharedEditor() = default;
    void join(std::shared_ptr<Client> client);
    void leave(std::shared_ptr<Client> client);
};


#endif //PROJECT_SHAREDEDITOR_H
