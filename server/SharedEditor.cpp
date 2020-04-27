//
// Created by gheb on 26/04/20.
//

#include "SharedEditor.h"

void SharedEditor::join(std::shared_ptr<Client> client) {
    auto it =_clients.begin();
    for(int i = 0; i< client->getId(); i++)
        it++;
    _clients.insert(it, client);
}

void SharedEditor::leave(std::shared_ptr<Client> client) {
    auto it =_clients.begin();
    for(int i = 0; i< client->getId(); i++)
        it++;
    _clients.erase(it);
}
