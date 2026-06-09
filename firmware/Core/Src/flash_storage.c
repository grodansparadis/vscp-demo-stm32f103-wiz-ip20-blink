// FILE: flash_storage.c

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
 */

#include "flash_storage.h"

/* ------------------------------------------------------------------ */

bool flash_storage_is_valid(void)
{
    flash_storage_header_t hdr;
    flash_storage_read(0U, (uint16_t *)&hdr, sizeof(hdr) / sizeof(uint16_t));
    return (hdr.magic == FLASH_STORAGE_MAGIC);
}

/* ------------------------------------------------------------------ */

HAL_StatusTypeDef flash_storage_write_header(void)
{
    const flash_storage_header_t hdr = {
        .magic   = FLASH_STORAGE_MAGIC,
        .version = FLASH_STORAGE_VERSION,
    };
    return flash_storage_write(0U, (const uint16_t *)&hdr, sizeof(hdr) / sizeof(uint16_t));
}

/* ------------------------------------------------------------------ */

HAL_StatusTypeDef flash_storage_erase(void)
{
    FLASH_EraseInitTypeDef erase_init = {
        .TypeErase   = FLASH_TYPEERASE_PAGES,
        .PageAddress = FLASH_STORAGE_BASE,
        .NbPages     = FLASH_STORAGE_PAGES,
    };
    uint32_t page_error = 0U;
    HAL_StatusTypeDef status;

    HAL_FLASH_Unlock();
    status = HAL_FLASHEx_Erase(&erase_init, &page_error);
    HAL_FLASH_Lock();

    return status;
}

/* ------------------------------------------------------------------ */

HAL_StatusTypeDef flash_storage_write(uint32_t offset, const uint16_t *data, size_t count)
{
    HAL_StatusTypeDef status = HAL_OK;

    if (data == NULL) {
        return HAL_ERROR;
    }

    HAL_FLASH_Unlock();

    for (size_t i = 0U; i < count && status == HAL_OK; i++) {
        uint32_t addr = FLASH_STORAGE_BASE + offset + (i * 2U);

        /* Bounds check */
        if ((addr + 2U) > (FLASH_STORAGE_BASE + FLASH_STORAGE_SIZE)) {
            status = HAL_ERROR;
            break;
        }

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, (uint64_t)data[i]);
    }

    HAL_FLASH_Lock();

    return status;
}

/* ------------------------------------------------------------------ */

void flash_storage_read(uint32_t offset, uint16_t *data, size_t count)
{
    if (data == NULL) {
        return;
    }

    for (size_t i = 0U; i < count; i++) {
        uint32_t addr = FLASH_STORAGE_BASE + offset + (i * 2U);
        data[i] = *(const volatile uint16_t *)addr;
    }
}
