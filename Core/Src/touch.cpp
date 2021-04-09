/*
 * touch.cpp
 *
 *  Created on: Mar 25, 2021
 *      Author: kris
 */
#include <touch.h>

namespace XPT2046 {
    constexpr static uint8_t SAMPLE_INTERVAL = 5;
    constexpr static uint8_t CHANNEL_SETTINGS_X = 0b10010000;
    constexpr static uint8_t CHANNEL_SETTINGS_Y = 0b11010000;

#if (ILI9341_ROTATION == ILI9341_ROTATION_0 \
     || ILI9341_ROTATION == ILI9341_ROTATION_180)
    constexpr static CalibrationPoint calibration_point ({ 10, 10 },
                                                         { 80, 280 },
                                                         { 200, 170 });
#elif (ILI9341_ROTATION == ILI9341_ROTATION_90 \
       || ILI9341_ROTATION == ILI9341_ROTATION_270)
    constexpr static CalibrationPoint calibration_point ({ 20, 25 },
                                                         { 160, 220 },
                                                         { 300, 110 });
#endif

#if (ILI9341_ROTATION == ILI9341_ROTATION_0)
    constexpr static CalibrationData calib_data{ -0.0009337,  -0.0636839,
                                                 250.342,     -0.0889775,
                                                 -0.00118110, 356.538 };
#elif (ILI9341_ROTATION == ILI9341_ROTATION_90)
    constexpr static CalibrationData calib_data{ -0.0885542, 0.0016532,
                                                 349.800,    0.0007309,
                                                 0.06543699, -15.290 };
#elif (ILI9341_ROTATION == ILI9341_ROTATION_180)
    constexpr static CalibrationData calib_data{
        0.0006100, 0.0647828, -13.634, 0.0890609, 0.0001381, -35.73
    };
#elif (ILI9341_ROTATION == ILI9341_ROTATION_270)
    constexpr static CalibrationData calib_data{ 0.0902216,  0.0006510,
                                                 -38.657,    -0.0010005,
                                                 -0.0667030, 258.08 };
#endif

    constexpr Point TouchSamples::average () const {
        uint16_t x = 0, y = 0;
        for (uint8_t i = 0; i < samples.max_size (); i++) {
            x += this->samples[i].x;
            y += this->samples[i].y;
        }
        x = x / MAX_SAMPLES;
        y = y / MAX_SAMPLES;
        return Point (x, y);
    }

    static void cs_set (GPIO_PinState state) {
        HAL_GPIO_WritePin (TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, state);
    }

    void TouchPanel::get_raw_data () {
        cs_set (GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive (const_cast<SPI_HandleTypeDef *> (this->spi),
                                 this->tx_buffer, this->rx_buffer, 5, 1000);
        cs_set (GPIO_PIN_SET);
    }

    const Point TouchPanel::read_xy () {
        this->get_raw_data ();
        const uint16_t x = static_cast<uint16_t> ((this->rx_buffer[1] << 8)
                                                  | this->rx_buffer[2]);
        const uint16_t y = static_cast<uint16_t> ((this->rx_buffer[3] << 8)
                                                  | this->rx_buffer[4]);
        return Point{ x, y };
    }

    Point TouchPanel::read_touch_point () {
        const Point raw_point = this->read_xy ();
        uint16_t x, y;
        if (this->mode == TouchScreenOperationMode::NORMAL) {
            x = this->cd.alpha_x * raw_point.x + this->cd.beta_x * raw_point.y
                + this->cd.delta_x;
            y = this->cd.alpha_y * raw_point.x + this->cd.beta_y * raw_point.y
                + this->cd.delta_y;
        } else {
            /*
             * We're running calibration so just return raw
             * point measurements without compensation
             */
            x = raw_point.x;
            y = raw_point.y;
        }
        return Point (x, y);
    }

    bool TouchPanel::is_touched () {
        return this->state == TouchScreenState::TOUCHED;
    }

    Point TouchPanel::get_touch_point () const { return this->ts.average (); }

    void TouchPanel::run () {
        switch (this->state) {
        case TouchScreenState::IDLE:
            break;

        case TouchScreenState::PRESAMPLING: {
            if ((HAL_GetTick () - this->ts.sample_timer) > SAMPLE_INTERVAL) {
                const Point point_sample = this->read_touch_point ();
                this->ts.samples[this->ts.counter++] = point_sample;
                if (this->ts.counter == MAX_SAMPLES) {
                    this->ts.counter = 0;
                    this->state = TouchScreenState::TOUCHED;
                }

                if (GPIO_PIN_SET
                    == HAL_GPIO_ReadPin (TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin)) {
                    this->state = TouchScreenState::RELEASED;
                }
                this->ts.sample_timer = HAL_GetTick ();
            }
        } break;

        case TouchScreenState::TOUCHED:
            if ((HAL_GetTick () - this->ts.sample_timer) > SAMPLE_INTERVAL) {
                const Point point_sample = this->read_touch_point ();
                this->ts.samples[this->ts.counter++] = point_sample;
                this->ts.counter %= MAX_SAMPLES;

                if (GPIO_PIN_SET
                    == HAL_GPIO_ReadPin (TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin)) {
                    this->state = TouchScreenState::RELEASED;
                }
                this->ts.sample_timer = HAL_GetTick ();
            }
            break;
        case TouchScreenState::RELEASED:
            this->state = TouchScreenState::IDLE;
            this->ts.counter = 0;
            while (HAL_NVIC_GetPendingIRQ (this->touch_irq)) {
                __HAL_GPIO_EXTI_CLEAR_IT (TOUCH_IRQ_Pin);
                HAL_NVIC_ClearPendingIRQ (this->touch_irq);
            }
            HAL_NVIC_EnableIRQ (this->touch_irq);
            break;
        }
   }

    void TouchPanel::IRQ_handler () {
        HAL_NVIC_DisableIRQ (this->touch_irq);
        this->state = TouchScreenState::PRESAMPLING;
    }

    TouchPanel::TouchPanel () : spi{ nullptr }, cd{ std::move (calib_data) } {}
    void TouchPanel::init (const SPI_HandleTypeDef &hspi, IRQn_Type hirq) {

        this->spi = &hspi;
        this->touch_irq = hirq;
        // Set CS pin
        cs_set (GPIO_PIN_SET);

        this->tx_buffer[0] = 0x80; /* Command to clear settings */
        this->get_raw_data ();     /* Send it */
        HAL_Delay (1);

        // Pad the send buffer
        this->tx_buffer[0] = (CHANNEL_SETTINGS_X >> 3);
        this->tx_buffer[1] = (uint8_t) (CHANNEL_SETTINGS_X << 5);
        this->tx_buffer[2] = (CHANNEL_SETTINGS_Y >> 3);
        this->tx_buffer[3] = (uint8_t) (CHANNEL_SETTINGS_Y << 5);
        this->tx_buffer[4] = 0;
    }

    //
    // Draw X in circle as calibration point indicator
    //
    static void calibration_draw_point (const GFX_Color::GFX &gfx, uint16_t x,
                                        uint16_t y) {
        gfx.draw_line (x - 4, y, x + 4, y, ILI9341::Color::WHITE);
        gfx.draw_line (x, y - 4, x, y + 4, ILI9341::Color::WHITE);
        gfx.draw_rectangle (x - 5, y - 5, 11, 11, ILI9341::Color::WHITE);
    }

    //
    // Calculate new Calibration data - mathematics with well known 3-point
    // calibration
    //
    static void calibration_calculate (TouchPanel &tp) {
        const int32_t delta = (tp.cp_raw.a[0] - tp.cp_raw.c[0])
                                  * (tp.cp_raw.b[1] - tp.cp_raw.c[1])
                              - (tp.cp_raw.b[0] - tp.cp_raw.c[0])
                                    * (tp.cp_raw.a[1] - tp.cp_raw.c[1]);

        const double alpha_x
            = static_cast<float> (
                  (calibration_point.a[0] - calibration_point.c[0])
                      * (tp.cp_raw.b[1] - tp.cp_raw.c[1])
                  - (calibration_point.b[0] - calibration_point.c[0])
                        * (tp.cp_raw.a[1] - tp.cp_raw.c[1]))
              / delta;

        const double beta_x
            = static_cast<float> (
                  (tp.cp_raw.a[0] - tp.cp_raw.c[0])
                      * (calibration_point.b[0] - calibration_point.c[0])
                  - (tp.cp_raw.b[0] - tp.cp_raw.c[0])
                        * (calibration_point.a[0] - calibration_point.c[0]))
              / delta;

        const double delta_x = (static_cast<float> (calibration_point.a[0])
                                    * (tp.cp_raw.b[0] * tp.cp_raw.c[1]
                                       - tp.cp_raw.c[0] * tp.cp_raw.b[1])
                                - static_cast<float> (calibration_point.b[0])
                                      * (tp.cp_raw.a[0] * tp.cp_raw.c[1]
                                         - tp.cp_raw.c[0] * tp.cp_raw.a[1])
                                + static_cast<float> (calibration_point.c[0])
                                      * (tp.cp_raw.a[0] * tp.cp_raw.b[1]
                                         - tp.cp_raw.b[0] * tp.cp_raw.a[1]))
                               / delta;

        const double alpha_y
            = static_cast<float> (
                  (calibration_point.a[1] - calibration_point.c[1])
                      * (tp.cp_raw.b[1] - tp.cp_raw.c[1])
                  - (calibration_point.b[1] - calibration_point.c[1])
                        * (tp.cp_raw.a[1] - tp.cp_raw.c[1]))
              / delta;

        const double beta_y
            = static_cast<float> (
                  (tp.cp_raw.a[0] - tp.cp_raw.c[0])
                      * (calibration_point.b[1] - calibration_point.c[1])
                  - (tp.cp_raw.b[0] - tp.cp_raw.c[0])
                        * (calibration_point.a[1] - calibration_point.c[1]))
              / delta;

        const double delta_y = (static_cast<float> (calibration_point.a[1])
                                    * (tp.cp_raw.b[0] * tp.cp_raw.c[1]
                                       - tp.cp_raw.c[0] * tp.cp_raw.b[1])
                                - static_cast<float> (calibration_point.b[1])
                                      * (tp.cp_raw.a[0] * tp.cp_raw.c[1]
                                         - tp.cp_raw.c[0] * tp.cp_raw.a[1])
                                + static_cast<float> (calibration_point.c[1])
                                      * (tp.cp_raw.a[0] * tp.cp_raw.b[1]
                                         - tp.cp_raw.b[0] * tp.cp_raw.a[1]))
                               / delta;
        tp.cd = CalibrationData (alpha_x, beta_x, delta_x, alpha_y, beta_y,
                                 delta_y);
    }

    void TouchPanel::calibrate (const GFX_Color::GFX &gfx) {
        auto calibration_count = 0;
        Point new_a, new_b, new_c;

        // Prepare the screen for points
        gfx.lcd->clear_display (ILI9341::Color::BLACK);

        // Set correct state to fetch raw data from touch controller
        this->mode = TouchScreenOperationMode::CALIBRATION;

        while (calibration_count < 4) {

            // We must run our state machine to capture user input
            this->run ();
            switch (calibration_count) {
            case 0:
                calibration_draw_point (gfx, calibration_point.a[0],
                                        calibration_point.a[1]);
                if (this->state == TouchScreenState::TOUCHED) {
                    new_a = this->get_touch_point ();
                }
                if (this->state == TouchScreenState::RELEASED) {
                    HAL_Delay (200);
                    calibration_count++;
                }
                break;

            case 1:
                calibration_draw_point (gfx, calibration_point.b[0],
                                        calibration_point.b[1]);
                if (this->state == TouchScreenState::TOUCHED) {
                    new_b = this->get_touch_point ();
                }
                if (this->state == TouchScreenState::RELEASED) {
                    HAL_Delay (200);
                    calibration_count++;
                }
                break;
            case 2:
                calibration_draw_point (gfx, calibration_point.c[0],
                                        calibration_point.c[1]);
                if (this->state == TouchScreenState::TOUCHED) {
                    new_c = this->get_touch_point ();
                }
                if (this->state == TouchScreenState::RELEASED) {
                    HAL_Delay (200);
                    calibration_count++;
                }
                break;

            case 3:
                // Create new calibration point from the captured samples
                CalibrationPoint new_calibration_point ({ new_a.x, new_a.y },
                                                        { new_b.x, new_b.y },
                                                        { new_c.x, new_c.y });
                this->cp_raw = std::move (new_calibration_point);
                // and then re-caculate calibration
                calibration_calculate (*this);
                calibration_count++;

                break;
            }
        }
        this->mode = TouchScreenOperationMode::NORMAL;
    }
}
