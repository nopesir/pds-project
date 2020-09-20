//
// Created by gheb on 31/07/20.
//

#ifndef PROJECT_JSONUTILITY_H
#define PROJECT_JSONUTILITY_H

#include "json.hpp"
#include "Symbol.h"
#include "File.h"

using nlohmann::json;
class jsonUtility {

public:
    //static json merge(const json &a, const json &b);
    static void from_json_stored_to_syms(const json& j, std::vector<json>& json_symbols); //json -> std::vector<json>
    static std::vector<Symbol> from_jsons_to_syms(const std::vector<json>& jsons); // std::vector<json> -> std::vector<Symbol>
    static std::vector<json> from_syms_to_jsons(const std::vector<Symbol>& symbols); // std::vector<Symbol> -> std::vector<json>

    static void to_json_symbol(json &j, const Symbol &symbol); //symbol -> json
    static Symbol from_json_symbol(const json& j); // json -> symbol

    static void to_json_symbols_vector_response(json &j, const std::string &op, const std::string &resp, const std::vector<Symbol> &symVector); //openfile response with symbol vector

    static std::vector<json> from_files_to_json(const std::vector<File>& files); //std::vector<File> -> std::vector<json>
    static void to_json_file(json &j, const File &file); // File -> json
    static void to_json_j_file_vector(json &j, const std::string &op, const std::string &resp, const std::vector<json> &j_file_vector); //listfile response with filevector (jsons)

    static void from_json_user_filename(const json &j, std::string &user, std::string &filename); //json -> 2 string: user and filename

    static void to_json_simple_resp(json &j, const std::string &op, const std::string &resp);
    static void from_json_simple_op(const json& j, std::string& op);

    static void to_json_newuri(json &j, const std::string &op, const std::string &resp, const std::string &uri); //new file response
    static void from_json_uri(const json &j, std::string &user, std::string &uri); //json -> 2 string: user and uri
    static void from_json_inviteURI(const json& j, std::string& invited, std::string& applicant, std::string &uri); //json -> 3 string

    static void from_json(const json& j, std::string& user, std::string& pass); //for login req
    static void to_json_username_login(json &j, const std::string &op, const std::string &resp, const std::string &username_login, const std::string &color_login, const std::string &mail, const int &ed_id); //login resp

    static void from_json(const json &j, std::string &user, std::string &pass, std::string &email); //for signup req

    static void to_json_rename_file(json &j, const std::string &op, const std::string &resp, const std::string &filename); //rename resp
    static void from_json_rename_file(const json& j, std::string& file_name, std::string& uri, std::string& username); //rename req

    static void from_json_username(const json &j, std::string &user); //json -> string : username

    static void to_json_cursor_change_resp(json &j, const std::string &op, const std::string &username, const std::string &color, const int &pos);
    static void from_json_cursor_change_req(const json& j, int& index);

    static void to_json_collab_colors_resp(json &j, const std::string &op, const std::string &resp, const std::map<std::string, std::pair<std::string,bool>> &collab_colors_map);
    static void from_json_collab_colors_req(const json& j, std::string& uri);

    static void to_json_user_on_off(json &j, const std::string &op, const std::string &resp, const std::string &username, const std::map<std::string, std::pair<std::string,bool>> &collab_colors_map); //get user online/offline resp

    static void to_json_open_w_uri_resp(json &j, const std::string &op, const std::string &resp, const std::vector<Symbol> &sym_vector, const std::string &filename);

    static void to_json_insertion_resp(json &j, const std::string &op, const Symbol &symbol, const int &indexInEditor);
    static void from_json_insertion_req(const json& j, Symbol& s, int &indexInEditor);

    static void to_json_removal_range_resp(json &j, const std::string &op, const std::vector<std::pair<int, int>> &symbolsId);
    static void from_json_removal_range_req(const json& j, std::vector<std::pair<int, int>>& symbolsId);

    static void to_json_format_range_res(json &j, const std::string &op, const std::vector<std::pair<int, int>> &symbolsId, const int &format);
    static void from_json_format_range_req(const json &j, std::vector<std::pair<int, int>>& symbolsId, int& format);

    static void to_json_fontsize_change_resp(json &j, const std::string &op, const std::vector<std::pair<int, int>> &symbolsId, const int &fontSize);
    static void from_json_fontsize_change_req(const json &j, std::vector<std::pair<int, int>>& symbolsId, int& fontSize);

    static void to_json_font_family_ch_resp(json &j, const std::string &op, const std::vector<std::pair<int, int>> &symbolsId, const std::string &fontFamily);
    static void from_json_font_family_ch_req(const json &j, std::vector<std::pair<int, int>>& symbolsId, std::string& fontFamily);

    static void to_json_alignment_ch_resp(json &j, const std::string &op, const std::vector<std::pair<int, int>> &symbolsId, const int &alignment);
    static void from_json_alignment_ch_req(const json &j, std::vector<std::pair<int, int>>& symbolsId, int& alignment);

    static void to_json_insertion_range_resp(json &j, const std::string &op, const int &firstIndex, const std::vector<json> &symbolsJSONS);
    static void from_json_insertion_range_req(const json &j, std::vector<json>& symbols, int &startIndex);
};


#endif //PROJECT_JSONUTILITY_H
