//
// Created by gheb on 31/07/20.
//

#ifndef PROJECT_SYMBOLSTYLE_H
#define PROJECT_SYMBOLSTYLE_H

#include <string>

#define DEFAULT_FONT_FAMILY "Times New Roman"
#define DEFAULT_FONT_SIZE 14
#define DEFAULT_ALIGNMENT 1
#define DEFAULT_COLOR "#00ffffff"



class SymbolStyle {

private:
    bool _isBold;
    bool _isItalic;
    bool _isUnderlined;
    std::string _fontFamily;
    int _fontSize;
    int _alignment;
    std::string _color;

public:
    /* Constructors */
    SymbolStyle(); //default style parameters
    SymbolStyle(bool isBold, bool isItalic, bool isUnderlined, std::string fontFamily, int fontSize, int alignment, std::string color);

    /* Getters */
    std::string getFontFamily() const;
    int getFontSize() const;
    int getAlignment() const;
    bool isBold() const;
    bool isItalic() const;
    bool isUnderlined() const;
    std::string getColor() const;

    /* Setters */
    void setBold(bool val);
    void setItalic(bool val);
    void setUnderlined(bool val);
    void setFontFamily(std::string fontFamily);
    void setFontSize(int value);
    void setAlignment(int value);
    void setColor(std::string color);

};


#endif //PROJECT_SYMBOLSTYLE_H
