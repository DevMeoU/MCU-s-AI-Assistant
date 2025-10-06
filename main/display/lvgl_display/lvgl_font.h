#pragma once

#include <lvgl.h>

#ifdef BUILTIN_TEXT_FONT
    LV_FONT_DECLARE(BUILTIN_TEXT_FONT);
#else
    #error "Please define BUILTIN_TEXT_FONT"
#endif

#ifdef BUILTIN_ICON_FONT
    LV_FONT_DECLARE(BUILTIN_ICON_FONT);
#else
    #error "Please define BUILTIN_ICON_FONT"
#endif

#ifdef BUILTIN_FONT_AWESOME_30_1
    LV_FONT_DECLARE(BUILTIN_FONT_AWESOME_30_1);
#else
    #define BUILTIN_FONT_AWESOME_30_1   font_awesome_30_1
    LV_FONT_DECLARE(BUILTIN_FONT_AWESOME_30_1)
#endif

#ifdef BUILTIN_FONT_AWESOME_30_4
    LV_FONT_DECLARE(BUILTIN_FONT_AWESOME_30_4);
#else
    #define BUILTIN_FONT_AWESOME_30_4   font_awesome_30_4
    LV_FONT_DECLARE(BUILTIN_FONT_AWESOME_30_4)
#endif

class LvglFont {
public:
    virtual const lv_font_t* font() const = 0;
    virtual ~LvglFont() = default;
};

// Built-in font
class LvglBuiltInFont : public LvglFont {
public:
    LvglBuiltInFont(const lv_font_t* font) : font_(font) {}
    virtual const lv_font_t* font() const override { return font_; }

private:
    const lv_font_t* font_;
};


class LvglCBinFont : public LvglFont {
public:
    LvglCBinFont(void* data);
    virtual ~LvglCBinFont();
    virtual const lv_font_t* font() const override { return font_; }

private:
    lv_font_t* font_;
};
