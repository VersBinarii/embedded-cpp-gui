/*
 * touch.h
 *
 *  Created on: Mar 25, 2021
 *      Author: kris
 */

#ifndef INC_TOUCH_H_
#define INC_TOUCH_H_

#include <graphics/GFX_color.h>
#include <graphics/point.h>
#include <main.h>
#include <lcd.h>
#include <array>


namespace XPT2046 {

    constexpr static uint8_t MAX_SAMPLES = 32;
    constexpr static uint8_t TX_BUFF_LEN = 5;

    template <typename T, size_t N> struct CalibrationPoint {
        std::array<T, N> a;
        std::array<T, N> b;
        std::array<T, N> c;

        constexpr CalibrationPoint (std::array<T, N> a, std::array<T, N> b,
                                    std::array<T, N> c)
            : a (std::move (a)), b (std::move (b)), c (std::move (c)) {}
        /*
         * Create a default set of calibration  points
         */
        constexpr CalibrationPoint ()
            :
#if (ILI9341_ROTATION == ILI9341_ROTATION_0 \
     || ILI9341_ROTATION == ILI9341_ROTATION_180)
              a ({ 10, 10 }), b ({ 80, 280 }), c ({ 200, 170 })
#elif (ILI9341_ROTATION == ILI9341_ROTATION_90 \
       || ILI9341_ROTATION == ILI9341_ROTATION_270)
              a ({ 20, 25 }), b ({ 160, 220 }), c ({ 300, 110 })
#endif
        {
        }
        constexpr int32_t delta () const {
            return (a[0] - c[0]) * (b[1] - c[1])
                   - (b[0] - c[0]) * (a[1] - c[1]);
        }
    };

    template <typename T> struct CalibrationData {
        T alpha_x;
        T beta_x;
        T delta_x;
        T alpha_y;
        T beta_y;
        T delta_y;

        constexpr CalibrationData (T ax, T bx, T dx, T ay, T by, T dy)
            : alpha_x (std::move (ax)), beta_x (std::move (bx)),
              delta_x (std::move (dx)), alpha_y (std::move (ay)),
              beta_y (std::move (by)), delta_y (std::move (dy)){};
        constexpr CalibrationData ()
            :
#if (ILI9341_ROTATION == ILI9341_ROTATION_0)
              alpha_x (-0.0009337), beta_x (-0.0636839), delta_x (250.342),
              alpha_y (-0.0889775), beta_y (-0.00118110), delta_y (356.538)
#elif (ILI9341_ROTATION == ILI9341_ROTATION_90)
              alpha_x (-0.0885542), beta_x (0.0016532), delta_x (349.800),
              alpha_y (0.0007309), beta_y (0.06543699), delta_y (-15.290)
#elif (ILI9341_ROTATION == ILI9341_ROTATION_180)
              alpha_x (0.0006100), beta_x (0.0647828), delta_x (-13.634),
              alpha_y (0.0890609), beta_y (0.0001381), delta_y (-35.73)
#elif (ILI9341_ROTATION == ILI9341_ROTATION_270)
              alpha_x (0.0902216), beta_x (0.0006510), delta_x (-38.657),
              alpha_y (-0.0010005), beta_y (-0.0667030), delta_y (258.08)
#endif
        {
        }
    };

    struct TouchSamples {
        uint32_t sample_timer;
        std::array<Point<uint16_t>, MAX_SAMPLES> samples;
        uint8_t counter;

        // Functions
        constexpr TouchSamples () : sample_timer (0), counter (0) {}
        constexpr Point<uint16_t> average () const;
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
        TouchSamples ts;

        const Point<uint16_t> read_xy ();
        void get_raw_data ();
        Point<uint16_t> read_touch_point ();

      public:
        CalibrationPoint<uint16_t, 2> cp;
        CalibrationData<double> cd;
        // We start NORMAL operation by default
        TouchScreenOperationMode mode{ TouchScreenOperationMode::NORMAL };

        TouchPanel ();
        void init (const SPI_HandleTypeDef &hspi, IRQn_Type hirq);
        void IRQ_handler ();
        void run ();
        void calibrate (const GFX_Color::GFX &gfx);
        Point<uint16_t> get_touch_point () const;
        bool is_touched () const;
        void clear_touch();
    };
}

#endif /* INC_TOUCH_H_ */
