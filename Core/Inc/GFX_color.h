#ifndef GFX_COLOR_H_
#define GFX_COLOR_H_

#include <main.h>
#include <lcd.h>

namespace GFX_Color {
//
// Adjust color type to your TFT
//
#define WIDTH SSD1306_LCDWIDTH
#define HEIGHT SSD1306_LCDHEIGHT
#define PIXEL_BLACK	ILI9341_BLACK
#define PIXEL_WHITE	ILI9341_WHITE

#define USING_STRINGS 1 // 0 - do not compile, 1 - compile

#define USING_IMAGE 1
#if USING_IMAGE == 1
#define USING_IMAGE_ROTATE 0
#endif

// Trygonometric graphic functions
#define USING_RECTANGLE 1
#define USING_CIRCLE 0
#define USING_FILL_CIRCLE 0
#define USING_ROUND_RECTANGLE 1
#define USING_FILL_ROUND_RECTANGLE 1
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
    ILI9341::LCD *lcd;
    const uint8_t *font;
    uint8_t font_size;
};

void init (GFX &gfx, ILI9341::LCD &lcd);
void set_font (GFX &gfx, const uint8_t *font_t);
void gfx_set_font_size (GFX &gfx, uint8_t size_t);
uint8_t get_font_height (GFX &gfx);
uint8_t get_font_width (GFX &gfx);
uint8_t get_font_size (GFX &gfx);
void draw_char (GFX &gfx, int x, int y, char chr, ILI9341::Color color);
void draw_string (GFX &gfx, int x, int y, char *str, ILI9341::Color color);
#endif

#if USING_LINES == 1
void draw_line (GFX &gfx, int x_start, int y_start, int x_end, int y_end,
                ILI9341::Color color);
#endif

#if USING_RECTANGLE == 1
void draw_rectangle (GFX &gfx, int x, int y, uint16_t w, uint16_t h,
                     ILI9341::Color color);
#endif
#if USING_FILL_RECTANGLE ==1
void draw_fill_rectangle (GFX &gfx, int x, int y, uint16_t w, uint16_t h,
                          ILI9341::Color color);
#endif
#if USING_CIRCLE == 1
void draw_circle (GFX &gfx, int x0, int y0, uint16_t r, ILI9341::Color color);
#endif
#if USING_FILL_CIRCLE == 1
void draw_fill_circle (GFX &gfx, int x0, int y0, uint16_t r,
                       ILI9341::Color color);
#endif
#if USING_ROUND_RECTANGLE == 1
void draw_round_rectangle (GFX &gfx, int x, int y, uint16_t w, uint16_t h,
                           uint16_t r, ILI9341::Color color);
#endif
#if USING_FILL_ROUND_RECTANGLE == 1
void draw_fill_round_rectangle (GFX &gfx, int x, int y, uint16_t w, uint16_t h,
                                uint16_t r, ILI9341::Color color);
#endif
#if USING_TRIANGLE == 1
void draw_triangle (GFX &gfx, int x0, int y0, int x1, int y1, int x2, int y2,
                    ILI9341::Color color);
#endif
#if USING_FILL_TRIANGLE == 1
void draw_fill_triangle (GFX &gfx, int x0, int y0, int x1, int y1, int x2,
                         int y2, ILI9341::Color color);
#endif
#if USING_IMAGE == 1
void draw_image (GFX &gfx, int x, int y, const uint8_t *img, uint16_t w,
                 uint16_t h);
#if USING_IMAGE_ROTATE == 1
void image_rotate (GFX &gfx, int x, int y, const uint8_t *img, uint8_t w,
                   uint8_t h, ILI9341::Color color, uint16_t angle);
#endif
#endif
}
#endif /* GFX_Color_H_ */
