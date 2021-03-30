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

#define MAX_SAMPLES 10

namespace XPT2046 {
    struct Point {
        uint16_t x;
        uint16_t y;
        constexpr Point () : x (0), y (0) {}
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
    };

    enum class TouchScreenState { IDLE, PRESAMPLING, TOUCHED, RELEASED };

    struct TouchSamples {
        uint32_t sample_timer;
        std::array<Point, MAX_SAMPLES> samples;
        uint8_t counter;

        constexpr Point average ();
        constexpr TouchSamples () : sample_timer (0), counter (0) {}
    };

    enum class TouchScreenOperationMode { NORMAL, CALIBRATION };

    struct TouchPanel {
        SPI_HandleTypeDef *touch_spi;
        IRQn_Type touch_irq;
        uint8_t tx_buffer[5];
        uint8_t rx_buffer[5];
        CalibrationData cd;
        // We start NORMAL operation by default
        TouchScreenOperationMode mode{ TouchScreenOperationMode::NORMAL };
        CalibrationPoint cp_raw;
        // Always starts in IDLE step
        volatile TouchScreenState state{ TouchScreenState::IDLE };
        // Init to default
        TouchSamples ts{};
    };

    void init (TouchPanel &tp, SPI_HandleTypeDef &hspi, IRQn_Type hirq);
    void run (TouchPanel &tp);
    bool is_touched (TouchPanel &tp);
    constexpr Point get_touch_point (TouchPanel &tp);
    void IRQ_handler (TouchPanel &tp);
    void calibrate (TouchPanel &tp, GFX_Color::GFX &gfx);
}

#endif /* INC_TOUCH_H_ */
