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

#include "adxl345.h"
#include "adxl345_dev.h"
#include "adxl345_err.h"

// =============================================================================
// local types and definitions

// =============================================================================
// local (forward) declarations

static adxl345_err_t get_converted_reg(adxl345_t *adxl345, uint8_t reg_id,
                                       float *val, float scale);

static adxl345_err_t set_converted_reg(adxl345_t *adxl345, uint8_t reg_id,
                                       float val, float scale);

// =============================================================================
// local storage

// =============================================================================
// public code

adxl345_err_t adxl345_init(adxl345_t *adxl345, adxl345_dev_t *dev) {
  uint8_t reg;
  adxl345->dev = dev;
  adxl345_err_t err;

  if ((err = adxl345_get_devid_reg(adxl345, &reg)) != ADXL345_ERR_NONE) {
    // failed to read device ID
    return err;

  } else if (reg != ADXL345_DEVICE_ID) {
    // device ID doesn't match
    return ADXL354_ERR_INIT;

  } else {
    // success
    return ADXL345_ERR_NONE;
  }
}

adxl345_err_t adxl345_reset(adxl345_t *adxl345) {
  adxl345_err_t err;

  // stop taking measurements first...
  err = adxl345_write_reg(adxl345, ADXL345_REG_POWER_CTL, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  // drain any data in the FIFO by performing reads until DATA_READY goes false.
  while (true) {
    uint8_t reg;
    adxl345_data_regs_t regs;

    err = adxl345_get_int_source_reg(adxl345, &reg);
    if (err != ADXL345_ERR_NONE) return err;

    // stop reading when DATA_READY bit goes false.
    if ((reg & ADXL345_DATA_READY_INT) == 0) break;

    // Read x, y, z registers
    err = adxl345_get_data_regs(adxl345, &regs);
    if (err != ADXL345_ERR_NONE) return err;
  }

  err = adxl345_write_reg(adxl345, ADXL345_REG_THRESH_TAP, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_OFSX, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_OFSY, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_OFSZ, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_DUR, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_LATENT, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_WINDOW, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_THRESH_ACT, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_THRESH_INACT, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_TIME_INACT, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_ACT_INACT_CTL, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_THRESH_FF, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_TIME_FF, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_TAP_AXES, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_BW_RATE, 0x0A, true); // !!
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_INT_ENABLE, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_INT_MAP, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_DATA_FORMAT, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_write_reg(adxl345, ADXL345_REG_FIFO_CTL, 0, true);
  if (err != ADXL345_ERR_NONE) return err;

  return err;
}

adxl345_err_t adxl345_write_reg(adxl345_t *adxl345, uint8_t reg_id, uint8_t val,
                                bool verify) {
  adxl345_err_t err;

  err = adxl345_dev_write_reg(adxl345->dev, reg_id, val);
  if ((err == ADXL345_ERR_NONE) && verify) {
    uint8_t v;
    err = adxl345_dev_read_reg(adxl345->dev, reg_id, &v);
    if (err != ADXL345_ERR_NONE) {
      return err;
    } else if (v != val) {
      return ADXL345_ERR_VERIFY;
    }
  }
  return err;
}

// ==========================================
// low-level register access

adxl345_err_t adxl345_get_devid_reg(adxl345_t *adxl345, uint8_t *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_DEVID, val);
}

adxl345_err_t adxl345_get_thresh_tap_reg(adxl345_t *adxl345, uint8_t *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_THRESH_TAP, val);
}

adxl345_err_t adxl345_set_thresh_tap_reg(adxl345_t *adxl345, uint8_t val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_THRESH_TAP, val);
}

adxl345_err_t adxl345_get_ofsx_reg(adxl345_t *adxl345, uint8_t *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_OFSX, val);
}

adxl345_err_t adxl345_set_ofsx_reg(adxl345_t *adxl345, uint8_t val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_OFSX, val);
}

adxl345_err_t adxl345_get_ofsy_reg(adxl345_t *adxl345, uint8_t *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_OFSY, val);
}

adxl345_err_t adxl345_set_ofsy_reg(adxl345_t *adxl345, uint8_t val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_OFSX, val);
}

adxl345_err_t adxl345_get_ofsz_reg(adxl345_t *adxl345, uint8_t *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_OFSZ, val);
}

adxl345_err_t adxl345_set_ofsz_reg(adxl345_t *adxl345, uint8_t val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_OFSZ, val);
}

adxl345_err_t adxl345_get_dur_reg(adxl345_t *adxl345, uint8_t *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_DUR, val);
}

adxl345_err_t adxl345_set_dur_reg(adxl345_t *adxl345, uint8_t val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_DUR, val);
}

adxl345_err_t adxl345_get_latency_reg(adxl345_t *adxl345, uint8_t *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_LATENT, val);
}

adxl345_err_t adxl345_set_latency_reg(adxl345_t *adxl345, uint8_t val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_LATENT, val);
}

adxl345_err_t adxl345_get_window_reg(adxl345_t *adxl345, uint8_t *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_WINDOW, val);
}

adxl345_err_t adxl345_set_window_reg(adxl345_t *adxl345, uint8_t val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_WINDOW, val);
}

adxl345_err_t adxl345_get_thresh_act_reg(adxl345_t *adxl345, uint8_t *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_THRESH_ACT, val);
}

adxl345_err_t adxl345_set_thresh_act_reg(adxl345_t *adxl345, uint8_t val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_THRESH_ACT, val);
}

adxl345_err_t adxl345_get_thresh_inact_reg(adxl345_t *adxl345, uint8_t *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_THRESH_INACT, val);
}

adxl345_err_t adxl345_set_thresh_inact_reg(adxl345_t *adxl345, uint8_t val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_THRESH_INACT, val);
}

adxl345_err_t adxl345_get_time_inact_reg(adxl345_t *adxl345, uint8_t *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_TIME_INACT, val);
}

adxl345_err_t adxl345_set_time_inact_reg(adxl345_t *adxl345, uint8_t val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_TIME_INACT, val);
}

adxl345_err_t adxl345_get_act_inact_ctl_reg(adxl345_t *adxl345,
                                            adxl345_act_inact_ctl_reg *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_ACT_INACT_CTL, val);
}

adxl345_err_t adxl345_set_act_inact_ctl_reg(adxl345_t *adxl345,
                                            adxl345_act_inact_ctl_reg val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_ACT_INACT_CTL, val);
}

adxl345_err_t adxl345_get_thresh_ff_reg(adxl345_t *adxl345, uint8_t *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_THRESH_FF, val);
}

adxl345_err_t adxl345_set_thresh_ff_reg(adxl345_t *adxl345, uint8_t val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_THRESH_FF, val);
}

adxl345_err_t adxl345_get_time_ff_reg(adxl345_t *adxl345, uint8_t *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_TIME_FF, val);
}

adxl345_err_t adxl345_set_time_ff_reg(adxl345_t *adxl345, uint8_t val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_TIME_FF, val);
}

adxl345_err_t adxl345_get_tap_axes_reg(adxl345_t *adxl345,
                                       adxl345_tap_axes_reg *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_TAP_AXES, val);
}

adxl345_err_t adxl345_set_tap_axes_reg(adxl345_t *adxl345,
                                       adxl345_tap_axes_reg val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_TAP_AXES, val);
}

adxl345_err_t adxl345_get_bw_rate_reg(adxl345_t *adxl345,
                                      adxl345_bw_rate_reg *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_BW_RATE, val);
}

adxl345_err_t adxl345_set_bw_rate_reg(adxl345_t *adxl345,
                                      adxl345_bw_rate_reg val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_BW_RATE, val);
}

adxl345_err_t adxl345_get_power_ctl_reg(adxl345_t *adxl345,
                                        adxl345_power_ctl_reg *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_POWER_CTL, val);
}

adxl345_err_t adxl345_set_power_ctl_reg(adxl345_t *adxl345,
                                        adxl345_power_ctl_reg val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_POWER_CTL, val);
}

adxl345_err_t adxl345_get_int_enable_reg(adxl345_t *adxl345,
                                         adxl345_interrupt_reg *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_INT_ENABLE, val);
}

adxl345_err_t adxl345_set_int_enable_reg(adxl345_t *adxl345,
                                         adxl345_interrupt_reg val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_INT_ENABLE, val);
}

adxl345_err_t adxl345_get_int_map_reg(adxl345_t *adxl345,
                                      adxl345_interrupt_reg *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_INT_MAP, val);
}

adxl345_err_t adxl345_set_int_map_reg(adxl345_t *adxl345,
                                      adxl345_interrupt_reg val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_INT_MAP, val);
}

adxl345_err_t adxl345_get_int_source_reg(adxl345_t *adxl345,
                                         adxl345_interrupt_reg *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_INT_SOURCE, val);
}

adxl345_err_t adxl345_get_data_format_reg(adxl345_t *adxl345,
                                          adxl345_data_format_reg *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_DATA_FORMAT, val);
}

adxl345_err_t adxl345_set_data_format_reg(adxl345_t *adxl345,
                                          adxl345_data_format_reg val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_DATA_FORMAT, val);
}

adxl345_err_t adxl345_get_data_regs(adxl345_t *adxl345,
                                    adxl345_data_regs_t *dst) {
  return adxl345_dev_read_regs(adxl345->dev, ADXL345_REG_DATAX0, (uint8_t *)dst,
                               sizeof(adxl345_data_regs_t));
}

adxl345_err_t adxl345_get_fifo_ctl_reg(adxl345_t *adxl345,
                                       adxl345_fifo_mode_reg *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_FIFO_CTL, val);
}

adxl345_err_t adxl345_set_fifo_ctl_reg(adxl345_t *adxl345,
                                       adxl345_fifo_mode_reg val) {
  return adxl345_dev_write_reg(adxl345->dev, ADXL345_REG_FIFO_CTL, val);
}

adxl345_err_t adxl345_get_fifo_status_reg(adxl345_t *adxl345,
                                          adxl345_fifo_status_reg *val) {
  return adxl345_dev_read_reg(adxl345->dev, ADXL345_REG_FIFO_STATUS, val);
}

// ==========================================
// higher level functions.  In the functions below,
// _g stands for gravity and _s stands for seconds.

/**
 * @brief Enter measurement mode: start measuring
 */
adxl345_err_t adxl345_start(adxl345_t *adxl345) {
  uint8_t reg;
  adxl345_err_t err;

  err = adxl345_get_power_ctl_reg(adxl345, &reg);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_set_power_ctl_reg(adxl345, reg | ADXL345_MEASURE);
  return err;
}

/**
 * @brief Enter standby mode: stop measuring
 */
adxl345_err_t adxl345_stop(adxl345_t *adxl345) {
  uint8_t reg;
  adxl345_err_t err;

  err = adxl345_get_power_ctl_reg(adxl345, &reg);
  if (err != ADXL345_ERR_NONE) return err;

  err = adxl345_set_power_ctl_reg(adxl345, reg & ~ADXL345_MEASURE);
  return err;
}

/**
 * @brief Indicate if a sample is available.
 */
adxl345_err_t adxl345_is_sample_available(adxl345_t *adxl345, bool *is_sample_available) {
  uint8_t reg;
  adxl345_err_t err;

  err = adxl345_get_int_source_reg(adxl345, &reg);
  if (err == ADXL345_ERR_NONE) {
    *is_sample_available = (reg & ADXL345_DATA_READY_INT) ? true : false;
  } else {
    *is_sample_available = false;
  }
  return err;
}

adxl345_err_t adxl345_get_tap_thresh_g(adxl345_t *adxl345, float *val) {
  return get_converted_reg(adxl345, ADXL345_REG_THRESH_TAP, val,
                           ADXL345_REG_THRESH_TAP_SCALE);
}

adxl345_err_t adxl345_set_tap_thresh_g(adxl345_t *adxl345, float val) {
  return set_converted_reg(adxl345, ADXL345_REG_THRESH_TAP, val,
                           ADXL345_REG_THRESH_TAP_SCALE);
}

adxl345_err_t adxl345_get_ofsx_g(adxl345_t *adxl345, float *val) {
  return get_converted_reg(adxl345, ADXL345_REG_OFSX, val, ADXL345_OFSx_SCALE);
}

adxl345_err_t adxl345_set_ofsx_g(adxl345_t *adxl345, float val) {
  return set_converted_reg(adxl345, ADXL345_REG_OFSX, val, ADXL345_OFSx_SCALE);
}

adxl345_err_t adxl345_get_ofsy_g(adxl345_t *adxl345, float *val) {
  return get_converted_reg(adxl345, ADXL345_REG_OFSY, val, ADXL345_OFSx_SCALE);
}

adxl345_err_t adxl345_set_ofsy_g(adxl345_t *adxl345, float val) {
  return set_converted_reg(adxl345, ADXL345_REG_OFSY, val, ADXL345_OFSx_SCALE);
}

adxl345_err_t adxl345_get_ofsz_g(adxl345_t *adxl345, float *val) {
  return get_converted_reg(adxl345, ADXL345_REG_OFSZ, val, ADXL345_OFSx_SCALE);
}

adxl345_err_t adxl345_set_ofsz_g(adxl345_t *adxl345, float val) {
  return set_converted_reg(adxl345, ADXL345_REG_OFSZ, val, ADXL345_OFSx_SCALE);
}

adxl345_err_t adxl345_get_dur_g(adxl345_t *adxl345, float *val) {
  return get_converted_reg(adxl345, ADXL345_REG_DUR, val, ADXL345_DUR_SCALE);
}

adxl345_err_t adxl345_set_dur_g(adxl345_t *adxl345, float val) {
  return set_converted_reg(adxl345, ADXL345_REG_DUR, val, ADXL345_DUR_SCALE);
}

adxl345_err_t adxl345_get_latency_s(adxl345_t *adxl345, float *val) {
  return get_converted_reg(adxl345, ADXL345_REG_LATENT, val,
                           ADXL345_LATENT_SCALE);
}

adxl345_err_t adxl345_set_latency_s(adxl345_t *adxl345, float val) {
  return set_converted_reg(adxl345, ADXL345_REG_LATENT, val,
                           ADXL345_LATENT_SCALE);
}

adxl345_err_t adxl345_get_window_s(adxl345_t *adxl345, float *val) {
  return get_converted_reg(adxl345, ADXL345_REG_WINDOW, val,
                           ADXL345_WINDOW_SCALE);
}

adxl345_err_t adxl345_set_window_s(adxl345_t *adxl345, float val) {
  return set_converted_reg(adxl345, ADXL345_REG_WINDOW, val,
                           ADXL345_WINDOW_SCALE);
}

adxl345_err_t adxl345_get_thresh_act_g(adxl345_t *adxl345, float *val) {
  return get_converted_reg(adxl345, ADXL345_REG_THRESH_ACT, val,
                           ADXL345_THRESH_ACT_SCALE);
}

adxl345_err_t adxl345_set_thresh_act_g(adxl345_t *adxl345, float val) {
  return set_converted_reg(adxl345, ADXL345_REG_THRESH_ACT, val,
                           ADXL345_THRESH_ACT_SCALE);
}

adxl345_err_t adxl345_get_thresh_inact_g(adxl345_t *adxl345, float *val) {
  return get_converted_reg(adxl345, ADXL345_REG_THRESH_INACT, val,
                           ADXL345_THRESH_INACT_SCALE);
}

adxl345_err_t adxl345_set_thresh_inact_g(adxl345_t *adxl345, float val) {
  return set_converted_reg(adxl345, ADXL345_REG_THRESH_INACT, val,
                           ADXL345_THRESH_INACT_SCALE);
}

adxl345_err_t adxl345_get_time_inact_s(adxl345_t *adxl345, float *val) {
  return get_converted_reg(adxl345, ADXL345_REG_TIME_INACT, val,
                           ADXL345_TIME_INACT_SCALE);
}

adxl345_err_t adxl345_set_time_inact_s(adxl345_t *adxl345, float val) {
  return set_converted_reg(adxl345, ADXL345_REG_TIME_INACT, val,
                           ADXL345_TIME_INACT_SCALE);
}

adxl345_err_t adxl345_get_thresh_ff_g(adxl345_t *adxl345, float *val) {
  return get_converted_reg(adxl345, ADXL345_REG_THRESH_FF, val,
                           ADXL345_THRESH_FF_SCALE);
}

adxl345_err_t adxl345_set_thresh_ff_g(adxl345_t *adxl345, float val) {
  return set_converted_reg(adxl345, ADXL345_REG_THRESH_FF, val,
                           ADXL345_THRESH_FF_SCALE);
}

adxl345_err_t adxl345_get_time_ff_s(adxl345_t *adxl345, float *val) {
  return get_converted_reg(adxl345, ADXL345_REG_TIME_FF, val,
                           ADXL345_TIME_FF_SCALE);
}

adxl345_err_t adxl345_set_time_ff_s(adxl345_t *adxl345, float val) {
  return set_converted_reg(adxl345, ADXL345_REG_TIME_FF, val,
                           ADXL345_TIME_FF_SCALE);
}

adxl345_err_t adxl345_available_samples(adxl345_t *adxl345, uint8_t *val) {
  uint8_t reg;
  adxl345_err_t err = adxl345_get_fifo_status_reg(adxl345, &reg);
  *val = (err == ADXL345_ERR_NONE) ? reg & 0x1F : 0;
  return err;
}

/** @brief Read an x, y, z sample frame.
 */
adxl345_err_t adxl345_get_isample(adxl345_t *adxl345,
                                  adxl345_isample_t *sample) {
  adxl345_data_regs_t regs;
  adxl345_err_t err;

  err = adxl345_get_data_regs(adxl345, &regs);
  if (err != ADXL345_ERR_NONE) return err;

  // Using default values:
  //   x1:x0 is a 16 bit signed value with 10 bits of resolution.
  sample->x = (int16_t)((regs.x1 << 8) | regs.x0);
  sample->y = (int16_t)((regs.y1 << 8) | regs.y0);
  sample->z = (int16_t)((regs.z1 << 8) | regs.z0);

  return ADXL345_ERR_NONE;
}

/** @brief Read an x, y, z sample frame.
 */
adxl345_err_t adxl345_get_fsample(adxl345_t *adxl345,
                                  adxl345_fsample_t *sample) {
  adxl345_data_regs_t regs;
  adxl345_err_t err;

  err = adxl345_get_data_regs(adxl345, &regs);
  if (err != ADXL345_ERR_NONE) return err;

  // Using default values:
  //   x1:x0 is a 16 bit signed value with 10 bits of resolution.
  //   Full scale is 2^9 = 512 corresponding to 2g
  //   This can change with sensitivity settings, packing mode, etc.
  sample->x = (int16_t)((regs.x1 << 8) | regs.x0) * ADXL345_2G_SCALE;
  sample->y = (int16_t)((regs.y1 << 8) | regs.y0) * ADXL345_2G_SCALE;
  sample->z = (int16_t)((regs.z1 << 8) | regs.z0) * ADXL345_2G_SCALE;

  return ADXL345_ERR_NONE;
}

// =============================================================================
// local (static) code

static adxl345_err_t get_converted_reg(adxl345_t *adxl345, uint8_t reg_id,
                                       float *val, float scale) {
  uint8_t reg;
  adxl345_err_t err = adxl345_dev_read_reg(adxl345->dev, reg_id, &reg);
  *val = (err == ADXL345_ERR_NONE) ? reg * scale : 0.0;
  return err;
}

static adxl345_err_t set_converted_reg(adxl345_t *adxl345, uint8_t reg_id,
                                       float val, float scale) {
  uint8_t reg = val / scale;
  return adxl345_dev_write_reg(adxl345->dev, reg_id, reg);
}
