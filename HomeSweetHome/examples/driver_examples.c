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

static uint8_t src_data[128];
static uint8_t chk_data[128];
/**
 * Example of using Clock_Offset_Flash to read and write Flash main array.
 */
void Clock_Offset_Flash_example(void)
{
	uint32_t page_size;
	uint16_t i;

	/* Init source data */
	page_size = flash_get_page_size(&Clock_Offset_Flash);

	for (i = 0; i < page_size; i++) {
		src_data[i] = i;
	}

	/* Write data to flash */
	flash_write(&Clock_Offset_Flash, 0x3200, src_data, page_size);

	/* Read data from flash */
	flash_read(&Clock_Offset_Flash, 0x3200, chk_data, page_size);
}

/**
 * Example of using OLED to write "Hello World" using the IO abstraction.
 *
 * Since the driver is asynchronous we need to use statically allocated memory for string
 * because driver initiates transfer and then returns before the transmission is completed.
 *
 * Once transfer has been completed the tx_cb function will be called.
 */

static uint8_t example_OLED[12] = "Hello World!";

static void complete_cb_OLED(const struct spi_m_async_descriptor *const io_descr)
{
	/* Transfer completed */
}

void OLED_example(void)
{
	struct io_descriptor *io;
	spi_m_async_get_io_descriptor(&OLED, &io);

	spi_m_async_register_callback(&OLED, SPI_M_ASYNC_CB_XFER, (FUNC_PTR)complete_cb_OLED);
	spi_m_async_enable(&OLED);
	io_write(io, example_OLED, 12);
}

void cmps_i2c_example(void)
{
	struct io_descriptor *cmps_i2c_io;

	i2c_m_sync_get_io_descriptor(&cmps_i2c, &cmps_i2c_io);
	i2c_m_sync_enable(&cmps_i2c);
	i2c_m_sync_set_slaveaddr(&cmps_i2c, 0x12, I2C_M_SEVEN);
	io_write(cmps_i2c_io, (uint8_t *)"Hello World!", 12);
}

/**
 * Example of using Debug_io to write "Hello World" using the IO abstraction.
 *
 * Since the driver is asynchronous we need to use statically allocated memory for string
 * because driver initiates transfer and then returns before the transmission is completed.
 *
 * Once transfer has been completed the tx_cb function will be called.
 */

static uint8_t example_Debug_io[12] = "Hello World!";

static void tx_cb_Debug_io(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
}

void Debug_io_example(void)
{
	struct io_descriptor *io;

	usart_async_register_callback(&Debug_io, USART_ASYNC_TXC_CB, tx_cb_Debug_io);
	/*usart_async_register_callback(&Debug_io, USART_ASYNC_RXC_CB, rx_cb);
	usart_async_register_callback(&Debug_io, USART_ASYNC_ERROR_CB, err_cb);*/
	usart_async_get_io_descriptor(&Debug_io, &io);
	usart_async_enable(&Debug_io);

	io_write(io, example_Debug_io, 12);
}

/**
 * Example of using gnss to write "Hello World" using the IO abstraction.
 *
 * Since the driver is asynchronous we need to use statically allocated memory for string
 * because driver initiates transfer and then returns before the transmission is completed.
 *
 * Once transfer has been completed the tx_cb function will be called.
 */

static uint8_t example_gnss[12] = "Hello World!";

static void tx_cb_gnss(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
}

void gnss_example(void)
{
	struct io_descriptor *io;

	usart_async_register_callback(&gnss, USART_ASYNC_TXC_CB, tx_cb_gnss);
	/*usart_async_register_callback(&gnss, USART_ASYNC_RXC_CB, rx_cb);
	usart_async_register_callback(&gnss, USART_ASYNC_ERROR_CB, err_cb);*/
	usart_async_get_io_descriptor(&gnss, &io);
	usart_async_enable(&gnss);

	io_write(io, example_gnss, 12);
}
