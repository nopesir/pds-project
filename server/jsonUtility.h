//
// Created by gheb on 31/07/20.
//

#ifndef PROJECT_JSONUTILITY_H
#define PROJECT_JSONUTILITY_H

#include "json.hpp"
#include "Symbol.h"

using nlohmann::json;
class jsonUtility {

public:
    static json merge(const json &a, const json &b);
    static std::vector<json> fromSymToJson(const std::vector<Symbol>& symbols);
    static std::vector<Symbol> fromJsonToSym(const std::vector<json>& jsons);
    static std::vector<Symbol> fromJsonToFormattingSym(const std::vector<json>& jsons);
    static Symbol from_json_symbol(const json& j);
    static std::vector<json> fromFileToJson(const std::vector<File>& files);
    static void to_json(json &j, const std::string &op, const std::string &resp);
    static void to_json_symbol(json &j, const Symbol &symbol);
    static void to_json_file(json &j, const File &file);
    static void to_json_newuri(json &j, const std::string &op, const std::string &resp, const std::string &uri);
    static void to_json_rename_file(json &j, const std::string &op, const std::string &resp, const std::string &filename);
    static void to_json_usernameLogin(json &j, const std::string &op, const std::string &resp, const std::string &usernameLogin, const std::string &colorLogin, const std::string &mail, const int &siteId);
    static void to_json_cursor_change(json &j, const std::string &op, const std::string &username, const std::string &color, const int &pos);
    static void to_json_collab_colors_resp(json &j, const std::string &op, const std::string &resp, const std::map<std::string, std::pair<std::string,bool>> &collabColorsMap);
    static void to_json_user_offline(json &j, const std::string &op, const std::string &resp, const std::string &username, const std::map<std::string, std::pair<std::string,bool>> &collabColorsMap);
    static void to_json_symVector(json &j, const std::string &op, const std::string &resp, const std::vector<Symbol> &symVector);
    static void to_json_symVectorAndFilename(json &j, const std::string &op, const std::string &resp, const std::vector<Symbol> &symVector, const std::string &filename);
    static void to_json_fileVector(json &j, const std::string &op, const std::string &resp, const std::vector<json> &vectorFile);
    static void to_json(json& j, const std::string& op, const std::string& user, const std::string& pass);
    static void to_json(json& j, const std::string& op, const std::string& user, const std::string& pass, const std::string& email);
    static void to_json_insertion(json &j, const std::string &op, const Symbol &symbol, const int &indexInEditor);
    static void to_json_removal_range(json &j, const std::string &op, const std::vector<std::pair<int, int>> &symbolsId);
    static void to_json_format_range(json &j, const std::string &op, const std::vector<std::pair<int, int>> &symbolsId, const int &format);
    static void to_json_fontsize_change(json &j, const std::string &op, const std::vector<std::pair<int, int>> &symbolsId, const int &fontSize);
    static void to_json_fontfamily_change(json &j, const std::string &op, const std::vector<std::pair<int, int>> &symbolsId, const std::string &fontFamily);
    static void to_json_alignment_change(json &j, const std::string &op, const std::vector<std::pair<int, int>> &symbolsId, const int &alignment);
    static void to_json_insertion_range(json &j, const std::string &op, const int &firstIndex, const std::vector<json> &symbolsJSONS);
    static void from_json_insertion(const json& j, Symbol& s, int &indexInEditor);
    static void from_json(const json& j, std::string& op);
    static void from_json_filename(const json &j, std::string &user, std::string &filename);
    static void from_json_renameFile(const json& j, std::string& nameFile, std::string& uri, std::string& username);
    static void from_json_uri(const json &j, std::string &user, std::string &uri);
    static void from_json_username(const json &j, std::string &user);
    static void from_json_resp(const json &j, std::string &resp);
    static void from_json(const json& j, std::string& user, std::string& pass);
    static void from_json(const json &j, std::string &user, std::string &pass, std::string &email);
    static void from_json_storedSymbols(const json& j, std::vector<json>& jsonSymbols);
    static void from_json_cursor_change_req(const json& j, int& index);
    static void from_json_removal_range(const json& j, std::vector<std::pair<int, int>>& symbolsId);
    static void from_json_format_range(const json &j, std::vector<std::pair<int, int>>& symbolsId, int& format);
    static void from_json_fontsize_change(const json &j, std::vector<std::pair<int, int>>& symbolsId, int& fontSize);
    static void from_json_fontfamily_change(const json &j, std::vector<std::pair<int, int>>& symbolsId, std::string& fontFamily);
    static void from_json_alignment_change(const json &j, std::vector<std::pair<int, int>>& symbolsId, int& alignment);
    static void from_json_insertion_range(const json &j, std::vector<json>& symbols, int &startIndex);
    static void from_json_inviteURI(const json& j, std::string& invited, std::string& applicant, std::string &uri);
    static void from_json_collab_colors(const json& j, std::string& uri);
};


#endif //PROJECT_JSONUTILITY_H
