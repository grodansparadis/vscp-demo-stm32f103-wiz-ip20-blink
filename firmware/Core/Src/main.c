/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum {
  VSCP_STATE_DISCONNECTED = 0, /**< Waiting for a client to connect    */
  VSCP_STATE_CONNECTED,        /**< Client connected, processing cmds  */
} vscp_state_t;

typedef enum {
  VSCP_SUBSTATE_POLL = 0, /**< Polling state */
  VSCP_SUBSTATE_AUTO,     /**< RETR mode */
} vscp_substate_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* Ring buffer for IRQ-driven UART1 RX ------------------------------------ */
#define UART1_RX_BUF_SIZE 2048u /* must be a power of 2 */
static uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];
static volatile uint16_t uart1_rx_head = 0; /* written by ISR/callback     */
static volatile uint16_t uart1_rx_tail = 0; /* read  by main loop          */
static uint8_t uart1_rx_byte;               /* single-byte DMA target      */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void
SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void
uart1_rx_start(void);
static int
uart1_rx_getline(char *out, size_t max_len, uint16_t timeout_ms);
static int
uart1_rx_wait_for(const char *needle, uint16_t timeout_ms);
static int
uart1_rx_scan(const char *needle);
static void
uart1_rx_consume_through(const char *needle);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#include <stdio.h>
#include "usart.h"

//////////////////////////////////////////////////////////////////////////
// Retargeting printf to USART2 for debug output.
// This allows us to use printf() to send data over USART2,
// which can be useful for debugging purposes. The __io_putchar
// function is called by the printf() function to output each character,
// and it uses the HAL_UART_Transmit function to send the character over
// USART2. Make sure that USART2 is properly initialized and configured in
// your project for this to work correctly.
//

/*!
 * @brief  Retargets the C library printf function to the USART.
 * @param  ch: Character to be transmitted.
 * @retval The character that was transmitted.
 */
int
__io_putchar(int ch)
{
  HAL_UART_Transmit(&huart2, (uint8_t *) &ch, 1, HAL_MAX_DELAY);
  return ch;
}

/*!
 * @brief  Retargets the C library printf function to the USART.
 * @param  file: File descriptor (not used).
 * @param  ptr: Pointer to the data to be written.
 * @param  len: Length of the data to be written.
 * @retval Number of bytes written.
 */
int
_write(int file, char *ptr, int len)
{
  for (int i = 0; i < len; i++) {
    __io_putchar(*ptr++);
  }
  return len;
}

/*!
 * @brief  Get UART response with timeout (blocking, used during AT init only).
 * @param  buf: Buffer to store the received data.
 * @param  buf_size: Size of the buffer.
 * @param  timeout_ms: Timeout in milliseconds.
 * @retval Number of bytes received, or 0 on timeout.
 */
static size_t
getWizIp20Response(char *buf, size_t buf_size, uint32_t timeout_ms)
{
  memset(buf, 0, buf_size);
  uint16_t rx_len = 0;
  HAL_UARTEx_ReceiveToIdle(&huart1, (uint8_t *) buf, (uint16_t) (buf_size - 1), &rx_len, timeout_ms);
  if (rx_len > 0) {
    buf[rx_len] = '\0';
    printf("IP20 AT response: %s\r\n", buf);
  }
  return rx_len;
}

/*!
 * @brief  Re-arm UART1 IRQ for one byte.  Call once at startup and again
 *         from HAL_UART_RxCpltCallback.
 */
static void
uart1_rx_start(void)
{
  HAL_UART_Receive_IT(&huart1, &uart1_rx_byte, 1);
}

/*!
 * @brief  HAL callback – fires after each byte received via UART1 IT.
 *         Stores the byte in the ring buffer and re-arms the IRQ.
 */
void
HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1) {
    uint16_t next = (uart1_rx_head + 1u) & (UART1_RX_BUF_SIZE - 1u);
    if (next != uart1_rx_tail) { /* drop byte if buffer full */
      uart1_rx_buf[uart1_rx_head] = uart1_rx_byte;
      uart1_rx_head               = next;
    }
    uart1_rx_start(); /* re-arm for next byte     */
  }
}

/*!
 * @brief  Fetch a \r\n-terminated string from the UART1 ring buffer.
 *
 * The buffer is only consumed when a complete \r\n line is available,
 * so partial data is never discarded on timeout.
 *
 * @param  out        Destination buffer; null-terminated on success.
 * @param  max_len    Size of @p out in bytes (including the null terminator).
 * @param  timeout_ms Wait behaviour:
 *                      0x0000 – return immediately if no complete line yet
 *                      0x0001…0xFFFE – wait up to N milliseconds
 *                      0xFFFF – wait forever
 * @retval  0   A complete \r\n-terminated line was copied into @p out.
 * @retval -1   No complete line available within the specified timeout.
 */
static int
uart1_rx_getline(char *out, size_t max_len, uint16_t timeout_ms)
{
  /* Scan the ring buffer for \r\n without consuming any bytes yet. */
  for (;;) {
    /* --- peek: is there a \r\n somewhere in the buffer? --- */
    uint16_t head  = uart1_rx_head; /* snapshot (volatile, read once) */
    uint16_t idx   = uart1_rx_tail;
    uint8_t found  = 0;
    uint16_t eol   = 0; /* index of '\n' in ring buffer   */
    uint16_t count = 0; /* bytes up to and including '\n' */

    while (idx != head) {
      uint8_t ch = uart1_rx_buf[idx];
      count++;
      if (ch == '\n' && count >= 2 && uart1_rx_buf[(idx - 1u) & (UART1_RX_BUF_SIZE - 1u)] == '\r') {
        found = 1;
        eol   = idx;
        break;
      }
      idx = (idx + 1u) & (UART1_RX_BUF_SIZE - 1u);
    }

    if (found) {
      /* Consume bytes up to and including the '\n'. */
      size_t pos = 0;
      idx        = uart1_rx_tail;
      while (1) {
        uint8_t ch = uart1_rx_buf[idx];
        if (pos < max_len - 1u) {
          out[pos++] = (char) ch;
        }
        idx           = (idx + 1u) & (UART1_RX_BUF_SIZE - 1u);
        uart1_rx_tail = idx;
        if (idx == (uint16_t) ((eol + 1u) & (UART1_RX_BUF_SIZE - 1u)))
          break;
      }
      out[pos] = '\0';
      return 0;
    }

    /* No complete line yet – handle timeout. */
    if (timeout_ms == 0u) {
      return -1; /* no-wait: fail immediately      */
    }
    if (timeout_ms == 0xFFFFu) {
      continue; /* wait forever: keep polling     */
    }

    /* Finite timeout: spin until deadline or line arrives. */
    uint32_t deadline = HAL_GetTick() + timeout_ms;
    while (HAL_GetTick() < deadline) {
      /* re-check ring buffer */
      head  = uart1_rx_head;
      idx   = uart1_rx_tail;
      found = 0;
      count = 0;
      while (idx != head) {
        uint8_t ch = uart1_rx_buf[idx];
        count++;
        if (ch == '\n' && count >= 2 && uart1_rx_buf[(idx - 1u) & (UART1_RX_BUF_SIZE - 1u)] == '\r') {
          found = 1;
          eol   = idx;
          break;
        }
        idx = (idx + 1u) & (UART1_RX_BUF_SIZE - 1u);
      }
      if (found)
        break;
    }
    if (!found)
      return -1; /* timed out                      */
    /* line arrived during finite wait – loop back to consume it        */
    timeout_ms = 0xFFFFu; /* force consume path next iter   */
  }
}
/*!
 * @brief  Wait until @p needle appears anywhere in a received line.
 *
 * Reads complete \r\n-terminated lines from the ring buffer one at a time
 * and checks whether @p needle is a substring of each line.
 * Lines that do not match are discarded.
 *
 * @param  needle      String to search for (case-sensitive substring match).
 * @param  timeout_ms  Same semantics as uart1_rx_getline:
 *                       0x0000 – return immediately if no matching line queued
 *                       0x0001–0xFFFE – wait up to N ms
 *                       0xFFFF – wait forever
 * @retval  0   A line containing @p needle was found.
 * @retval -1   Timeout elapsed without a matching line.
 */
static int __attribute__((unused))
uart1_rx_wait_for(const char *needle, uint16_t timeout_ms)
{
  static char line[100];
  uint32_t deadline = (timeout_ms != 0u && timeout_ms != 0xFFFFu) ? HAL_GetTick() + timeout_ms : 0u;

  for (;;) {
    /* How long to wait for the next line? */
    uint16_t line_timeout;
    if (timeout_ms == 0u) {
      line_timeout = 0u; /* no-wait              */
    }
    else if (timeout_ms == 0xFFFFu) {
      line_timeout = 0xFFFFu; /* wait forever         */
    }
    else {
      uint32_t now = HAL_GetTick();
      if (now >= deadline)
        return -1; /* overall timeout      */
      uint32_t left = deadline - now;
      line_timeout  = (left > 0xFFFEu) ? 0xFFFEu : (uint16_t) left;
    }

    if (uart1_rx_getline(line, sizeof(line), line_timeout) != 0) {
      return -1; /* no line / timed out  */
    }

    if (strstr(line, needle) != NULL) {
      return 0; /* match found          */
    }
    /* line didn't match – discard and try again (unless no-wait) */
    if (timeout_ms == 0u)
      return -1;
  }
}
/*!
 * @brief  Scan the ring buffer for @p needle without consuming any bytes.
 * @retval 0  Needle found in the buffer.
 * @retval -1 Needle not present.
 */
static int
uart1_rx_scan(const char *needle)
{
  size_t nlen   = strlen(needle);
  uint16_t head = uart1_rx_head; /* snapshot */
  uint16_t idx  = uart1_rx_tail;
  size_t mpos   = 0; /* match position within needle */

  while (idx != head) {
    if ((char) uart1_rx_buf[idx] == needle[mpos]) {
      mpos++;
      if (mpos == nlen)
        return 0; /* full match */
    }
    else {
      mpos = 0;
      /* re-check current char against needle[0] */
      if ((char) uart1_rx_buf[idx] == needle[0])
        mpos = 1;
    }
    idx = (idx + 1u) & (UART1_RX_BUF_SIZE - 1u);
  }
  return -1;
}

/*!
 * @brief  Discard all bytes in the ring buffer up to and including the
 *         last character of @p needle.  Call only after uart1_rx_scan()
 *         returned 0.
 */
static void
uart1_rx_consume_through(const char *needle)
{
  size_t nlen   = strlen(needle);
  uint16_t head = uart1_rx_head; /* snapshot */
  uint16_t idx  = uart1_rx_tail;
  size_t mpos   = 0;

  while (idx != head) {
    if ((char) uart1_rx_buf[idx] == needle[mpos]) {
      mpos++;
      idx = (idx + 1u) & (UART1_RX_BUF_SIZE - 1u);
      if (mpos == nlen) {
        uart1_rx_tail = idx; /* consume up to end of needle */
        return;
      }
    }
    else {
      mpos = 0;
      if ((char) uart1_rx_buf[idx] == needle[0]) {
        mpos = 1;
      }
      idx = (idx + 1u) & (UART1_RX_BUF_SIZE - 1u);
    }
  }
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int
main(void)
{
  /* USER CODE BEGIN 1 */
  char buf[100]; // Buffer for AT command responses and incoming data
  size_t rx_len;
  (void) rx_len;                  // only used to capture AT response lengths during init
  uint32_t led_blink_until   = 0; // LED blink timestamp
  vscp_state_t vscp_state    = VSCP_STATE_DISCONNECTED;
  vscp_state_t vscp_substate = VSCP_SUBSTATE_POLL;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  // Debug print to indicate that the program has started
  printf("STM32F103 Wiznet IP20 VSCP blink demo starting\r\n");

  /*
    * WIZ-IP20 initialization sequence (AT command mode, blocking)
      We use blocking calls here for simplicity since this only runs once at startup.

      Commands are here: https://docs.wiznet.io/Product/Modules/Serial-to-Ethernet-Module/W232N/command-manual-en
  */

  rx_len = getWizIp20Response(buf, sizeof(buf), 10);

  // Enter AT command mode: guard time, "+++", guard time
  HAL_Delay(500);
  HAL_UART_Transmit(&huart1, (uint8_t *) "+++", 3, HAL_MAX_DELAY);
  HAL_Delay(500);

  rx_len = getWizIp20Response(buf, sizeof(buf), 1000);

  // Query product name
  HAL_UART_Transmit(&huart1, (uint8_t *) "MN\r\n", 4, HAL_MAX_DELAY);
  rx_len = getWizIp20Response(buf, sizeof(buf), 1000);

  // Query firmware version
  HAL_UART_Transmit(&huart1, (uint8_t *) "VR\r\n", 4, HAL_MAX_DELAY);
  rx_len = getWizIp20Response(buf, sizeof(buf), 1000);

  // Query MAC address
  HAL_UART_Transmit(&huart1, (uint8_t *) "MC\r\n", 4, HAL_MAX_DELAY);
  rx_len = getWizIp20Response(buf, sizeof(buf), 1000);

  // Query operation mode
  HAL_UART_Transmit(&huart1, (uint8_t *) "OP\r\n", 4, HAL_MAX_DELAY);
  rx_len = getWizIp20Response(buf, sizeof(buf), 1000);

  // Get connect string
  HAL_UART_Transmit(&huart1, (uint8_t *) "SD\r\n", 4, HAL_MAX_DELAY);
  rx_len = getWizIp20Response(buf, sizeof(buf), 1000);

  // Get connect string
  HAL_UART_Transmit(&huart1, (uint8_t *) "DD\r\n", 4, HAL_MAX_DELAY);
  rx_len = getWizIp20Response(buf, sizeof(buf), 1000);

  // Get UART interface
  HAL_UART_Transmit(&huart1, (uint8_t *) "UN\r\n", 4, HAL_MAX_DELAY);
  rx_len = getWizIp20Response(buf, sizeof(buf), 1000);

  // Get local IP address
  HAL_UART_Transmit(&huart1, (uint8_t *) "LI\r\n", 4, HAL_MAX_DELAY);
  rx_len = getWizIp20Response(buf, sizeof(buf), 1000);

  // Get local subnet mask
  HAL_UART_Transmit(&huart1, (uint8_t *) "SM\r\n", 4, HAL_MAX_DELAY);
  rx_len = getWizIp20Response(buf, sizeof(buf), 1000);

  // Get local gateway IP address
  HAL_UART_Transmit(&huart1, (uint8_t *) "GW\r\n", 4, HAL_MAX_DELAY);
  rx_len = getWizIp20Response(buf, sizeof(buf), 1000);

  // Get local DNS IP address
  HAL_UART_Transmit(&huart1, (uint8_t *) "DS\r\n", 4, HAL_MAX_DELAY);
  rx_len = getWizIp20Response(buf, sizeof(buf), 1000);

  // Get local port number
  HAL_UART_Transmit(&huart1, (uint8_t *) "LP\r\n", 4, HAL_MAX_DELAY);
  rx_len = getWizIp20Response(buf, sizeof(buf), 1000);

  // Set connect string
  HAL_UART_Transmit(&huart1, (uint8_t *) "SD<CONNECT>\r\n", 13, HAL_MAX_DELAY);

  // Set disconnect string
  HAL_UART_Transmit(&huart1, (uint8_t *) "DD<DISCONNECT>\r\n", 16, HAL_MAX_DELAY);

  // Save settings
  HAL_UART_Transmit(&huart1, (uint8_t *) "SV\r\n", 4, HAL_MAX_DELAY);

  // Restart
  HAL_UART_Transmit(&huart1, (uint8_t *) "RT\r\n", 4, HAL_MAX_DELAY);

  HAL_Delay(500);

  // Switch to IRQ-driven receive for data mode
  uart1_rx_start();

  // Greet
  HAL_UART_Transmit(&huart1, (uint8_t *) "HELLO WORLD\r\n", 13, HAL_MAX_DELAY);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    /* ------------------------------------------------------------------
     * State machine – scan ring buffer for control tokens (no \r\n needed)
     * ----------------------------------------------------------------*/
    switch (vscp_state) {

      case VSCP_STATE_DISCONNECTED:
        if (uart1_rx_scan("<CONNECT>") == 0) {
          uart1_rx_consume_through("<CONNECT>");
          printf("Received <CONNECT>\r\n");
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
          led_blink_until      = HAL_GetTick() + 50u;
          const char welcome[] = "Welcome to the VSCP daemon on ST32 + Wiznet IP20.\r\n"
                                 "Version: 0.0.1\r\n"
                                 "Copyright (c) 2000-2026 Ake Hedman, Grodans \r\n"
                                 "Paradis AB, https://www.grodansparadis.com\r\n"
                                 "+OK - Success.\r\n";
          HAL_UART_Transmit(&huart1, (uint8_t *) welcome, sizeof(welcome) - 1, HAL_MAX_DELAY);
          vscp_state = VSCP_STATE_CONNECTED;
          printf("State: DISCONNECTED -> CONNECTED\r\n");
        }
        else {
          /* Discard any \r\n-terminated lines that aren't <CONNECT> */
          uart1_rx_getline(buf, sizeof(buf), 0);
        }
        break;

      case VSCP_STATE_CONNECTED:
        if (uart1_rx_scan("<DISCONNECT>") == 0) {
          uart1_rx_consume_through("<DISCONNECT>");
          printf("Received <DISCONNECT>\r\n");
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
          led_blink_until = HAL_GetTick() + 50u;
          HAL_UART_Transmit(&huart1, (uint8_t *) "+OK - Disconnect.\r\n", 19, HAL_MAX_DELAY);
          vscp_state = VSCP_STATE_DISCONNECTED;
          printf("State: CONNECTED -> DISCONNECTED\r\n");
        }
        else if (uart1_rx_getline(buf, sizeof(buf), 0) == 0) {
          /* Consume and log incoming VSCP commands */
          printf("VSCP cmd: %s", buf);
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
          led_blink_until = HAL_GetTick() + 50u;
        }
        break;

      default:
        vscp_state = VSCP_STATE_DISCONNECTED;
        break;
    }

    /* Turn LED off after 50 ms blink */
    if (led_blink_until != 0u && HAL_GetTick() >= led_blink_until) {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); /* LED off */
      led_blink_until = 0u;
    }

    /* Toggle the state of pin 13 on GPIO port C */
    // if ((HAL_GetTick() - now) >= 500) {
    //   now = HAL_GetTick();
    //   HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    //   printf("Super\r\n"); // Print "Super" to the ITM console
    //   const char msg[] = "hello\r\n";
    //   HAL_UART_Transmit(&huart2, (uint8_t *) msg, sizeof(msg) - 1, HAL_MAX_DELAY);
    // }

    /* Wait for 500 milliseconds */
    // HAL_Delay(500);
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void
SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void
Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {}
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void
assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
