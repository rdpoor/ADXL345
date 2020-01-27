/**
 * MIT License
 *
 * Copyright (c) 2020 R. Dunbar Poor <rdpoor@gmail.com>
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

#include <atmel_start.h>
#include <stdio.h>
#include <string.h>
#include "adxl345.h"
#include "adxl345_asf4_i2c.h"
#include "adxl345_err.h"

// =============================================================================
// local types and definitions

// character sequence to home cursor and clear to end of screen
// (not currently used)
#define CLEAR_SCREEN "\e[1;1H\e[2J"

// =============================================================================
// local (forward) declarations

// =============================================================================
// local storage

// =============================================================================
// public code

// =============================================================================
// local (static) code

int main(void) {
  adxl345_t adxl345;          // the ADXL345 object
  adxl345_dev_t adxl345_dev;  // the ADXL345 device interface
  adxl345_err_t err;
  uint32_t sample_count;
  uint8_t high_water;

  /* Initializes MCU, drivers and middleware */
  atmel_start_init();

  printf("\r\nADXL345 example: initializing...\r\n");

  // Initialize the device-level interface
  err = adxl345_dev_init(&adxl345_dev, &ADXL345_0, ADXL345_I2C_PRIMARY_ADDRESS,
                         I2C_M_SEVEN);
  ASSERT(err == ADXL345_ERR_NONE);

  // Initialize the ADXL345 object with the device-level interface
  err = adxl345_init(&adxl345, &adxl345_dev);
  // ASSERT(err == ADXL345_ERR_NONE);

  // Reset the ADXL345 (in case it was running)
  err = adxl345_reset(&adxl345);
  ASSERT(err == ADXL345_ERR_NONE);

  // Configure ADXL345 sampling rate
  err = adxl345_set_bw_rate_reg(&adxl345, ADXL345_RATE_100);
  ASSERT(err == ADXL345_ERR_NONE);

  // Configure FIFO mode with high water mark set to 1 sample.
  err = adxl345_set_fifo_ctl_reg(&adxl345, ADXL345_FIFO_MODE_ENABLE | 1);
  ASSERT(err == ADXL345_ERR_NONE);

  high_water = 0;
  sample_count = 0;

  printf("high water, sample count, x, y, z\r\n");

  // Start converting
  err = adxl345_start(&adxl345);
  ASSERT(err == ADXL345_ERR_NONE);

  while (1) {
    uint8_t available;         // # of samples available in FIFO
    adxl345_isample_t sample;  // xyz sample data

    // get number of samples available in the FIFO
    err = adxl345_available_samples(&adxl345, &available);
    ASSERT(err == ADXL345_ERR_NONE);

    // track the high water mark
    if (available > high_water) high_water = available;

    for (int i = 0; i < available; i++) {
      // Fetch X, Y, Z sample in floating point format
      err = adxl345_get_isample(&adxl345, &sample);
      ASSERT(err == ADXL345_ERR_NONE);

      sample_count += 1;
      printf("%2d, %5ld, %4d, %4d, %4d\r", high_water, sample_count,
             sample.x, sample.y, sample.z);
    }
  }
}

// =============================================================================
// private (local) code
