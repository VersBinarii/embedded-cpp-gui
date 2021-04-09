#include <fonts.h>
#include <string.h>

namespace Fonts {
    Font::Font (const FONT_INFO &font) : current_font (font) {}

    uint8_t Font::put_char (const ILI9341::LCD &lcd, char ch, uint16_t x,
                            uint16_t y, ILI9341::Color fg_color,
                            ILI9341::Color bg_color, FontBackground fb) const {
        const uint8_t char_number = ch - this->current_font.start_char;

        // Handle space char
        if (ch == ' ') {
            return 5 * this->current_font.space_width;
        }

        // Check if the character to be printed is within
        // the bounds of our font table
        if (char_number > this->current_font.end_char) {
            return 0;
        }

        const FONT_CHAR_INFO &current_char
            = this->current_font.description[char_number];

        const uint8_t *current_char_bitmap
            = &this->current_font.char_bitmap[current_char.char_offset];
        uint8_t char_bits = 0;
        // Iterate over row by height
        for (uint16_t pixel_height = 0;
             pixel_height < this->current_font.char_height; pixel_height++) {
            // How many bits describe the character
            uint8_t char_bits_remaining = current_char.char_width;

            // Iterate over each byte in current chars row
            for (uint8_t byte = 0; byte <= (current_char.char_width - 1) / 8; byte++) {
                uint8_t line = static_cast<uint8_t> (*current_char_bitmap);
                if (char_bits_remaining >= 8) {
                    char_bits_remaining -= 8;
                    char_bits = 8;
                }else{
                	char_bits = char_bits_remaining;
                }

                // Iterate over each bit in current char rows byte
                for (uint8_t i = 0; i < char_bits; i++, line <<= 1) {
                    if (line & 0x80) {
                        lcd.write_pixel (x + (byte * 8) + i, y + pixel_height,
                                         fg_color);
                    } else if (fb == FontBackground::COLOR) {
                        lcd.write_pixel (x + (byte * 8) + i, y + pixel_height,
                                         bg_color);
                    }
                }
                // Lets grab next byte(line) in the current char
                current_char_bitmap++;
            }
        }
        // On success return width of the char we just processed
        return current_char.char_width;
    }
    uint8_t Font::put_string (const ILI9341::LCD &lcd, const char *str,
                              uint16_t x, uint16_t y, ILI9341::Color fg_color,
                              ILI9341::Color bg_color,
                              FontBackground fb) const {

        const uint8_t str_len = strlen ((char *)str);
        uint16_t shift = 0;
        uint8_t idx = 0;
       while (idx < str_len) {
            const uint8_t char_width = this->put_char (lcd, str[idx++], x + shift, y,
                                                 fg_color, bg_color, fb);
            // Width of char just printed + the space between chars
            shift += (char_width + this->current_font.space_width);
        }
        return shift - this->current_font.space_width;
    }
}
