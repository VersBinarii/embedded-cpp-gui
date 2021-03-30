/*
 * touch.cpp
 *
 *  Created on: Mar 25, 2021
 *      Author: kris
 */
#include <touch.h>

namespace XPT2046 {
    constexpr static uint8_t CHANNEL_SETTINGS_X = 0b10010000;
    constexpr static uint8_t CHANNEL_SETTINGS_Y = 0b11010000;
    constexpr static uint8_t SAMPLE_INTERVAL = 5;

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

    constexpr Point TouchSamples::average () {
        Point average_point;
        for (uint8_t i = 0; i < samples.max_size (); i++) {
            average_point.x += this->samples[i].x;
            average_point.y += this->samples[i].y;
        }
        average_point.x = average_point.x / MAX_SAMPLES;
        average_point.y = average_point.y / MAX_SAMPLES;
        return average_point;
    }

    static void cs_set (GPIO_PinState state) {
        HAL_GPIO_WritePin (TOUCH_CS_GPIO_Port, TOUCH_CS_Pin, state);
    }

    static void get_raw_data (TouchPanel &tp) {
        cs_set (GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive (tp.touch_spi, tp.tx_buffer, tp.rx_buffer, 5,
                                 1000);
        cs_set (GPIO_PIN_SET);
    }

    static Point read_xy (TouchPanel &tp) {
        Point point;
        get_raw_data (tp);
        point.x = (uint16_t) ((tp.rx_buffer[1] << 8) | tp.rx_buffer[2]);
        point.y = (uint16_t) ((tp.rx_buffer[3] << 8) | tp.rx_buffer[4]);
        return point;
    }

    static Point read_touch_point (TouchPanel &tp) {
        Point calibrated_point;
        Point raw_point = read_xy (tp);

        if (tp.mode == TouchScreenOperationMode::NORMAL) {
            calibrated_point.x = tp.cd.alpha_x * raw_point.x
                                 + tp.cd.beta_x * raw_point.y + tp.cd.delta_x;
            calibrated_point.y = tp.cd.alpha_y * raw_point.x
                                 + tp.cd.beta_y * raw_point.y + tp.cd.delta_y;
        } else {
            /*
             * We're running calibration so just return raw
             * point measurements without compensation
             */
            calibrated_point.x = raw_point.x;
            calibrated_point.y = raw_point.y;
        }
        return calibrated_point;
    }

    bool is_touched (TouchPanel &tp) {
        return tp.state == TouchScreenState::TOUCHED;
    }

    constexpr Point get_touch_point (TouchPanel &tp) {
        return tp.ts.average ();
    }

    void run (TouchPanel &tp) {
        switch (tp.state) {
        case TouchScreenState::IDLE:
            break;

        case TouchScreenState::PRESAMPLING: {
            if ((HAL_GetTick () - tp.ts.sample_timer) > SAMPLE_INTERVAL) {
                Point point_sample = read_touch_point (tp);
                tp.ts.samples[tp.ts.counter++] = point_sample;
                if (tp.ts.counter == MAX_SAMPLES) {
                    tp.ts.counter = 0;
                    tp.state = TouchScreenState::TOUCHED;
                }

                if (GPIO_PIN_SET
                    == HAL_GPIO_ReadPin (TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin)) {
                    tp.state = TouchScreenState::RELEASED;
                }
                tp.ts.sample_timer = HAL_GetTick ();
            }
        } break;

        case TouchScreenState::TOUCHED:
            if ((HAL_GetTick () - tp.ts.sample_timer) > SAMPLE_INTERVAL) {
                Point point_sample = read_touch_point (tp);
                tp.ts.samples[tp.ts.counter++] = point_sample;
                tp.ts.counter %= MAX_SAMPLES;

                if (GPIO_PIN_SET
                    == HAL_GPIO_ReadPin (TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin)) {
                    tp.state = TouchScreenState::RELEASED;
                }
                tp.ts.sample_timer = HAL_GetTick ();
            }
            break;
        case TouchScreenState::RELEASED:
            tp.state = TouchScreenState::IDLE;
            tp.ts.counter = 0;
            while (HAL_NVIC_GetPendingIRQ (tp.touch_irq)) {
                __HAL_GPIO_EXTI_CLEAR_IT (TOUCH_IRQ_Pin);
                HAL_NVIC_ClearPendingIRQ (tp.touch_irq);
            }
            HAL_NVIC_EnableIRQ (tp.touch_irq);
            break;
        }
    }

    void IRQ_handler (TouchPanel &tp) {
        HAL_NVIC_DisableIRQ (tp.touch_irq);
        tp.state = TouchScreenState::PRESAMPLING;
    }

    void init (TouchPanel &tp, SPI_HandleTypeDef &hspi, IRQn_Type hirq) {

#if (ILI9341_ROTATION == ILI9341_ROTATION_0)
        constexpr CalibrationData cd = { -0.0009337, -0.0636839,  250.342,
                                         -0.0889775, -0.00118110, 356.538 };
#elif (ILI9341_ROTATION == ILI9341_ROTATION_90)
        constexpr CalibrationData cd = { -0.0885542, 0.0016532,  349.800,
                                         0.0007309,  0.06543699, -15.290 };
#elif (ILI9341_ROTATION == ILI9341_ROTATION_180)
        constexpr CalibrationData cd
            = { 0.0006100, 0.0647828, -13.634, 0.0890609, 0.0001381, -35.73 };
#elif (ILI9341_ROTATION == ILI9341_ROTATION_270)
        constexpr CalibrationData cd = { 0.0902216,  0.0006510,  -38.657,
                                         -0.0010005, -0.0667030, 258.08 };
#endif

    tp.touch_spi = &hspi;
    tp.touch_irq = hirq;

    // Set CS pin
    cs_set (GPIO_PIN_SET);

    tp.tx_buffer[0] = 0x80; /* Command to clear settings */
    get_raw_data (tp);      /* Send it */
    HAL_Delay (1);

    // Pad the send buffer
    tp.tx_buffer[0] = (CHANNEL_SETTINGS_X >> 3);
    tp.tx_buffer[1] = (uint8_t) (CHANNEL_SETTINGS_X << 5);
    tp.tx_buffer[2] = (CHANNEL_SETTINGS_Y >> 3);
    tp.tx_buffer[3] = (uint8_t) (CHANNEL_SETTINGS_Y << 5);
    tp.tx_buffer[4] = 0;

    // Initialize Calibration data struct based on screen location
    tp.cd = cd;
    }


    //
    // Draw X in circle as calibration point indicator
    //
    void calibration_draw_point (GFX_Color::GFX &gfx, uint16_t x, uint16_t y) {
        GFX_Color::draw_line (gfx, x - 4, y, x + 4, y, ILI9341::Color::WHITE);
        GFX_Color::draw_line (gfx, x, y - 4, x, y + 4, ILI9341::Color::WHITE);
        GFX_Color::draw_rectangle (gfx, x - 5, y - 5, 11, 11,
                                   ILI9341::Color::WHITE);
    }

    //
    // Calculate new Calibration data - mathematics with well known 3-point
    // calibration
    //
    static void calibration_calculate (TouchPanel &tp) {
        int32_t delta = (tp.cp_raw.a[0] - tp.cp_raw.c[0])
                            * (tp.cp_raw.b[1] - tp.cp_raw.c[1])
                        - (tp.cp_raw.b[0] - tp.cp_raw.c[0])
                              * (tp.cp_raw.a[1] - tp.cp_raw.c[1]);

        tp.cd.alpha_x
            = (float)((calibration_point.a[0] - calibration_point.c[0])
                          * (tp.cp_raw.b[1] - tp.cp_raw.c[1])
                      - (calibration_point.b[0] - calibration_point.c[0])
                            * (tp.cp_raw.a[1] - tp.cp_raw.c[1]))
              / delta;

        tp.cd.beta_x
            = (float)((tp.cp_raw.a[0] - tp.cp_raw.c[0])
                          * (calibration_point.b[0] - calibration_point.c[0])
                      - (tp.cp_raw.b[0] - tp.cp_raw.c[0])
                            * (calibration_point.a[0]
                               - calibration_point.c[0]))
              / delta;

        tp.cd.delta_x = ((float)calibration_point.a[0]
                             * (tp.cp_raw.b[0] * tp.cp_raw.c[1]
                                - tp.cp_raw.c[0] * tp.cp_raw.b[1])
                         - (float)calibration_point.b[0]
                               * (tp.cp_raw.a[0] * tp.cp_raw.c[1]
                                  - tp.cp_raw.c[0] * tp.cp_raw.a[1])
                         + (float)calibration_point.c[0]
                               * (tp.cp_raw.a[0] * tp.cp_raw.b[1]
                                  - tp.cp_raw.b[0] * tp.cp_raw.a[1]))
                        / delta;

        tp.cd.alpha_y
            = (float)((calibration_point.a[1] - calibration_point.c[1])
                          * (tp.cp_raw.b[1] - tp.cp_raw.c[1])
                      - (calibration_point.b[1] - calibration_point.c[1])
                            * (tp.cp_raw.a[1] - tp.cp_raw.c[1]))
              / delta;

        tp.cd.beta_y
            = (float)((tp.cp_raw.a[0] - tp.cp_raw.c[0])
                          * (calibration_point.b[1] - calibration_point.c[1])
                      - (tp.cp_raw.b[0] - tp.cp_raw.c[0])
                            * (calibration_point.a[1]
                               - calibration_point.c[1]))
              / delta;

        tp.cd.delta_y = ((float)calibration_point.a[1]
                             * (tp.cp_raw.b[0] * tp.cp_raw.c[1]
                                - tp.cp_raw.c[0] * tp.cp_raw.b[1])
                         - (float)calibration_point.b[1]
                               * (tp.cp_raw.a[0] * tp.cp_raw.c[1]
                                  - tp.cp_raw.c[0] * tp.cp_raw.a[1])
                         + (float)calibration_point.c[1]
                               * (tp.cp_raw.a[0] * tp.cp_raw.b[1]
                                  - tp.cp_raw.b[0] * tp.cp_raw.a[1]))
                        / delta;
    }

    void calibrate (TouchPanel &tp, GFX_Color::GFX &gfx) {
        auto calibration_count = 0;
        Point new_a, new_b, new_c;

        // Prepare the screen for points
        ILI9341::clear_display (*gfx.lcd, ILI9341::Color::BLACK);

        // Set correct state to fetch raw data from touch controller
        tp.mode = TouchScreenOperationMode::CALIBRATION;

        while (calibration_count < 4) {

            // We must run our state machine to capture user input
            run (tp);
            switch (calibration_count) {
            case 0:
                calibration_draw_point (gfx, calibration_point.a[0],
                                        calibration_point.a[1]);
                if (tp.state == TouchScreenState::TOUCHED) {
                    new_a = get_touch_point (tp);
                }
                if (tp.state == TouchScreenState::RELEASED) {
                    HAL_Delay (200);
                    calibration_count++;
                }
                break;

            case 1:
                GFX_Color::draw_fill_rectangle (gfx, tp.cp_raw.a[0] - 6,
                                                tp.cp_raw.a[1] - 6, 13, 13,
                                                ILI9341::Color::BLACK);
                calibration_draw_point (gfx, calibration_point.b[0],
                                        calibration_point.b[1]);
                if (tp.state == TouchScreenState::TOUCHED) {
                    new_b = get_touch_point (tp);
                }
                if (tp.state == TouchScreenState::RELEASED) {
                    HAL_Delay (200);
                    calibration_count++;
                }
                break;
            case 2:
                GFX_Color::draw_fill_rectangle (gfx, tp.cp_raw.b[0] - 6,
                                                tp.cp_raw.b[1] - 6, 13, 13,
                                                ILI9341::Color::BLACK);
                calibration_draw_point (gfx, calibration_point.c[0],
                                        calibration_point.c[1]);
                if (tp.state == TouchScreenState::TOUCHED) {
                    new_c = get_touch_point (tp);
                }
                if (tp.state == TouchScreenState::RELEASED) {
                    HAL_Delay (200);
                    calibration_count++;
                }
                break;

            case 3:
                GFX_Color::draw_fill_rectangle (gfx, tp.cp_raw.c[0] - 6,
                                                tp.cp_raw.c[1] - 6, 13, 13,
                                                ILI9341::Color::BLACK);
                // Create new calibration point from the captured samples
                CalibrationPoint new_calibration_point ({ new_a.x, new_a.y },
                                                        { new_b.x, new_b.y },
                                                        { new_c.x, new_c.y });
                tp.cp_raw = std::move (new_calibration_point);
                // and then re-caculate calibration
                calibration_calculate (tp);
                calibration_count++;

                break;
            }
        }
        tp.mode = TouchScreenOperationMode::NORMAL;
    }
}
