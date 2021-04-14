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
#include <touch.h>
#include <lcd.h>
#include <graphics/GFX_color.h>
#include <graphics/point.h>
#include <graphics/fonts.h>
#include <graphics/gui.h>


#define BME280_ADDRESS 0x76

 BME::BME280 bme280;
 XPT2046::TouchPanel tp;

 static int16_t brightnes = 550;
 const static auto increase () {
     brightnes += 25;
     __HAL_TIM_SET_COMPARE (&htim9, TIM_CHANNEL_2, brightnes);
     if (brightnes > 1000) {
         brightnes = 1000;
     }
 }

 const static auto decrease () {
		 brightnes -= 25;
		 __HAL_TIM_SET_COMPARE (&htim9, TIM_CHANNEL_2, brightnes);
		 if (brightnes <= 0){
			 brightnes = 5;
     }
 }

 void main2 () {
     tp.init (hspi1, EXTI2_IRQn);
     ILI9341::LCD lcd{ &hspi5 };
     const Fonts::Font font{ arialBlack_20ptFontInfo };
     const GFX_Color::GFX gfx{ lcd, font };
     GUI::Gui<uint16_t> gui{ tp };

     //  TIMER 2 PWM controlling the LCD back light
     HAL_TIM_PWM_Start (&htim9, TIM_CHANNEL_2);
     __HAL_TIM_SET_COMPARE (&htim9, TIM_CHANNEL_2, brightnes);

     // Timer 5 triggering every 1us to check for touch screen events
     HAL_TIM_OC_Start_IT(&htim5, TIM_CHANNEL_4);
     // BME::BME280_init (bme280, &hi2c1, BME280_ADDRESS);

     //tp.calibrate (gfx);

     lcd.clear_display (ILI9341::Color::BLACK);

     GUI::Button<uint16_t> btn_up (ILI9341::TFTWIDTH / 2 - GUI::BTN_WIDTH / 2,
                                   ILI9341::TFTHEIGHT / 2 - GUI::BTN_HEIGHT,
                                   GUI::BTN_WIDTH, GUI::BTN_HEIGHT, "+");

     btn_up.on_click (increase);

     GUI::Button<uint16_t> btn_down (
         ILI9341::TFTWIDTH / 2 - GUI::BTN_WIDTH / 2,
         ILI9341::TFTHEIGHT / 2 + 5, GUI::BTN_WIDTH, GUI::BTN_HEIGHT, "-");

     btn_down.on_click (decrease);

     gui.add_btn (btn_up);
     gui.add_btn (btn_down);
     /* Infinite loop */
     while (1) {
         //      BME::BME280_read_temp (bme280, temperature);
         //    BME::BME280_read_pressure (bme280, pressure);
         //   BME::BME280_read_humidity (bme280, humidity);

         if (tp.is_touched ()) {
             gui.process_touch_event ();
             tp.clear_touch();
             HAL_Delay(50);
              //  const Point<uint16_t> p = tp.get_touch_point ();
             //   lcd.write_pixel (p.x, p.y, ILI9341::Color::WHITE);
         }

         gui.draw (gfx);
     }
}


// EXTI interrupt handler on the TOUCH IRQ pin
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == TOUCH_IRQ_Pin){
            tp.IRQ_handler ();
        }
}

// TIM3 Timer interrupt for checking the touch screen input
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM5){
		tp.run();
	}
}

