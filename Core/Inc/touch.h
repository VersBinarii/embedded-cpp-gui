/*
 * touch.h
 *
 *  Created on: Mar 25, 2021
 *      Author: kris
 */

#ifndef INC_TOUCH_H_
#define INC_TOUCH_H_

#include <main.h>
#include <lcd.h>
#include <GFX_color.h>
#include <array>


namespace XPT2046 {
    constexpr static uint8_t MAX_SAMPLES = 5;
    constexpr static uint8_t TX_BUFF_LEN = 5;

    struct Point {
        uint16_t x;
        uint16_t y;
        constexpr Point () : x (0), y (0) {}
        constexpr Point (uint16_t x, uint16_t y) : x (x), y (y) {}
    };

    struct CalibrationPoint {
        std::array<uint16_t, 2> a;
        std::array<uint16_t, 2> b;
        std::array<uint16_t, 2> c;

        constexpr CalibrationPoint (std::array<uint16_t, 2> a,
                                    std::array<uint16_t, 2> b,
                                    std::array<uint16_t, 2> c)
            : a (std::move (a)), b (std::move (b)), c (std::move (c)) {}
        constexpr CalibrationPoint () : a{ 0, 0 }, b{ 0, 0 }, c{ 0, 0 } {}
    };

    struct CalibrationData {
        double alpha_x;
        double beta_x;
        double delta_x;
        double alpha_y;
        double beta_y;
        double delta_y;

        constexpr CalibrationData (double ax, double bx, double dx, double ay,
                                   double by, double dy)
            : alpha_x (ax), beta_x (bx), delta_x (dx), alpha_y (ay),
              beta_y (by), delta_y (dy){};
    };


    struct TouchSamples {
        uint32_t sample_timer;
        std::array<Point, MAX_SAMPLES> samples;
        uint8_t counter;

        // Functions
        constexpr TouchSamples () : sample_timer (0), counter (0) {}
        constexpr Point average () const;
    };

    class TouchPanel {

        enum class TouchScreenState : uint8_t {
            IDLE,
            PRESAMPLING,
            TOUCHED,
            RELEASED
        };

        enum class TouchScreenOperationMode : uint8_t { NORMAL, CALIBRATION };

        const SPI_HandleTypeDef *spi;
        IRQn_Type touch_irq;
        uint8_t tx_buffer[TX_BUFF_LEN];
        uint8_t rx_buffer[TX_BUFF_LEN];
        // Always starts in IDLE step
        volatile TouchScreenState state{ TouchScreenState::IDLE };
        // Init to default
        TouchSamples ts{};

        const Point read_xy ();
        void get_raw_data ();
        Point read_touch_point ();

      public:
        CalibrationPoint cp_raw;
        CalibrationData cd;
        // We start NORMAL operation by default
        TouchScreenOperationMode mode{ TouchScreenOperationMode::NORMAL };

        TouchPanel ();
        void init (const SPI_HandleTypeDef &hspi, IRQn_Type hirq);
        void IRQ_handler ();
        void run ();
        void calibrate (const GFX_Color::GFX &gfx);
        Point get_touch_point () const;
        bool is_touched ();
    };

}

#endif /* INC_TOUCH_H_ */
