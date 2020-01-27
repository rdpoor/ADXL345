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

// =============================================================================
// includes

#include <string.h>
#include "adxl345_asf4_i2c.h"
#include "adxl345_err.h"
#include "driver_init.h"
#include "utils.h"
#include "hal_i2c_m_sync.h"

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

// =============================================================================
// local storage

// =============================================================================
// public code

adxl345_err_t adxl345_dev_init(adxl345_dev_t *dev,
                               struct i2c_m_sync_desc *const i2c_descriptor,
                               int16_t slave_addr,
                               int32_t addr_len) {
  dev->i2c_descriptor = i2c_descriptor;
  dev->slave_addr = slave_addr;
  dev->addr_len = addr_len;

  i2c_m_sync_enable(i2c_descriptor);
  i2c_m_sync_set_slaveaddr(i2c_descriptor, slave_addr, addr_len);

  return ADXL345_ERR_NONE;
}

/** @brief Read one ADXL345 register.
 *
 * @param reg_addr Address of register to be read
 * @param dst Pointer to destination buffer.
 *
 * @return 0 on success, non-zero on error.
 */
adxl345_err_t adxl345_dev_read_reg(adxl345_dev_t *dev, uint8_t reg_addr, uint8_t *dst) {
  int32_t err = i2c_m_sync_cmd_read(dev->i2c_descriptor, reg_addr, dst, 1);
  if (err < 0) {
    return ADXL345_ERR_WRITE;
  }
  return ADXL345_ERR_NONE;
}

/** @brief Write one ADXL345 register.
 *
 * @param reg_addr Address of register to be written.
 * @param val Value to be written.
 *
 * @return 0 on success, non-zero on error.
 */
adxl345_err_t adxl345_dev_write_reg(adxl345_dev_t *dev, uint8_t reg_addr, uint8_t val) {
  uint8_t buf[2];
  buf[0] = reg_addr;
  buf[1] = val;

  struct _i2c_m_msg msg;
  int32_t ret;
  msg.addr = dev->slave_addr;
  msg.len = sizeof(buf);
  msg.flags = I2C_M_STOP;
  msg.buffer = buf;

  ret = i2c_m_sync_transfer(dev->i2c_descriptor, &msg);
  if (ret < 0) {
    return ADXL345_ERR_WRITE;
  }
  return ADXL345_ERR_NONE;
}

/** @brief Read multiple registers from the ADXL345.
 *
 * Read one or more consecutive registers in a single I/O operation
 *
 * @param reg_addr Address of first register to be read
 * @param dst Pointer to destination buffer.  Must have capacity of n_bytes.
 * @param n_bytes Number of registers to be read.
 *
 * @return 0 on success, non-zero on error.
 */
adxl345_err_t adxl345_dev_read_regs(adxl345_dev_t *dev, uint8_t reg_addr, uint8_t *dst, uint8_t n_bytes) {
  int32_t err = i2c_m_sync_cmd_read(dev->i2c_descriptor, reg_addr, dst, n_bytes);
  if (err < 0) {
    return ADXL345_ERR_READ;
  }
  return ADXL345_ERR_NONE;
}

/** @brief Write multiple registers to the ADXL345.
 *
 * Write one or more consecutive registers in a single I/O operation
 *
 * @param start_addr Address of first register to be written.
 * @param src Pointer to source data.  Must have capacity of n_bytes.
 * @param n_bytes Number of registers to be written.
 *
 * @return 0 on success, non-zero on error.
 */
adxl345_err_t adxl345_dev_write_regs(adxl345_dev_t *dev, uint8_t reg_addr, uint8_t *src, uint8_t n_bytes) {
  uint8_t buf[ADXL345_I2C_MAX_COUNT + 1];
  struct _i2c_m_msg msg;
  int32_t ret;
  msg.addr = dev->slave_addr;
  msg.len = n_bytes > ADXL345_I2C_MAX_COUNT ? ADXL345_I2C_MAX_COUNT : n_bytes;
  msg.flags = I2C_M_STOP;
  msg.buffer = buf;

  buf[0] = reg_addr;
  memcpy(&buf[1], src, msg.len);

  ret = i2c_m_sync_transfer(dev->i2c_descriptor, &msg);
  if (ret < 0) {
    return ADXL345_ERR_WRITE;
  }
  return ADXL345_ERR_NONE;
}

// =============================================================================
// local (static) code
