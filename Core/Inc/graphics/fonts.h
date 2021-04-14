#include <memory>
#include "main.h"
#include <lcd.h>

#ifndef INC_FONTS_FONTS_H
#define INC_FONTS_FONTS_H

namespace Fonts {
    enum class FontBackground { TRANSPARENT, COLOR };

    struct FONT_CHAR_INFO {
        const uint8_t char_width;
        const uint16_t char_offset;
    };

    struct FONT_INFO {
        const uint8_t char_height;
        const uint8_t start_char;
        const uint8_t end_char;
        const uint8_t space_width;
        const FONT_CHAR_INFO *const description;
        const uint8_t *const char_bitmap;
    };

    struct Font {

        const FONT_INFO &current_font;
        Font (const FONT_INFO &font);

        uint8_t put_char (const ILI9341::LCD &lcd, char ch, uint16_t x,
                          uint16_t y, ILI9341::Color fg_color,
                          ILI9341::Color bg_color, FontBackground fb) const;
        uint8_t put_string (const ILI9341::LCD &lcd, const char *str,
                            uint16_t x, uint16_t y, ILI9341::Color fg_color,
                            ILI9341::Color bg_color, FontBackground fb) const;
    };
}
#endif /* INC_FONTS_FONTS_H_ */
