/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"

void ADXL345_0_example(void)
{
	struct io_descriptor *ADXL345_0_io;

	i2c_m_sync_get_io_descriptor(&ADXL345_0, &ADXL345_0_io);
	i2c_m_sync_enable(&ADXL345_0);
	i2c_m_sync_set_slaveaddr(&ADXL345_0, 0x12, I2C_M_SEVEN);
	io_write(ADXL345_0_io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using TARGET_IO to write "Hello World" using the IO abstraction.
 */
void TARGET_IO_example(void)
{
	struct io_descriptor *io;
	usart_sync_get_io_descriptor(&TARGET_IO, &io);
	usart_sync_enable(&TARGET_IO);

	io_write(io, (uint8_t *)"Hello World!", 12);
}
