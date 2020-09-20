//
// Created by gheb on 31/07/20.
//

#include "jsonUtility.h"
#include "SymStyle.h"

void jsonUtility::to_json_simple_resp(json &j, const std::string &op, const std::string &resp) {
    j = json{
            {"operation", op},
            {"content", {
                                  {"response", resp}
                          }}
    };
}

void jsonUtility::to_json_symbol(json &j, const Symbol &symbol) {
    j = json{
            {"letter", symbol.get_char()},
            {"id", symbol.get_id()}, //std::pair
            {"pos", symbol.get_pos()}, //std::vector<int>
            {"isBold", symbol.get_style().isBold()},
            {"isItalic", symbol.get_style().isItalic()},
            {"isUnderlined", symbol.get_style().isUnderlined()},
            {"fontFamily", symbol.get_style().get_font_family()},
            {"fontSize", symbol.get_style().get_font_sz()},
            {"alignment", symbol.get_style().get_alignment()},
            {"color", symbol.get_style().getColor()}
    };
}

/* We need to use this 'to_json' to serialize std::vector<symbol> (see function to_json_symbols_vector_response) */
void to_json(json& j, const Symbol& s) {
    j = json{
            {"letter", s.get_char()},
            {"id", s.get_id()}, //std::pair
            {"pos", s.get_pos()}, //std::vector<int>
            {"isBold", s.get_style().isBold()},
            {"isItalic", s.get_style().isItalic()},
            {"isUnderlined", s.get_style().isUnderlined()},
            {"fontFamily", s.get_style().get_font_family()},
            {"fontSize", s.get_style().get_font_sz()},
            {"alignment", s.get_style().get_alignment()},
            {"color", s.get_style().getColor()}
    };
}

void jsonUtility::to_json_symbols_vector_response(json &j, const std::string &op, const std::string &resp, const std::vector<Symbol> &symVector) {
    j = json{
            {"operation", op},
            {"content", {
                                  {"response", resp},
                                  {"symVector", symVector} //use to_json previously defined
                          }}
    };
}

void jsonUtility::to_json_cursor_change_resp(json &j, const std::string &op, const std::string &username, const std::string &color, const int &pos) {
    j = json{
            {"operation", op},
            {"username", username},
            {"color", color},
            {"pos", pos}
    };
}

void jsonUtility::to_json_collab_colors_resp(json &j, const std::string &op, const std::string &resp, const std::map<std::string, std::pair<std::string,bool>> &collab_colors_map) {
    j = json{
            {"operation",       op},
            {"response",        resp},
            {"collabColorsMap", collab_colors_map}
    };
}

void jsonUtility::to_json_user_on_off(json &j, const std::string &op, const std::string &resp, const std::string &username, const std::map<std::string, std::pair<std::string,bool>> &collab_colors_map) {
    j = json{
            {"operation",       op},
            {"response",        resp},
            {"username",        username},
            {"collabColorsMap", collab_colors_map}
    };
}

void jsonUtility::to_json_open_w_uri_resp(json &j, const std::string &op, const std::string &resp, const std::vector<Symbol> &sym_vector, const std::string &filename) {
    j = json{
            {"operation", op},
            {"content", {
                                  {"response", resp},
                                  {"filename", filename},
                                  {"symVector", sym_vector}
                          }}
    };
}

void jsonUtility::to_json_file(json &j, const File &file) {
    j = json{
            {"idfile", file.getIdFile()},
            {"filename", file.getFileName()},
            {"owner", file.getOwner()},
            {"timestamp", file.getTimeStamp()}
    };
}

void jsonUtility::to_json_j_file_vector(json &j, const std::string &op, const std::string &resp, const std::vector<json> &j_file_vector) {
    j = json{
            {"operation", op},
            {"content", {
                                  {"response", resp},
                                  {"vectorFile", j_file_vector} //JSON vector
                          }}
    };
}

void jsonUtility::to_json_username_login(json &j, const std::string &op, const std::string &resp, const std::string &username_login, const std::string &color_login, const std::string &mail, const int &ed_id) {
    j = json{
            {"operation", op},
            {"content", {
                                  {"response", resp},
                                  {"usernameLogin", username_login},
                                  {"colorLogin", color_login},
                                  {"mailLogin", mail},
                                  {"siteId", ed_id}
                          }}
    };
}

void jsonUtility::to_json_newuri(json &j, const std::string &op, const std::string &resp, const std::string &uri) {
    j = json{
            {"operation", op},
            {"content", {
                                  {"response", resp},
                                  {"uri", uri}
                          }}
    };
}

void jsonUtility::to_json_rename_file(json &j, const std::string &op, const std::string &resp, const std::string &filename) {
    j = json{
            {"operation", op},
            {"content", {
                                  {"response", resp},
                                  {"filename", filename}
                          }}
    };
}

void jsonUtility::to_json_insertion_resp(json &j, const std::string &op, const Symbol &symbol, const int &indexInEditor) {
    j = json{
            {"operation", op},
            {"id", symbol.get_id()},
            {"pos", symbol.get_pos()},
            {"letter", symbol.get_char()},
            {"isBold", symbol.get_style().isBold()},
            {"isItalic", symbol.get_style().isItalic()},
            {"isUnderlined", symbol.get_style().isUnderlined()},
            {"fontFamily", symbol.get_style().get_font_family()},
            {"fontSize", symbol.get_style().get_font_sz()},
            {"alignment", symbol.get_style().get_alignment()},
            {"color", symbol.get_style().getColor()},
            {"indexInEditor", indexInEditor}
    };
}

void jsonUtility::to_json_removal_range_resp(json &j, const std::string &op, const std::vector<std::pair<int,int>> &symbolsId) {
    j = json{
            {"operation", op},
            {"symbolsId", symbolsId}
    };
}

void jsonUtility::to_json_format_range_res(json &j, const std::string &op, const std::vector<std::pair<int,int>> &symbolsId, const int &format) {
    j = json{
            {"operation", op},
            {"symbolsId", symbolsId},
            {"format", format}
    };
}

void jsonUtility::to_json_fontsize_change_resp(json &j, const std::string &op, const std::vector<std::pair<int,int>> &symbolsId, const int &fontSize) {
    j = json{
            {"operation", op},
            {"symbolsId", symbolsId},
            {"fontSize", fontSize}
    };
}

void jsonUtility::to_json_font_family_ch_resp(json &j, const std::string &op, const std::vector<std::pair<int,int>> &symbolsId, const std::string &fontFamily) {
    j = json{
            {"operation", op},
            {"symbolsId", symbolsId},
            {"fontFamily", fontFamily}
    };
}

void jsonUtility::to_json_alignment_ch_resp(json &j, const std::string &op, const std::vector<std::pair<int,int>> &symbolsId, const int &alignment) {
    j = json{
            {"operation", op},
            {"symbolsId", symbolsId},
            {"alignment", alignment}
    };
}

void jsonUtility::to_json_insertion_range_resp(json &j, const std::string &op, const int &firstIndex, const std::vector<json> &symbolsJSONS) {
    j = json{
            {"operation",       op},
            {"firstIndexRange", firstIndex},
            {"symbols",         symbolsJSONS}
    };
}

void jsonUtility::from_json_insertion_req(const json& j, Symbol& s, int &indexInEditor) {
    indexInEditor = j.at("indexInEditor").get<int>();
    wchar_t letter = j.at("letter").get<wchar_t>();
    std::pair<int,int> id = j.at("id").get<std::pair<int,int>>();
    std::vector<int> pos = j.at("pos").get<std::vector<int>>();
    SymStyle style;
    style.set_bold(j.at("isBold").get<bool>());
    style.set_italic(j.at("isItalic").get<bool>());
    style.set_underlined(j.at("isUnderlined").get<bool>());
    style.set_font_family(j.at("fontFamily").get<std::string>());
    style.set_font_sz(j.at("fontSize").get<int>());
    style.set_alignment(j.at("alignment").get<int>());
    style.set_color(j.at("color").get<std::string>());
    s = Symbol(letter, id, pos, style);
}

void jsonUtility::from_json_simple_op(const json &j, std::string &op) {
    op = j.at("operation").get<std::string>();
}


void jsonUtility::from_json(const json &j, std::string &user, std::string &pass) {
    user = j.at("content").at("username").get<std::string>();
    pass = j.at("content").at("password").get<std::string>();
}

void jsonUtility::from_json_inviteURI(const json &j, std::string &invited, std::string &applicant, std::string &uri) {
    invited = j.at("invited").get<std::string>();
    applicant = j.at("applicant").get<std::string>();
    uri = j.at("uri").get<std::string>();
}

void jsonUtility::from_json_user_filename(const json &j, std::string &user, std::string &filename) {
    user = j.at("content").at("username").get<std::string>();
    filename = j.at("content").at("filename").get<std::string>();
}

void jsonUtility::from_json_uri(const json &j, std::string &user, std::string &uri) {
    user = j.at("content").at("username").get<std::string>();
    uri = j.at("content").at("uri").get<std::string>();
}

void jsonUtility::from_json_username(const json &j, std::string &user) {
    user = j.at("content").at("username").get<std::string>();
}

void jsonUtility::from_json(const json &j, std::string &user, std::string &pass, std::string &email) {
    user = j.at("content").at("username").get<std::string>();
    pass = j.at("content").at("password").get<std::string>();
    email = j.at("content").at("email").get<std::string>();
}

//json jsonUtility::merge(const json &a, const json &b) {
//    json result = a.flatten();
//    json tmp = b.flatten();
//
//    for (json::iterator it = tmp.begin(); it != tmp.end(); ++it) {
//        result[it.key()] = it.value();
//    }
//    return result.unflatten();
//}

std::vector<json> jsonUtility::from_syms_to_jsons(const std::vector<Symbol>& symbols) {
    if(symbols.empty())
        return json::array();

    // Get jsons from symbols
    std::vector<json> jsons;
    for (auto const &sym: symbols) {
        json j;
        jsonUtility::to_json_symbol(j, sym); //convert sym into json
        jsons.push_back(j);
    }
    return jsons;
}

std::vector<Symbol> jsonUtility::from_jsons_to_syms(const std::vector<json>& jsons) {
    // Get symbols from json
    std::vector<Symbol> symbols;
    for (auto const &j: jsons) {
        symbols.push_back(jsonUtility::from_json_symbol(j));
    }
    return symbols;
}

std::vector<json> jsonUtility::from_files_to_json(const std::vector<File>& files) {
    // Get jsons from symbols
    std::vector<json> jsons;
    for (auto const &f: files) {
        json j;
        jsonUtility::to_json_file(j, f); //convert sym into json
        jsons.push_back(j);
    }
    return jsons;
}

void jsonUtility::from_json_stored_to_syms(const json& j, std::vector<json>& json_symbols) {
    json_symbols = j.get<std::vector<json>>();
}

Symbol jsonUtility::from_json_symbol(const json &j) {
    //get symbol values from json
    std::pair<int,int> id = j.at("id").get<std::pair<int,int>>();
    std::vector<int> pos = j.at("pos").get<std::vector<int>>();
    wchar_t letter = j.at("letter").get<wchar_t>();
    SymStyle style(j.at("isBold").get<bool>(), j.at("isItalic").get<bool>(), j.at("isUnderlined").get<bool>(),
                   j.at("fontFamily").get<std::string>(), j.at("fontSize").get<int>(), j.at("alignment").get<int>(),
                   j.at("color").get<std::string>());

    //now create the symbol
    Symbol s(letter, id, pos, style);
    return s;
}

void jsonUtility::from_json_cursor_change_req(const json &j, int& index) {
    index = j.at("index").get<int>();
}

void jsonUtility::from_json_collab_colors_req(const json &j, std::string& uri) {
    uri = j.at("uri").get<std::string>();
}

void jsonUtility::from_json_removal_range_req(const json &j, std::vector<std::pair<int,int>>& symbolsId) {
    symbolsId = j.at("symbolsId").get<std::vector<std::pair<int,int>>>();
}

void jsonUtility::from_json_format_range_req(const json &j, std::vector<std::pair<int,int>>& symbolsId, int& format) {
    symbolsId = j.at("symbolsId").get<std::vector<std::pair<int,int>>>();
    format = j.at("format").get<int>();
}

void jsonUtility::from_json_fontsize_change_req(const json &j, std::vector<std::pair<int,int>>& symbolsId, int& fontSize) {
    symbolsId = j.at("symbolsId").get<std::vector<std::pair<int,int>>>();
    fontSize = j.at("fontSize").get<int>();
}

void jsonUtility::from_json_font_family_ch_req(const json &j, std::vector<std::pair<int,int>>& symbolsId, std::string& fontFamily) {
    symbolsId = j.at("symbolsId").get<std::vector<std::pair<int,int>>>();
    fontFamily = j.at("fontFamily").get<std::string>();
}

void jsonUtility::from_json_alignment_ch_req(const json &j, std::vector<std::pair<int,int>>& symbolsId, int& alignment) {
    symbolsId = j.at("symbolsId").get<std::vector<std::pair<int,int>>>();
    alignment = j.at("alignment").get<int>();
}

void jsonUtility::from_json_insertion_range_req(const json &j, std::vector<json>& symbols, int& startIndex) {
    symbols = j.at("formattingSymVector").get<std::vector<json>>();
    startIndex = j.at("startIndex").get<int>();
}

void jsonUtility::from_json_rename_file(const json &j, std::string &file_name, std::string &uri, std::string &username) {
    file_name = j.at("content").at("newNameFile").get<std::string>();
    uri = j.at("content").at("uri").get<std::string>();
    username = j.at("content").at("username").get<std::string>();
}