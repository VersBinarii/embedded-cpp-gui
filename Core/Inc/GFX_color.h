#ifndef GFX_COLOR_H_
#define GFX_COLOR_H_

#include <main.h>
#include <lcd.h>
#include <fonts.h>

namespace GFX_Color {

#define USING_STRINGS 1 // 0 - do not compile, 1 - compile

#define USING_IMAGE 1
#if USING_IMAGE == 1
#define USING_IMAGE_ROTATE 0
#endif

// Trygonometric graphic functions
#define USING_RECTANGLE 1
#define USING_CIRCLE 0
#define USING_FILL_CIRCLE 0
#define USING_ROUND_RECTANGLE 0
#define USING_FILL_ROUND_RECTANGLE 0
#define USING_TRIANGLE 0
#define USING_FILL_TRIANGLE 0
#if ((USING_FILL_ROUND_RECTANGLE == 0) && (USING_STRINGS == 0))
#define USING_FILL_RECTANGLE 0
#endif
#if (USING_RECTANGLE == 0) && (USING_FILL_RECTANGLE == 0) && (USING_FILL_CIRCLE == 0) && (USING_ROUND_RECTANGLE == 0) && (USING_TRIANGLE == 0) && (USING_FILL_TRIANGLE == 0)
#define USING_LINES 0
#endif

/****************************************************************/

#if (USING_FILL_ROUND_RECTANGLE == 1 || USING_STRINGS == 1)
#define USING_FILL_RECTANGLE 1
#endif
#if (USING_RECTANGLE == 1) || (USING_FILL_RECTANGLE == 1) || (USING_FILL_CIRCLE == 1) || (USING_ROUND_RECTANGLE == 1) || (USING_TRIANGLE == 1) || (USING_FILL_TRIANGLE == 1)
#define USING_LINES 1
#endif
#if USING_ROUND_RECTANGLE == 1
#define CIRCLE_HELPER
#endif
#if (USING_FILL_CIRCLE == 1) || (USING_FILL_ROUND_RECTANGLE == 1)
#define FILL_CIRCLE_HELPER
#endif

#if USING_STRINGS == 1
/*
 *
 */
struct GFX {
    const ILI9341::LCD *const lcd;
    const Fonts::Font *const font;

    GFX (const ILI9341::LCD &lcd, const Fonts::Font &font);

    uint8_t get_font_height ();
    uint8_t draw_char (char ch, uint16_t x, uint16_t y,
                       ILI9341::Color fg_color, ILI9341::Color bg_color,
                       Fonts::FontBackground fb) const;
    uint8_t draw_string (const char *str, uint16_t x, uint16_t y,
                         ILI9341::Color fg_color, ILI9341::Color bg_color,
                         Fonts::FontBackground fb) const;

#endif

#if USING_LINES == 1
    void draw_line (uint16_t x_start, uint16_t y_start, uint16_t x_end,
                    uint16_t y_end, ILI9341::Color color) const;
#endif

#if USING_RECTANGLE == 1
    void draw_rectangle (uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                         ILI9341::Color color) const;
#endif
#if USING_FILL_RECTANGLE ==1
    void draw_fill_rectangle (uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                              ILI9341::Color color) const;
#endif
#if USING_CIRCLE == 1
    void draw_circle (uint16_t x0, uint16_t y0, uint16_t r,
                      ILI9341::Color color) const;
#endif
#if USING_FILL_CIRCLE == 1
    void draw_fill_circle (uint16_t x0, uint16_t y0, uint16_t r,
                           ILI9341::Color color) const;
#endif
#if USING_ROUND_RECTANGLE == 1
    void draw_round_rectangle (uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                               uint16_t r, ILI9341::Color color) const;
#endif
#if USING_FILL_ROUND_RECTANGLE == 1
    void draw_fill_round_rectangle (uint16_t x, uint16_t y, uint16_t w,
                                    uint16_t h, uint16_t r,
                                    ILI9341::Color color) const;
#endif
#if USING_TRIANGLE == 1
    void draw_triangle (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                        uint16_t x2, uint16_t y2, ILI9341::Color color) const;
#endif
#if USING_FILL_TRIANGLE == 1
    void draw_fill_triangle (uint16_t x0, uint16_t y0, uint16_t x1,
                             uint16_t y1, uint16_t x2, uint16_t y2,
                             ILI9341::Color color) const;
#endif
#if USING_IMAGE == 1
    void draw_image (uint16_t x, uint16_t y, const uint8_t *img, uint16_t w,
                     uint16_t h) const;
#if USING_IMAGE_ROTATE == 1
    void image_rotate (uint16_t x, uint16_t y, const uint8_t *img, uint8_t w,
                       uint8_t h, ILI9341::Color color, uint16_t angle) const;
#endif
#endif
};
}
#endif /* GFX_Color_H_ */
