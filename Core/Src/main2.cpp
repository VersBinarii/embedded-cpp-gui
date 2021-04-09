/* Includes ------------------------------------------------------------------*/
#include <cstdio>
#include <array>
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

#include <bme_280.h>
#include <fonts/arialBlack_20ptFontInfo.h>
#include <fonts.h>
#include <touch.h>
#include "GFX_color.h"
#include "lcd.h"

#define BME280_ADDRESS 0x76

 BME::BME280 bme280;
 XPT2046::TouchPanel tp;

 void main2 (void) {
     tp.init (hspi1, EXTI2_IRQn);
     ILI9341::LCD lcd{ &hspi5 };
     Fonts::Font font{ arialBlack_20ptFontInfo };
     GFX_Color::GFX gfx{ lcd, font };

     float temperature;
     float pressure;
     float humidity;

     char msg[32];

     HAL_TIM_PWM_Start (&htim9, TIM_CHANNEL_2);
     __HAL_TIM_SET_COMPARE (&htim9, TIM_CHANNEL_2, 700);

     // BME::BME280_init (bme280, &hi2c1, BME280_ADDRESS);

     // XPT2046::init (tp, hspi1, EXTI2_IRQn);

     // XPT2046::calibrate (tp, gfx);

     lcd.clear_display (ILI9341::Color::GREEN);

     XPT2046::Point p;

     gfx.draw_string ("Dupa sraka", 20, 50, ILI9341::Color::PURPLE,
                      ILI9341::Color::WHITE, Fonts::FontBackground::COLOR);
     gfx.draw_string ("Ale jaja japierpapier", 20, 100, ILI9341::Color::PURPLE,
                      ILI9341::Color::WHITE,
                      Fonts::FontBackground::TRANSPARENT);

     /* Infinite loop */
     while (1) {
         //      BME::BME280_read_temp (bme280, temperature);
         //    BME::BME280_read_pressure (bme280, pressure);
         //   BME::BME280_read_humidity (bme280, humidity);
         //   sprintf (msg, "Temp: %.2f C", temperature);
         //   GFX_DrawString (0, 130, msg, ILI9341_RED);
         //  sprintf (msg, "Pressure: %.2f Pa", pressure);
         //   GFX_DrawString (0, 140, msg, ILI9341_RED);
         //  sprintf (msg, "Hum: %.2f%%", humidity);
         //    GFX_DrawString (0, 150, msg, ILI9341_RED);

          tp.run ();
          if (tp.is_touched ()) {
              p = tp.get_touch_point ();
              lcd.write_pixel (p.x, p.y, ILI9341::Color::WHITE);
          }
     }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == TOUCH_IRQ_Pin){
            tp.IRQ_handler ();
        }
}

