/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void main2(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BLUE_LED_Pin GPIO_PIN_13
#define BLUE_LED_GPIO_Port GPIOC
#define TOUCH_IRQ_Pin GPIO_PIN_2
#define TOUCH_IRQ_GPIO_Port GPIOA
#define TOUCH_IRQ_EXTI_IRQn EXTI2_IRQn
#define BACKLIGHT_PWM_Pin GPIO_PIN_3
#define BACKLIGHT_PWM_GPIO_Port GPIOA
#define TOUCH_CS_Pin GPIO_PIN_4
#define TOUCH_CS_GPIO_Port GPIOA
#define TOUCH_CLK_Pin GPIO_PIN_5
#define TOUCH_CLK_GPIO_Port GPIOA
#define TOUCH_MISO_Pin GPIO_PIN_6
#define TOUCH_MISO_GPIO_Port GPIOA
#define TOUCH_MOSI_Pin GPIO_PIN_7
#define TOUCH_MOSI_GPIO_Port GPIOA
#define LCD_CLK_Pin GPIO_PIN_0
#define LCD_CLK_GPIO_Port GPIOB
#define LCD_DC_Pin GPIO_PIN_1
#define LCD_DC_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_2
#define LCD_CS_GPIO_Port GPIOB
#define ZERO_CROSS_DETECT_Pin GPIO_PIN_14
#define ZERO_CROSS_DETECT_GPIO_Port GPIOB
#define ZERO_CROSS_DETECT_EXTI_IRQn EXTI15_10_IRQn
#define TRIAK_CTRL_Pin GPIO_PIN_15
#define TRIAK_CTRL_GPIO_Port GPIOB
#define LCD_MOSI_Pin GPIO_PIN_10
#define LCD_MOSI_GPIO_Port GPIOA
#define BMP_SCL_Pin GPIO_PIN_6
#define BMP_SCL_GPIO_Port GPIOB
#define BMP_SDA_Pin GPIO_PIN_7
#define BMP_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
