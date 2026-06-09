// FILE: flash_storage.h

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (C) 2000-2026 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ******************************************************************************
 *
 * Persistent storage in the last 2 pages (2 KB) of internal flash.
 *
 * Memory map (STM32F103C6, 32 KB flash, 1 KB pages):
 *   0x08000000 – 0x080077FF  firmware  (30 KB)
 *   0x08007800 – 0x08007BFF  storage page 0  (1 KB)
 *   0x08007C00 – 0x08007FFF  storage page 1  (1 KB)
 *
 * Constraints:
 *   - An entire page must be erased before any byte in it can be written.
 *   - Writes are 16-bit (half-word) aligned.
 *   - Flash endurance: ~10 000 erase cycles per page.
 *
 * ******************************************************************************
 */

#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "stm32f1xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Region constants (must match linker script STORAGE region)          */
/* ------------------------------------------------------------------ */

#define FLASH_STORAGE_BASE   0x08007800UL  /**< First address of storage area */
#define FLASH_STORAGE_SIZE   0x00000800UL  /**< Total size: 2 KB              */
#define FLASH_STORAGE_PAGES  2U            /**< Number of 1 KB pages reserved  */
#define FLASH_PAGE_SIZE      0x00000400UL  /**< STM32F103 page size: 1 KB      */

/* ------------------------------------------------------------------ */
/* Header (stored at offset 0)                                         */
/* ------------------------------------------------------------------ */

#define FLASH_STORAGE_MAGIC    0xA55AU  /**< Magic value — storage is initialised */
#define FLASH_STORAGE_VERSION  1U       /**< Increment when layout changes        */

typedef struct {
    uint16_t magic;    /**< Must equal FLASH_STORAGE_MAGIC   */
    uint16_t version;  /**< Layout version for migration     */
} flash_storage_header_t;

/** Byte offset of the first user data word (immediately after header). */
#define FLASH_STORAGE_DATA_OFFSET  (sizeof(flash_storage_header_t))

/* ------------------------------------------------------------------ */
/* API                                                                 */
/* ------------------------------------------------------------------ */

/**
 * @brief  Check whether the storage area contains a valid header.
 *
 * Reads the magic word at offset 0. Returns false when the storage is
 * blank (erased) or has been corrupted.
 *
 * @return true  if the magic word matches FLASH_STORAGE_MAGIC.
 * @return false if the storage is uninitialised or erased.
 */
bool flash_storage_is_valid(void);

/**
 * @brief  Erase the entire storage area (both pages).
 *
 * Must be called before writing any data into a page that already
 * contains non-0xFFFF values.
 *
 * @return HAL_OK on success, HAL_ERROR / HAL_BUSY / HAL_TIMEOUT otherwise.
 */
HAL_StatusTypeDef flash_storage_erase(void);

/**
 * @brief  Write the storage header (magic + version) at offset 0.
 *
 * Call this once after flash_storage_erase() to mark the storage as valid.
 *
 * @return HAL_OK on success.
 */
HAL_StatusTypeDef flash_storage_write_header(void);

/**
 * @brief  Write an array of 16-bit half-words to flash storage.
 *
 * @param  offset  Byte offset from FLASH_STORAGE_BASE (must be even).
 * @param  data    Pointer to the source half-word array.
 * @param  count   Number of half-words to write.
 * @return HAL_OK on success, HAL_ERROR on bounds violation or flash error.
 */
HAL_StatusTypeDef flash_storage_write(uint32_t offset, const uint16_t *data, size_t count);

/**
 * @brief  Read an array of 16-bit half-words from flash storage.
 *
 * @param  offset  Byte offset from FLASH_STORAGE_BASE (must be even).
 * @param  data    Pointer to the destination buffer.
 * @param  count   Number of half-words to read.
 */
void flash_storage_read(uint32_t offset, uint16_t *data, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* FLASH_STORAGE_H */
