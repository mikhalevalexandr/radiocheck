/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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
extern uint16_t ADC[2];//buffer for reading battery voltage
extern float mcuVoltage;// MCU supply Voltage
extern float batteryVoltage;// Battery voltage 
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CE1_Pin GPIO_PIN_3
#define CE1_GPIO_Port GPIOA
#define CSN1_Pin GPIO_PIN_4
#define CSN1_GPIO_Port GPIOA
#define IRQ1_Pin GPIO_PIN_0
#define IRQ1_GPIO_Port GPIOB
#define IRQ1_EXTI_IRQn EXTI0_IRQn
#define LD2_Pin GPIO_PIN_1
#define LD2_GPIO_Port GPIOB
#define RELAY_Pin GPIO_PIN_15
#define RELAY_GPIO_Port GPIOA
#define SHDN_Pin GPIO_PIN_4
#define SHDN_GPIO_Port GPIOB
#define LEDR_Pin GPIO_PIN_5
#define LEDR_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define ADC_REFERENCE_VOLTAGE 1.2/* Voltage of Vrefint - STM32 internal voltage reference 
																		(look stm datasheet) is around 1.2 Volts*/
#define ADC_MAX               0xFFF /* It is maximum value in 12 bit adc. It corresponds to STM power supply voltage 
																			 (VDDA or Vref+. It depends on microcontroller)*/
#define VREFINT_CAL 					1638.00/* Raw data (ADC code for 1.2 Volts)acquired at a temperature of 30 °C (± 5 °C),
																				VDDA = VREF+ = 3.0 V (± 10 mV)*/
#define VOLTAGE_DEVIDER 			2 /*Voltage devider before ADC. For example, battery voltage is 5V, there is 2.5V on stm ADC,
because of resistors voltage devider on the circuit board*/

#define MINIMUM_BATTERY_VOLTAGE 			3.2 /*Voltage devider before ADC. For example battery voltage is 5V, there is 2.5V on stm ADC */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
