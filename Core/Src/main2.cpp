/* Includes ------------------------------------------------------------------*/
#include <cstdio>
#include <array>
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <bme_280.h>
#include <touch.h>
#include "GFX_color.h"
#include "fonts/fonts.h"
#include "lcd.h"
#include "logo.h"
#include "out.h"

#define BME280_ADDRESS 0x76

BME::BME280 bme280;
ILI9341::LCD lcd;
GFX_Color::GFX gfx;
XPT2046::TouchPanel tp;


void main2(void){
  /* USER CODE BEGIN 1 */
  float temperature;
  float pressure;
  float humidity;

  char msg[32];
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_2);

  ILI9341::init (lcd, &hspi5);

  GFX_Color::init (gfx, lcd);
  ILI9341::draw_image (lcd, 0, 5, logo1, 240, 72);
  GFX_Color::set_font (gfx, font_8x5);
  // BME::BME280_init (bme280, &hi2c1, BME280_ADDRESS);

  //XPT2046::init (tp, hspi1, EXTI2_IRQn);

  //XPT2046::calibrate (tp, gfx);

  //ILI9341::clear_display (lcd, ILI9341::Color::PURPLE);

  //XPT2046::Point p;



  /* Infinite loop */
  while (1) {
      //    BME::BME280_read_temp (bme280, temperature);
      //   BME::BME280_read_pressure (bme280, pressure);
      //  BME::BME280_read_humidity (bme280, humidity);
      // sprintf (msg, "Temp: %.2f C", temperature);
      // GFX_DrawString (0, 130, msg, ILI9341_RED);
      // sprintf (msg, "Pressure: %.2f Pa", pressure);
      //  GFX_DrawString (0, 140, msg, ILI9341_RED);
      // sprintf (msg, "Hum: %.2f%%", humidity);
      //  GFX_DrawString (0, 150, msg, ILI9341_RED);
      //  ILI9341_ClearDisplay (ILI9341_PURPLE);

	  uint16_t i;

	  for(i = 0; i < 1000; i+=25)
	  {
		  __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_2, i);
		  HAL_Delay(300);
	  }

	  for(i = 1000; i > 0; i-=25)
	  {
		  __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_2, i);
		  HAL_Delay(300);
	  }


     // XPT2046::run (tp);
     // if (XPT2046::is_touched (tp)) {
     //     p = XPT2046::get_touch_point (tp);
      //    ILI9341::write_pixel (lcd, p.x, p.y, ILI9341::Color::WHITE);
      //}
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == TOUCH_IRQ_Pin){
		XPT2046::IRQ_handler(tp);
	}
}

