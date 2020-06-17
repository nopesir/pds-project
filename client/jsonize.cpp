#include "jsonize.h"


void Jsonize::to_json(json &j, const std::string &op, const std::string &resp) {
    j = json{
            {"operation", op},
            {"content", {
                {"response", resp}
            }}
    };
}

void Jsonize::to_json(json &j, const std::string &op, const std::string &user, const std::string &pass) {
    j = json{
            {"operation", op},
            {"content", {
                {"username", user},
                {"password", pass}
            }}
    };
}

void Jsonize::to_json_insertion(json &j, const std::string &op, const std::pair<int, wchar_t> &tuple, const SymbolStyle &style) {
    j = json{
        {"operation", op},
        {"tuple", tuple},
        {"isBold", style.isBold()},
        {"isItalic", style.isItalic()},
        {"isUnderlined", style.isUnderlined()},
        {"fontFamily", style.getFontFamily()},
        {"fontSize", style.getFontSize()},
        {"alignment", style.getAlignment()},
        {"color", style.getColor()}
    };
}

void Jsonize::to_json_cursor_change_req(json &j, const std::string &op, const int &index) {
    j = json {
        {"operation", op},
        {"index", index}
    };
}

void Jsonize::to_json_removal_range(json &j, const std::string &op, const int &startIndex, const int &endIndex) {
    j = json{
            {"operation", op},
            {"startIndex", startIndex},
            {"endIndex", endIndex}
    };
}

void Jsonize::to_json_format_range(json &j, const std::string &op, const int &startIndex, const int &endIndex, const int &format) {
    j = json{
            {"operation", op},
            {"startIndex", startIndex},
            {"endIndex", endIndex},
            {"format", format}
    };
}

void Jsonize::to_json_fontsize_change(json &j, const std::string &op, const int &startIndex, const int &endIndex, const int &fontSize) {
    j = json{
            {"operation", op},
            {"startIndex", startIndex},
            {"endIndex", endIndex},
            {"fontSize", fontSize}
    };
}

void Jsonize::to_json_alignment_change(json &j, const std::string &op, const int &startBlock, const int &endBlock, const int &alignment) {
    j = json{
            {"operation", op},
            {"startBlock", startBlock},
            {"endBlock", endBlock},
            {"alignment", alignment}
    };
}

void Jsonize::to_json_fontfamily_change(json &j, const std::string &op, const int &startIndex, const int &endIndex, const std::string &fontFamily) {
    j = json{
            {"operation", op},
            {"startIndex", startIndex},
            {"endIndex", endIndex},
            {"fontFamily", fontFamily}
    };
}

void Jsonize::to_json_collab_colors(json &j, const std::string &op, const std::string &uri) {
    j = json{
        {"operation", op},
        {"uri", uri}
    };
}

void Jsonize::to_jsonFilename(json &j, const std::string &op, const std::string &user, const std::string &filename) {
    j = json{
            {"operation", op},
            {"content", {
                {"username", user},
                {"filename", filename}
            }}
    };
}

void Jsonize::to_jsonRenamefile(json &j, const std::string &op, const std::string &nameFile, const std::string &uri, const std::string &username) {
    j = json{
            {"operation", op},
            {"content", {
                {"newNameFile", nameFile},
                {"uri", uri},
                {"username", username}
            }}
    };
}

void Jsonize::to_jsonUri(json &j, const std::string &op, const std::string &user, const std::string &uri) {
    j = json{
            {"operation", op},
            {"content", {
                {"username", user},
                {"uri", uri}
            }}
    };
}

void Jsonize::to_json_inviteURI(json &j, const std::string &op, const std::string &invited, const std::string &applicant, const std::string &uri) {
    j = json{
            {"operation", op},
            {"invited", invited},
            {"applicant", applicant},
            {"uri", uri}
    };
}

void Jsonize::to_json(json &j, const std::string &op, const std::string &user, const std::string &pass, const std::string &email) {
    j = json{
            {"operation", op},
            {"content", {
                {"username", user},
                {"password", pass},
                {"email", email}
            }}
    };
}

void Jsonize::to_jsonUser(json &j, const std::string &op, const std::string &user) {
    j = json{
            {"operation", op},
            {"content", {
                {"username", user}
            }}
    };
}

void Jsonize::to_json_insertion_range(json &j, const std::string &op, const std::vector<json> &symVector) {
    j = json{
            {"operation", op},
            {"formattingSymVector", symVector} //JSON vector
    };
}

void Jsonize::from_json(const json &j, std::string &op) {
    op = j.at("operation").get<std::string>();
}

void Jsonize::from_json_inviteURI(const json &j, std::string &op) {
    op = j.at("content").at("response").get<std::string>();
}

void Jsonize::from_json_collab_colors_map(const json &j, std::string &op) {
    op = j.at("response").get<std::string>();
}

void Jsonize::from_json_resp(const json &j, std::string &resp) {
    resp = j.at("content").at("response").get<std::string>();
}

void Jsonize::from_json_formatting_symbols(const json &j, std::vector<json>& jsonSymbols) {
    jsonSymbols = j.at("formattingSymVector").get<std::vector<json>>();
}

/* We need to use this 'from_json' to deserialize std::vector<symbol> (see function from_json_symbols) */
void from_json(const json& j, Symbol& s) {
    wchar_t letter = j.at("letter").get<wchar_t>();
    std::pair<int,int> id = j.at("id").get<std::pair<int,int>>();
    std::vector<int> pos = j.at("pos").get<std::vector<int>>();
    SymbolStyle style;
    style.setBold(j.at("isBold").get<bool>());
    style.setItalic(j.at("isItalic").get<bool>());
    style.setUnderlined(j.at("isUnderlined").get<bool>());
    style.setFontFamily(j.at("fontFamily").get<std::string>());
    style.setFontSize(j.at("fontSize").get<int>());
    style.setAlignment(j.at("alignment").get<int>());
    style.setColor(j.at("color").get<std::string>());
    s = Symbol(letter, id, pos, style);
}

void Jsonize::from_json_symbols(const json &j, std::vector<Symbol>& symbols) {
    symbols = j.at("content").at("symVector").get<std::vector<Symbol>>(); //use from_json previously defined
}

void Jsonize::from_json_symbolsAndFilename(const json &j, std::vector<Symbol>& symbols, std::string& filename) {
    filename = j.at("content").at("filename").get<std::string>();
    symbols = j.at("content").at("symVector").get<std::vector<Symbol>>(); //use from_json previously defined
}

void Jsonize::from_json_filename(const json &j, std::string& filename) {
    filename = j.at("content").at("filename").get<std::string>();
}

void Jsonize::from_json_insertion_range(const json &j, int& firstIndex, std::vector<json>& jsonSymbols) {
    firstIndex = j.at("firstIndexRange").get<int>();
    jsonSymbols = j.at("symbols").get<std::vector<json>>();
}

void Jsonize::from_json_rename_file(const json &j, std::string &resp, std::string& filename) {
    filename = j.at("content").at("filename").get<std::string>();
    resp = j.at("content").at("response").get<std::string>();
}

void Jsonize::from_json_files(const json &j, std::vector<json>& jsonFiles) {
    jsonFiles = j.at("content").at("vectorFile").get<std::vector<json>>();
}

Symbol* Jsonize::from_json_symbol(const json &j) {
    wchar_t letter;
    std::pair<int,int> id;
    std::vector<int> pos;
    SymbolStyle style;

    try {
        //get symbol values from json
        letter = j.at("letter").get<wchar_t>();
        id = j.at("id").get<std::pair<int, int>>();
        pos = j.at("pos").get<std::vector<int>>();
        style = {j.at("isBold").get<bool>(), j.at("isItalic").get<bool>(),j.at("isUnderlined").get<bool>(),
                 j.at("fontFamily").get<std::string>(), j.at("fontSize").get<int>(), j.at("alignment").get<int>(),
                 j.at("color").get<std::string>()};
    }
    catch (json::exception& e) {
        std::cerr << "message: " << e.what() << '\n' << "exception id: " << e.id << std::endl;
        return nullptr;
    }

    //now create the symbol
    Symbol *s = new Symbol(letter, id, pos, style);
    return s;
}

TemplateSymbol* Jsonize::from_json_formatting_symbol(const json &j) {

    //get symbol values from json
    int index = j.at("index").get<int>();
    wchar_t letter = j.at("letter").get<wchar_t>();
    SymbolStyle style(j.at("isBold").get<bool>(), j.at("isItalic").get<bool>(),j.at("isUnderlined").get<bool>(),
                      j.at("fontFamily").get<std::string>(), j.at("fontSize").get<int>(), j.at("alignment").get<int>(),
                      j.at("color").get<std::string>());

    //now create the symbol
    TemplateSymbol *s = new TemplateSymbol(index, letter, style);
    return s;
}

File* Jsonize::from_json_file(const json &j) {
    std::string idfile;
    std::string filename;
    std::string owner;
    std::string timestamp;

    try {
        //get symbol values from json
        idfile = j.at("idfile").get<std::string>();
        filename = j.at("filename").get<std::string>();
        owner = j.at("owner").get<std::string>();
        timestamp = j.at("timestamp").get<std::string>();

    } catch (json::exception& e) {
        std::cerr << "message: " << e.what() << '\n' << "exception id: " << e.id << std::endl;
        return nullptr;
    }
    //now create the file
    File *f = new File(idfile, filename, owner, timestamp);
    return f;
}

void Jsonize::from_json_usernameLogin(const json &j, std::string &name, std::string &color, std::string &mail, int &siteID) {
    name = j.at("content").at("usernameLogin").get<std::string>();
    color = j.at("content").at("colorLogin").get<std::string>();
    mail = j.at("content").at("mailLogin").get<std::string>();
    siteId = j.at("content").at("siteId").get<int>();
}

void Jsonize::from_json(const json &j, std::string &user, std::string &pass) {
    user = j.at("content").at("username").get<std::string>();
    pass = j.at("content").at("password").get<std::string>();
}

void Jsonize::from_json(const json &j, std::string &user, std::string &pass, std::string &email) {
    user = j.at("content").at("username").get<std::string>();
    pass = j.at("content").at("password").get<std::string>();
    email = j.at("content").at("email").get<std::string>();
}

void Jsonize::from_jsonUri(const json &j, std::string &uri) {
    uri = j.at("content").at("uri").get<std::string>();
}

void Jsonize::from_json_insertion(const json& j, Symbol& s, int &indexInEditor) {
    indexInEditor = j.at("indexInEditor").get<int>();
    wchar_t letter = j.at("letter").get<wchar_t>();
    std::pair<int,int> id = j.at("id").get<std::pair<int,int>>();
    std::vector<int> pos = j.at("pos").get<std::vector<int>>();
    SymbolStyle style;
    style.setBold(j.at("isBold").get<bool>());
    style.setItalic(j.at("isItalic").get<bool>());
    style.setUnderlined(j.at("isUnderlined").get<bool>());
    style.setFontFamily(j.at("fontFamily").get<std::string>());
    style.setFontSize(j.at("fontSize").get<int>());
    style.setAlignment(j.at("alignment").get<int>());
    style.setColor(j.at("color").get<std::string>());
    s = Symbol(letter, id, pos, style);
}

void Jsonize::from_json_removal(const json &j, int& index) {
    index = j.at("index").get<int>();
}

void Jsonize::from_json_cursor_change(const json &j, std::string& username, std::string& color, int& pos) {
    username = j.at("username").get<std::string>();
    color = j.at("color").get<std::string>();
    pos = j.at("pos").get<int>();
}

void Jsonize::from_json_collab_colors_resp(const json &j, myCollabColorsMap& collabColorsMap) {
    collabColorsMap = j.at("collabColorsMap").get<myCollabColorsMap>();
}

void Jsonize::from_json_user_offline(const json &j, std::string &username, myCollabColorsMap& collabColorsMap) {
    username = j.at("username").get<std::string>();
    collabColorsMap = j.at("collabColorsMap").get<myCollabColorsMap>();
}

void Jsonize::from_json_removal_range(const json &j, std::vector<sId>& symbolsId) {
    symbolsId = j.at("symbolsId").get<std::vector<sId>>();
}

void Jsonize::from_json_format_range(const json &j, std::vector<sId>& symbolsId, int& format) {
    symbolsId = j.at("symbolsId").get<std::vector<sId>>();
    format = j.at("format").get<int>();
}

void Jsonize::from_json_fontsize_change(const json &j, std::vector<sId>& symbolsId, int& fontSize) {
    symbolsId = j.at("symbolsId").get<std::vector<sId>>();
    fontSize = j.at("fontSize").get<int>();
}

void Jsonize::from_json_alignment_change(const json &j, int& startBlock, int& endBlock, int& alignment) {
    startBlock = j.at("startBlock").get<int>();
    endBlock = j.at("endBlock").get<int>();
    alignment = j.at("alignment").get<int>();
}

void Jsonize::from_json_fontfamily_change(const json &j, std::vector<sId>& symbolsId, std::string& fontFamily) {
    symbolsId = j.at("symbolsId").get<std::vector<sId>>();
    fontFamily = j.at("fontFamily").get<std::string>();
}

void Jsonize::to_json_FormattingSymbol(json &j, const TemplateSymbol &symbol) {
    j = json{
            {"index", symbol.getIndex()},
            {"letter", symbol.getLetter()},
            {"isBold", symbol.getStyle().isBold()},
            {"isItalic", symbol.getStyle().isItalic()},
            {"isUnderlined", symbol.getStyle().isUnderlined()},
            {"fontFamily", symbol.getStyle().getFontFamily()},
            {"fontSize", symbol.getStyle().getFontSize()},
            {"alignment", symbol.getStyle().getAlignment()},
            {"color", symbol.getStyle().getColor()}
    };
}

std::vector<json> Jsonize::fromFormattingSymToJson(const std::vector<TemplateSymbol>& symbols) {
    if(symbols.empty())
        return json::array();

    // Get jsons from symbols
    std::vector<json> jsons;
    for (auto const &sym: symbols) {
        json j;
        Jsonize::to_json_FormattingSymbol(j, sym); //convert sym into json
        jsons.push_back(j);
    }
    return jsons;
}
