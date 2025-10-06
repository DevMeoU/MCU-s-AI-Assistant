#pragma once

#include <lvgl.h>

LV_FONT_DECLARE(BUILTIN_TEXT_FONT);
LV_FONT_DECLARE(BUILTIN_ICON_FONT);
LV_FONT_DECLARE(font_awesome_30_1);
LV_FONT_DECLARE(font_awesome_30_4);

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
