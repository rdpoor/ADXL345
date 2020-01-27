/**
 * MIT License
 *
 * Copyright (c) 2019 R. Dunbar Poor <rdpoor@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _ADXL345_ASF4_I2C_H_
#define _ADXL345_ASF4_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include <stdint.h>
#include <stdbool.h>
#include "adxl345_err.h"

// =============================================================================
// types and definitions

#define ADXL345_I2C_ALTERNATE_ADDRESS 0x1D   ///< SCO/ALT_ADDRESS high
#define ADXL345_I2C_PRIMARY_ADDRESS 0x53     ///< SCO/ALT_ADDRESS low

#define ADXL345_I2C_MAX_COUNT 16

typedef struct {
  struct i2c_m_sync_desc *i2c_descriptor;
  int16_t slave_addr;
  int32_t addr_len;
} adxl345_dev_t;

// =============================================================================
// declarations

adxl345_err_t adxl345_dev_init(adxl345_dev_t *dev,
                               struct i2c_m_sync_desc *const i2c_descriptor,
                               int16_t slave_addr,
                               int32_t addr_len);

/** @brief Read one ADXL345 register.
 *
 * @param addr Address of register to be read
 * @param dst Pointer to destination buffer.
 *
 * @return 0 on success, non-zero on error.
 */
adxl345_err_t adxl345_dev_read_reg(adxl345_dev_t *dev, uint8_t saddr, uint8_t *dst);

/** @brief Write one ADXL345 register.
 *
 * @param addr Address of register to be written.
 * @param val Value to be written.
 *
 * @return 0 on success, non-zero on error.
 */
adxl345_err_t adxl345_dev_write_reg(adxl345_dev_t *dev, uint8_t addr, uint8_t val);

/** @brief Read multiple registers from the ADXL345.
 *
 * Read one or more consecutive registers in a single I/O operation.
 *
 * @param start_addr Address of first register to be read
 * @param dst Pointer to destination buffer.  Must have capacity of n_bytes.
 * @param n_bytes Number of registers to be read.
 *
 * @return 0 on success, non-zero on error.
 */
adxl345_err_t adxl345_dev_read_regs(adxl345_dev_t *dev, uint8_t start_addr, uint8_t *dst, uint8_t n_bytes);

/** @brief Write multiple registers to the ADXL345.
 *
 * Write one or more consecutive registers in a single I/O operation.
 * Limited to ADXL345_I2C_MAX_COUNT registers at a time.
 *
 * @param start_addr Address of first register to be written.
 * @param src Pointer to source data.  Must have capacity of n_bytes.
 * @param n_bytes Number of registers to be written.
 *
 * @return 0 on success, non-zero on error.
 */
adxl345_err_t adxl345_dev_write_regs(adxl345_dev_t *dev, uint8_t start_addr, uint8_t *src, uint8_t n_bytes);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ADXL345_ASF4_I2C_H_ */
