/** @file adxl345h
 *
 * MIT License
 *
 * Copyright (c) 2019 R. Dunbar Poor <rdpoor@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _ADXL345_H_
#define _ADXL345_H_

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// includes

#include "adxl345_dev.h"
#include "adxl345_err.h"
#include <stdbool.h>
#include <stdint.h>

// =============================================================================
// types and definitions

typedef enum {
  ADXL345_REG_DEVID = 0,       ///< 0x00 00 R   11100101 Device ID
  ADXL345_REG_THRESH_TAP = 29, ///< 0x1D 29 R/W 00000000 Tap threshold
  ADXL345_REG_OFSX,            ///< 0x1E 30 R/W 00000000 X-axis offset
  ADXL345_REG_OFSY,            ///< 0x1F 31 R/W 00000000 Y-axis offset
  ADXL345_REG_OFSZ,            ///< 0x20 32 R/W 00000000 Z-axis offset
  ADXL345_REG_DUR,             ///< 0x21 33 R/W 00000000 Tap duration
  ADXL345_REG_LATENT,          ///< 0x22 34 R/W 00000000 Tap latency
  ADXL345_REG_WINDOW,          ///< 0x23 35 R/W 00000000 Tap window
  ADXL345_REG_THRESH_ACT,      ///< 0x24 36 R/W 00000000 Activity threshold
  ADXL345_REG_THRESH_INACT,    ///< 0x25 37 R/W 00000000 Inactivity threshold
  ADXL345_REG_TIME_INACT,      ///< 0x26 38 R/W 00000000 Inactivity time
  ADXL345_REG_ACT_INACT_CTL,   ///< 0x27 39 R/W 00000000 Activity control
  ADXL345_REG_THRESH_FF,       ///< 0x28 40 R/W 00000000 Free-fall threshold
  ADXL345_REG_TIME_FF,         ///< 0x29 41 R/W 00000000 Free-fall time
  ADXL345_REG_TAP_AXES,        ///< 0x2A 42 R/W 00000000 Axis control for tap
  ADXL345_REG_ACT_TAP_STATUS,  ///< 0x2B 43 R   00000000 Source of tap
  ADXL345_REG_BW_RATE,         ///< 0x2C 44 R/W 00001010 Data rate and power control
  ADXL345_REG_POWER_CTL,       ///< 0x2D 45 R/W 00000000 Power-saving features
  ADXL345_REG_INT_ENABLE,      ///< 0x2E 46 R/W 00000000 Interrupt enable control
  ADXL345_REG_INT_MAP,         ///< 0x2F 47 R/W 00000000 Interrupt mapping control
  ADXL345_REG_INT_SOURCE,      ///< 0x30 48 R   00000010 Source of interrupts
  ADXL345_REG_DATA_FORMAT,     ///< 0x31 49 R/W 00000000 Data format control
  ADXL345_REG_DATAX0,          ///< 0x32 50 R   00000000 X-Axis Data 0
  ADXL345_REG_DATAX1,          ///< 0x33 51 R   00000000 X-Axis Data 1
  ADXL345_REG_DATAY0,          ///< 0x34 52 R   00000000 Y-Axis Data 0
  ADXL345_REG_DATAY1,          ///< 0x35 53 R   00000000 Y-Axis Data 1
  ADXL345_REG_DATAZ0,          ///< 0x36 54 R   00000000 Z-Axis Data 0
  ADXL345_REG_DATAZ1,          ///< 0x37 55 R   00000000 Z-Axis Data 1
  ADXL345_REG_FIFO_CTL,        ///< 0x38 56 R/W 00000000 FIFO control
  ADXL345_REG_FIFO_STATUS      ///< 0x39 57 R   00000000 FIFO status
} adxl345_register_t;

/** Fixed device identifier */
#define ADXL345_DEVICE_ID 0xE5

/** Map raw THRESH_TAP register to g */
#define ADXL345_REG_THRESH_TAP_SCALE 0.0625

/** Map raw OFSX, OFXY, OFSZ register value to g */
#define ADXL345_OFSx_SCALE 0.0156

/** Map raw DUR register to seconds */
#define ADXL345_DUR_SCALE 0.000625

/** Map raw LATENT register to seconds */
#define ADXL345_LATENT_SCALE 0.00125

/** Map raw WINDOW register to seconds */
#define ADXL345_WINDOW_SCALE 0.00125

/** Map raw THRESH_ACT register to g */
#define ADXL345_THRESH_ACT_SCALE 0.0625

/** Map raw THRESH_INACT register to g */
#define ADXL345_THRESH_INACT_SCALE 0.0625

/** Map raw TIME_INACT register to seconds */
#define ADXL345_TIME_INACT_SCALE 1.0

/** 2 / 2^9 */
#define ADXL345_2G_SCALE 0.00390625

typedef enum {
  ADXL345_ACT_AC_ENABLE = 0x80,  ///< Enable AC coupling for activity detection
  ADXL345_ACT_X_ENABLE = 0x40,     ///< Enable X axis for activity detection
  ADXL345_ACT_Y_ENABLE = 0x20,     ///< Enable Y axis for activity detection
  ADXL345_ACT_Z_ENABLE = 0x10,     ///< Enable Z axis for activity detection
  ADXL345_INACT_AC_ENABLE = 0x08,  ///< Enable AC coupling for inactivity
                                 ///< detect.
  ADXL345_INACT_X_ENABLE = 0x04,   ///< Enable X axis for inactivity detection
  ADXL345_INACT_Y_ENABLE = 0x01,   ///< Enable Y axis for inactivity detection
  ADXL345_INACT_Z_ENABLE = 0x01,   ///< Enable Z axis for inactivity detection
} adxl345_act_inact_ctl_reg;

/** Map THRESH_FF register value to g */
#define ADXL345_THRESH_FF_SCALE 0.00625

/** Map TIME_FF register value to seconds */
#define ADXL345_TIME_FF_SCALE 0.005

typedef enum {
  ADXL345_DOUBLE_TAP_SUPPRESS = 0x08,  ///< Suppress double tap detection if
                                       ///< accel is greater than THRESH_TAP
  ADXL345_TAP_X_ENABLE = 0x04,         ///< Enable tap detection on X axis
  ADXL345_TAP_Y_ENABLE = 0x02,         ///< Enable tap detection on Y axis
  ADXL345_TAP_Z_ENABLE = 0x01,         ///< Enable tap detection on Z axis
} adxl345_tap_axes_reg;

typedef enum {
  ADXL345_LOW_POWER_ENABLE = 0x10,  ///< Enable low power mode
  ADXL345_RATE_3200 = 0x0f,         ///< Data Rate = 3200 Hz
  ADXL345_RATE_1600 = 0x0e,         ///< Data Rate = 1600 Hz
  ADXL345_RATE_800 = 0x0d,          ///< Data Rate = 800 Hz
  ADXL345_RATE_400 = 0x0c,          ///< Data Rate = 400 Hz
  ADXL345_RATE_200 = 0x0b,          ///< Data Rate = 200 Hz
  ADXL345_RATE_100 = 0x0a,          ///< Data Rate = 100 Hz (default)
  ADXL345_RATE_50 = 0x09,           ///< Data Rate = 50 Hz
  ADXL345_RATE_25 = 0x08,           ///< Data Rate = 25 Hz
  ADXL345_RATE_12_5 = 0x07,         ///< Data Rate = 12.5 Hz
  ADXL345_RATE_6_25 = 0x06,         ///< Data Rate = 6.25 Hz
  ADXL345_RATE_3_13 = 0x05,         ///< Data Rate = 3.13 Hz
  ADXL345_RATE_1_56 = 0x04,         ///< Data Rate = 1.56 Hz
  ADXL345_RATE_0_78 = 0x03,         ///< Data Rate = 0.78 Hz
  ADXL345_RATE_0_39 = 0x02,         ///< Data Rate = 0.39 Hz
  ADXL345_RATE_0_20 = 0x01,         ///< Data Rate = 0.2 Hz
  ADXL345_RATE_0_10 = 0x00,         ///< Data Rate = 0.1 Hz
} adxl345_bw_rate_reg;

typedef enum {
  ADXL345_LINK = 0x20,        ///< Enable link bit
  ADXL345_AUTO_SLEEP = 0x10,  ///< Enable auto sleep mode
  ADXL345_MEASURE = 0x08,     ///< Enter measurement mode
  ADXL345_SLEEP = 0x04,       ///< Enter sleep mode
  ADXL345_WAKEUP_8 = 0x00,    ///< Read at 8 Hz in sleep mode
  ADXL345_WAKEUP_4 = 0x01,    ///< Read at 4 Hz in sleep mode
  ADXL345_WAKEUP_2 = 0x02,    ///< Read at 2 Hz in sleep mode
  ADXL345_WAKEUP_1 = 0x03,    ///< Read at 1 Hz in sleep mode
} adxl345_power_ctl_reg;

typedef enum {
  ADXL345_DATA_READY_INT = 0x80,  ///< Enable / Read DATA_READY interrupt
  ADXL345_SINGLE_TAP_INT = 0x40,  ///< Enable / Read SINGLE_TAP interrupt
  ADXL345_DOUBLE_TAP_INT = 0x20,  ///< Enable / Read DOUBLE_TAP interrupt
  ADXL345_ACTIVITY_INT = 0x10,    ///< Enable / Read ACTIVITY interrupt
  ADXL345_INACTIVITY_INT = 0x08,  ///< Enable / Read INACTIVITY interrupt
  ADXL345_FREE_FALL_INT = 0x04,   ///< Enable / Read FREE_FALL interrupt
  ADXL345_WATERMARK_INT = 0x02,   ///< Enable / Read WATERMARK interrupt
  ADXL345_OVERRUN_INT = 0x01,     ///< Enable / Read OVERRUN interrupt
} adxl345_interrupt_reg;

typedef enum {
  ADXL345_SELF_TEST = 0x80,     ///< Enable self test mode
  ADXL345_3_WIRE_SPI = 0x40,    ///< Enable 3-wire SPI mode
  ADXL345_INT_INVERT = 0x20,    ///< Enable low-true interrupt line
  ADXL345_FULL_RES = 0x08,      ///< Enable full resolution mode
  ADXL345_LEFT_JUSTIFY = 0x04,  ///< Enable left-justified
  ADXL345_RANGE_2G = 0x00,      ///< Set range to +/- 2g
  ADXL345_RANGE_4G = 0x01,      ///< Set range to +/- 4g
  ADXL345_RANGE_8G = 0x02,      ///< Set range to +/- 8g
  ADXL345_RANGE_16G = 0x03,     ///< Set range to +/- 16g
} adxl345_data_format_reg;

typedef struct {
  uint8_t x0; ///<
  uint8_t x1; ///<
  uint8_t y0; ///<
  uint8_t y1; ///<
  uint8_t z0; ///<
  uint8_t z1; ///<
} adxl345_data_regs_t;

typedef enum {
  ADXL345_FIFO_MODE_BYPASS = 0x00,   ///< No FIFO
  ADXL345_FIFO_MODE_ENABLE = 0x40,   ///< Stop when FIFO is full
  ADXL345_FIFO_MODE_STREAM = 0x80,   ///< Save most recent 32 samples
  ADXL345_FIFO_MODE_TRIGGER = 0xC0,  ///< Stream samples until trigger evnt
  ADXL345_TRIGGER_INT2 = 0x20,       ///< Route trigger evnts to interrupt 2
  ADXL345_TRIGGER_WATERMARK_MASK = 0x1F,  ///< watermark reg is 5 bits wide
} adxl345_fifo_mode_reg;

typedef enum {
  ADXL345_FIFO_STATUS_TRIGGER = 0x80,  ///< True if FIFO trigger happened
  ADXL345_FIFO_ENTRIES_MASK = 0x3F,    ///< Holds # of entries in FIFO
} adxl345_fifo_status_reg;

typedef struct {
  int16_t x; ///< X acceleration in unspecified units
  int16_t y; ///< Y acceleration in unspecified units
  int16_t z; ///< Z acceleration in unspecified units
} adxl345_isample_t;

typedef struct {
  float x;  ///< X acceleration in g's
  float y;  ///< Y acceleration in g's
  float z;  ///< Z acceleration in g's
} adxl345_fsample_t;

typedef struct {
  adxl345_dev_t *dev;
} adxl345_t;

// =============================================================================
// declarations

/**
 * @brief initialize the adxl345 module.
 */
adxl345_err_t adxl345_init(adxl345_t *adxl345, adxl345_dev_t * dev);

// ==========================================
// low-level register access

adxl345_err_t adxl345_get_devid_reg(adxl345_t *adxl345, uint8_t *val);

adxl345_err_t adxl345_get_thresh_tap_reg(adxl345_t *adxl345, uint8_t *val);
adxl345_err_t adxl345_set_thresh_tap_reg(adxl345_t *adxl345, uint8_t val);

adxl345_err_t adxl345_get_ofsx_reg(adxl345_t *adxl345, uint8_t *val);
adxl345_err_t adxl345_set_ofsx_reg(adxl345_t *adxl345, uint8_t val);

adxl345_err_t adxl345_get_ofsy_reg(adxl345_t *adxl345, uint8_t *val);
adxl345_err_t adxl345_set_ofsy_reg(adxl345_t *adxl345, uint8_t val);

adxl345_err_t adxl345_get_ofsz_reg(adxl345_t *adxl345, uint8_t *val);
adxl345_err_t adxl345_set_ofsz_reg(adxl345_t *adxl345, uint8_t val);

adxl345_err_t adxl345_get_dur_reg(adxl345_t *adxl345, uint8_t *val);
adxl345_err_t adxl345_set_dur_reg(adxl345_t *adxl345, uint8_t val);

adxl345_err_t adxl345_get_latency_reg(adxl345_t *adxl345, uint8_t *val);
adxl345_err_t adxl345_set_latency_reg(adxl345_t *adxl345, uint8_t val);

adxl345_err_t adxl345_get_window_reg(adxl345_t *adxl345, uint8_t *val);
adxl345_err_t adxl345_set_window_reg(adxl345_t *adxl345, uint8_t val);

adxl345_err_t adxl345_get_thresh_act_reg(adxl345_t *adxl345, uint8_t *val);
adxl345_err_t adxl345_set_thresh_act_reg(adxl345_t *adxl345, uint8_t val);

adxl345_err_t adxl345_get_thresh_inact_reg(adxl345_t *adxl345, uint8_t *val);
adxl345_err_t adxl345_set_thresh_inact_reg(adxl345_t *adxl345, uint8_t val);

adxl345_err_t adxl345_get_time_inact_reg(adxl345_t *adxl345, uint8_t *val);
adxl345_err_t adxl345_set_time_inact_reg(adxl345_t *adxl345, uint8_t val);

adxl345_err_t adxl345_get_act_inact_ctl_reg(adxl345_t *adxl345, adxl345_act_inact_ctl_reg *val);
adxl345_err_t adxl345_set_act_inact_ctl_reg(adxl345_t *adxl345, adxl345_act_inact_ctl_reg val);

adxl345_err_t adxl345_get_thresh_ff_reg(adxl345_t *adxl345, uint8_t *val);
adxl345_err_t adxl345_set_thresh_ff_reg(adxl345_t *adxl345, uint8_t val);

adxl345_err_t adxl345_get_time_ff_reg(adxl345_t *adxl345, uint8_t *val);
adxl345_err_t adxl345_set_time_ff_reg(adxl345_t *adxl345, uint8_t val);

adxl345_err_t adxl345_get_tap_axes_reg(adxl345_t *adxl345, adxl345_tap_axes_reg *val);
adxl345_err_t adxl345_set_tap_axes_reg(adxl345_t *adxl345, adxl345_tap_axes_reg val);

adxl345_err_t adxl345_get_bw_rate_reg(adxl345_t *adxl345, adxl345_bw_rate_reg *val);
adxl345_err_t adxl345_set_bw_rate_reg(adxl345_t *adxl345, adxl345_bw_rate_reg val);

adxl345_err_t adxl345_get_power_ctl_reg(adxl345_t *adxl345, adxl345_power_ctl_reg *val);
adxl345_err_t adxl345_set_power_ctl_reg(adxl345_t *adxl345, adxl345_power_ctl_reg val);

adxl345_err_t adxl345_get_int_enable_reg(adxl345_t *adxl345, adxl345_interrupt_reg *val);
adxl345_err_t adxl345_set_int_enable_reg(adxl345_t *adxl345, adxl345_interrupt_reg val);

adxl345_err_t adxl345_get_int_map_reg(adxl345_t *adxl345, adxl345_interrupt_reg *val);
adxl345_err_t adxl345_set_int_map_reg(adxl345_t *adxl345, adxl345_interrupt_reg val);

adxl345_err_t adxl345_get_int_source_reg(adxl345_t *adxl345, adxl345_interrupt_reg *val);

adxl345_err_t adxl345_get_data_format_reg(adxl345_t *adxl345, adxl345_data_format_reg *val);
adxl345_err_t adxl345_set_data_format_reg(adxl345_t *adxl345, adxl345_data_format_reg val);

// Because x, y, z samples must be read in a multi-register read,
// these methods are not provided.
// adxl345_err_t adxl345_get_datax0_reg(adxl345_t *adxl345, uint8_t *val);
// adxl345_err_t adxl345_get_datax1_reg(adxl345_t *adxl345, uint8_t *val);
// adxl345_err_t adxl345_get_datay0_reg(adxl345_t *adxl345, uint8_t *val);
// adxl345_err_t adxl345_get_datay1_reg(adxl345_t *adxl345, uint8_t *val);
// adxl345_err_t adxl345_get_dataz0_reg(adxl345_t *adxl345, uint8_t *val);
// adxl345_err_t adxl345_get_dataz1_reg(adxl345_t *adxl345, uint8_t *val);

/** @brief Fetch x, y, z accelerometer data in one operation
 */
adxl345_err_t adxl345_get_data_regs(adxl345_t *adxl345, adxl345_data_regs_t *dst);

adxl345_err_t adxl345_get_fifo_ctl_reg(adxl345_t *adxl345, adxl345_fifo_mode_reg *val);
adxl345_err_t adxl345_set_fifo_ctl_reg(adxl345_t *adxl345, adxl345_fifo_mode_reg val);

adxl345_err_t adxl345_get_fifo_status_reg(adxl345_t *adxl345, adxl345_fifo_status_reg *val);

// ==========================================
// higher level functions.  In the functions below,
// _g stands for gravity and _s stands for seconds.

/** @brief Enter measurement mode: start measuring */
adxl345_err_t adxl345_start(adxl345_t *adxl345);

/** @brief Enter standby mode: stop measuring */
adxl345_err_t adxl345_stop(adxl345_t *adxl345);

adxl345_err_t adxl345_get_tap_thresh_g(adxl345_t *adxl345, float *val);
adxl345_err_t adxl345_set_tap_thresh_g(adxl345_t *adxl345, float val);

adxl345_err_t adxl345_get_ofsx_g(adxl345_t *adxl345, float *val);
adxl345_err_t adxl345_set_ofsx_g(adxl345_t *adxl345, float val);

adxl345_err_t adxl345_get_ofsy_g(adxl345_t *adxl345, float *val);
adxl345_err_t adxl345_set_ofsy_g(adxl345_t *adxl345, float val);

adxl345_err_t adxl345_get_ofsz_g(adxl345_t *adxl345, float *val);
adxl345_err_t adxl345_set_ofsz_g(adxl345_t *adxl345, float val);

adxl345_err_t adxl345_get_dur_g(adxl345_t *adxl345, float *val);
adxl345_err_t adxl345_set_dur_g(adxl345_t *adxl345, float val);

adxl345_err_t adxl345_get_latency_s(adxl345_t *adxl345, float *val);
adxl345_err_t adxl345_set_latency_s(adxl345_t *adxl345, float val);

adxl345_err_t adxl345_get_window_s(adxl345_t *adxl345, float *val);
adxl345_err_t adxl345_set_window_s(adxl345_t *adxl345, float val);

adxl345_err_t adxl345_get_thresh_act_g(adxl345_t *adxl345, float *val);
adxl345_err_t adxl345_set_thresh_act_g(adxl345_t *adxl345, float val);

adxl345_err_t adxl345_get_thresh_inact_g(adxl345_t *adxl345, float *val);
adxl345_err_t adxl345_set_thresh_inact_g(adxl345_t *adxl345, float val);

adxl345_err_t adxl345_get_time_inact_s(adxl345_t *adxl345, float *val);
adxl345_err_t adxl345_set_time_inact_s(adxl345_t *adxl345, float val);

adxl345_err_t adxl345_get_thresh_ff_g(adxl345_t *adxl345, float *val);
adxl345_err_t adxl345_set_thresh_ff_g(adxl345_t *adxl345, float val);

adxl345_err_t adxl345_get_time_ff_s(adxl345_t *adxl345, float *val);
adxl345_err_t adxl345_set_time_ff_s(adxl345_t *adxl345, float val);

adxl345_err_t adxl345_available_samples(adxl345_t *adxl345, uint8_t *val);

adxl345_err_t adxl345_get_isample(adxl345_t *adxl345, adxl345_isample_t *sample);

/** @brief Read an x, y, z sample frame.
 */
adxl345_err_t adxl345_get_fsample(adxl345_t *adxl345, adxl345_fsample_t *sample);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ADXL345_H_ */
