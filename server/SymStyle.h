//
// Created by gheb on 31/07/20.
//

#ifndef PROJECT_SYMSTYLE_H
#define PROJECT_SYMSTYLE_H

#include <string>

#define DEFAULT_FONT_FAMILY "Times New Roman"
#define DEFAULT_FONT_SIZE 14
#define DEFAULT_ALIGNMENT 1
#define DEFAULT_COLOR "#00ffffff"



class SymStyle {

private:
    bool is_bold;
    bool is_italic;
    bool is_underlined;
    int font_sz;
    std::string font_family;
    int alignment;
    std::string color;

public:
    SymStyle();
    SymStyle(bool isBold, bool isItalic, bool isUnderlined, std::string fontFamily, int fontSize, int alignment, std::string color);

    std::string get_font_family() const;
    int get_font_sz() const;
    int get_alignment() const;
    bool isBold() const;
    bool isItalic() const;
    bool isUnderlined() const;
    std::string getColor() const;

    /* Setters */
    void set_bold(bool bold);
    void set_italic(bool italic);
    void set_underlined(bool underlined);
    void set_font_family(std::string family);
    void set_font_sz(int size);
    void set_alignment(int value);
    void set_color(std::string clr);

};


#endif //PROJECT_SYMSTYLE_H
