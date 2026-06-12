/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern volatile uint32_t tim3_ovf;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_TIM2_Init(void);
void MX_TIM3_Init(void);

/* USER CODE BEGIN Prototypes */

/**
 * @brief  Read the 32-bit 1 µs free-running counter.
 *
 * TIM3 (16-bit hardware) is extended to 32 bits via a software overflow
 * counter (tim3_ovf) incremented in the TIM3 update ISR.
 * The read sequence is overflow-safe: if an overflow occurs between
 * reading hi and lo, the second hi read will differ and lo is re-read.
 *
 * Range:  ~4 294 s before wrap (32-bit @ 1 µs/tick).
 *
 * @return Microseconds since TIM3 was started.
 */
static inline uint32_t usec_now(void)
{
  uint32_t hi, lo;
  do {
    hi = tim3_ovf;
    lo = TIM3->CNT;
  } while (tim3_ovf != hi); /* retry if overflow hit between reads */
  return (hi << 16) | lo;
}

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

