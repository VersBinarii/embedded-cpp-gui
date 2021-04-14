#ifndef SRC_GUI_H_
#define SRC_GUI_H_

#include <string>
#include <algorithm>
#include <graphics/GFX_color.h>
#include <graphics/point.h>
#include <touch.h>

using namespace GFX_Color;
using namespace XPT2046;

namespace GUI {

    constexpr static uint8_t BTN_WIDTH = 60;
    constexpr static uint8_t BTN_HEIGHT = 70;
    constexpr static uint8_t MAX_BTNS = 8;

    template <typename P> class Button {
        P x;
        P y;
        P w;
        P h;
        std::string text;
        bool visible;
        typedef void (*OnClick) ();

      public:
        OnClick on_click_handler;

        Button (){};
        Button (P x_, P y_, P w_, P h_, std::string t)
            : x{ x_ }, y{ y_ }, w{ w_ }, h{ h_ }, text{ std::move (t) },
              visible (true){};
        template <typename F> void on_click (F f) {
            this->on_click_handler = f;
        }
        void draw (const GFX &gfx) {
            gfx.draw_fill_round_rectangle (this->x, this->y, this->w, this->h,
                                           5, ILI9341::Color::WHITE);
            gfx.draw_string (this->text.c_str (), x - 5 + w / 2, y - 10 + h / 2,
                             ILI9341::Color::PURPLE, ILI9341::Color::BLACK,
                             Fonts::FontBackground::TRANSPARENT);
        }
        void show_btn () { this->visible = true; }
        void hide_btn () { this->visible = false; }
        bool is_touched (const Point<P> &p) const {
            if (this->visible) {
                // Validate width
                if (p.x > this->x && p.x < this->x + this->w) {
                    // and now height
                    if (p.y > this->y && p.y < this->y + this->h) {
                        return true;
                    }
                }
            }
            return false;
        }
    };

    template <typename T> class Gui {
        enum class View {
            MAIN,
        };
        std::array<Button<T>, MAX_BTNS> buttons;
        const TouchPanel &tp;
        uint8_t button_counter;
        bool redraw;

      public:
        Gui (const TouchPanel &tp_) : tp{ tp_ }, button_counter (0), redraw(true) {}
        void add_btn (Button<T> &btn) {
            if (this->button_counter < MAX_BTNS) {
                this->buttons[this->button_counter++] = std::move (btn);
            }
        }

        void process_touch_event () {

                const auto touch_point = this->tp.get_touch_point ();
                std::for_each (this->buttons.cbegin (), this->buttons.cend (),
                               [touch_point] (Button<T> b) {
                                   if (b.is_touched (touch_point)) {
                                       // We have our button
                                       b.on_click_handler ();
                                   }
                               });
        }
        void draw (const GFX &gfx) {
            if (this->redraw) {
                for (auto i = 0; i < this->button_counter; i++) {
                    // We have our button
                    this->buttons[i].draw (gfx);
                }
                redraw = false;
            }
        }
    };
}

#endif /* SRC_GUI_H_ */
