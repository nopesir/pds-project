//
// Created by giuseppe on 8/2/20.
//

#ifndef PDS_PROJECT_DBSERVICE_H
#define PDS_PROJECT_DBSERVICE_H


#include <QtCore/QString>
#include "sqlite3.h"
#include <vector>
#include <map>
#include "File.h"

class dbService {



public:
    enum DB_RESPONSE {LOGIN_OK, LOGIN_FAILED, SIGNUP_OK, SIGNUP_FAILED, NEWFILE_OK, NEWFILE_FAILED, OPENFILE_OK, OPENFILE_FAILED,
        DB_ERROR, QUERY_ERROR, EMAIL_ERROR, ALREADY_LOGGED, LIST_EXIST, LIST_DOESNT_EXIST, OPENWITHURI_OK,
        OPENWITHURI_FAILED, LOGOUT_OK, LOGOUT_FAILED, RENAME_OK, RENAME_FAILED, ALREADY_PARTECIPANT, INVITE_URI_SUCCESS,
        INVITE_URI_FAILED, INVITED_NOT_EXIST, GET_EMAIL_OK, GET_COLLAB_COLORS_MAP_OK

    };
    static inline const char* enumToStr(dbService::DB_RESPONSE db_resp);
    static QString generateColor();
    static QString generateURI(int len);
    static QString getTimestamp();
    static DB_RESPONSE logout(const std::string &user, const std::string &urifile);
    static DB_RESPONSE logout(const std::string &user);
    static DB_RESPONSE login(const std::string &user, const std::string &pass, QString &color, QString &mail);
    static DB_RESPONSE signup(const std::string &user, const std::string &pass, const std::string &email);
    static DB_RESPONSE newFile(const std::string &user, const std::string &file_name, const QString &uri);
    static DB_RESPONSE listFile(const std::string &user, std::vector<File> &vectorFile);
    static DB_RESPONSE renameFile(const std::string &newNameFile, const std::string &urifile, const std::string &user);
    static DB_RESPONSE openFile(const std::string &user, const std::string &urifile);
    static DB_RESPONSE openURIFile(const std::string &user, const std::string &urifile, std::string &filename);
    static DB_RESPONSE addFriend(const std::string &invited, const std::string &urifile);
    static DB_RESPONSE getEmail(const std::string &invited, std::string &email_invited);
    static DB_RESPONSE getColors(const std::string &uri,
                                 std::map<std::string, std::pair<std::string, bool>> &collabColorsMap);
    static DB_RESPONSE cleanAll();
};
#endif //PDS_PROJECT_DBSERVICE_H
