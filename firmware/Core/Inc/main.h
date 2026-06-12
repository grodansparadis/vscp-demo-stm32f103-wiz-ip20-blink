/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <string.h>
#include <blinky.h>
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
void
Error_Handler(void);

/* USER CODE BEGIN EFP */

/**
 * @brief Set defaults for the Context Defaults object
 * @param pctx Pointer to context
 */
void
setContextDefaults(ctx_t *pctx);

/*!
  * @brief  Enter command mode on the WIZ-IP20 module.
  * @retval int 0 if successful, -1 if failed.
  */
int
goCommandMode(void);

/*!
  * @brief  Send a command to the WIZ-IP20 module and wait for a response.
  * @param  cmd: The command to send (null-terminated string including CRLF at end).
  * @param  response_buf: Buffer to store the response (must be pre-allocated).
  * @param  response_buf_size: Size of the response buffer.
  * @param  timeout_ms: Timeout in milliseconds to wait for the response.
  * @retval int Number of bytes received in the response, or -1 if an error occurred.
*/
int sendCommand(const char *cmd, char *response_buf, size_t response_buf_size, uint16_t timeout_ms);

/**
 * @fn validate_user
 * @brief Validate user
 *
 * @param user Username to check
 * @param password Password to check
 * @return True if user is valid, False if not.
 */
int
validate_user(const char *user, const char *password);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin       GPIO_PIN_13
#define LED_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
