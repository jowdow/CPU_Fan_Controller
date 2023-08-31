/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32g0xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
#include "tim.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
	/* USER CODE BEGIN NonMaskableInt_IRQn 0 */

	/* USER CODE END NonMaskableInt_IRQn 0 */
	/* USER CODE BEGIN NonMaskableInt_IRQn 1 */
	while (1) {
	}
	/* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
	/* USER CODE BEGIN HardFault_IRQn 0 */

	/* USER CODE END HardFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_HardFault_IRQn 0 */
		/* USER CODE END W1_HardFault_IRQn 0 */
	}
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void) {
	/* USER CODE BEGIN SVC_IRQn 0 */

	/* USER CODE END SVC_IRQn 0 */
	/* USER CODE BEGIN SVC_IRQn 1 */

	/* USER CODE END SVC_IRQn 1 */
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void) {
	/* USER CODE BEGIN PendSV_IRQn 0 */

	/* USER CODE END PendSV_IRQn 0 */
	/* USER CODE BEGIN PendSV_IRQn 1 */

	/* USER CODE END PendSV_IRQn 1 */
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) {
	/* USER CODE BEGIN SysTick_IRQn 0 */

	/* USER CODE END SysTick_IRQn 0 */
	HAL_IncTick();
	/* USER CODE BEGIN SysTick_IRQn 1 */

	if (isFanSpinning) {	// JD - If fan is spinning
		// JD - Doing the operations needed for a ADC value and saving the value
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADCValue_u32 = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);

		// JD - If ADC value has changed by more than 10 change PWM period
		if (ADCValue_u32 > (previousADCValue_u32 + 10) || ADCValue_u32 < (previousADCValue_u32 - 10)) {

			if (HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1) != HAL_OK) {// JD - Stopping PWM
				/* PWM Generation Error */
				Error_Handler();
			}

			// JD - ((adcValue_u32 - inputMin) * (scaledMax - scaledMin)) / (inputMax - inputMin);
			htim1.Instance->CCR1 = ((ADCValue_u32 - 90) * (640 - 0)) / (4095 - 90); // JD - Scaling and setting the new PWM period
			// JD - On page 418 of the reference manual it goes into details the CCR1 register
			// JD - CCR1 is the actual period value and PWM mode is explained in detail on page 364 on the reference manual

			if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK) {// JD - Starting PWM
				/* PWM Generation Error */
				Error_Handler();
			}

			previousADCValue_u32 = ADCValue_u32;
		}
	}
	/* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles EXTI line 4 to 15 interrupts.
 */
void EXTI4_15_IRQHandler(void) {
	/* USER CODE BEGIN EXTI4_15_IRQn 0 */

	/* USER CODE END EXTI4_15_IRQn 0 */
	if (LL_EXTI_IsActiveRisingFlag_0_31(LL_EXTI_LINE_12) != RESET) {
		LL_EXTI_ClearRisingFlag_0_31(LL_EXTI_LINE_12);
		/* USER CODE BEGIN LL_EXTI_LINE_12_RISING */

		if (LL_GPIO_IsInputPinSet(Button_Input_GPIO_Port, Button_Input_Pin)) {// JD - If the button is pressed
			if (isFanSpinning == 0) {	// JD - If the fan is not spinning
				if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK) {// JD - Starting PWM
					/* PWM Generation Error */
					Error_Handler();
				}
				isFanSpinning = 1;
			} else { // JD - If the fan is spinning
				if (HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1) != HAL_OK) {// JD - Stopping PWM
					/* PWM Generation Error */
					Error_Handler();
				}
				isFanSpinning = 0;
			}
		}
		/* USER CODE END LL_EXTI_LINE_12_RISING */
	}
	/* USER CODE BEGIN EXTI4_15_IRQn 1 */

	/* USER CODE END EXTI4_15_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
