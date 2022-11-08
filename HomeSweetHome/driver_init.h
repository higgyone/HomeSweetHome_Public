/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_INCLUDED
#define DRIVER_INIT_INCLUDED

#include "atmel_start_pins.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>

#include <hal_flash.h>

#include <hal_spi_m_async.h>

#include <hal_i2c_m_sync.h>
#include <hal_usart_async.h>
#include <hal_usart_async.h>

extern struct flash_descriptor Clock_Offset_Flash;

extern struct spi_m_async_descriptor OLED;

extern struct i2c_m_sync_desc        cmps_i2c;
extern struct usart_async_descriptor Debug_io;
extern struct usart_async_descriptor gnss;

void Clock_Offset_Flash_init(void);
void Clock_Offset_Flash_CLOCK_init(void);

void OLED_PORT_init(void);
void OLED_CLOCK_init(void);
void OLED_init(void);

void cmps_i2c_CLOCK_init(void);
void cmps_i2c_init(void);
void cmps_i2c_PORT_init(void);

void Debug_io_PORT_init(void);
void Debug_io_CLOCK_init(void);
void Debug_io_init(void);

void gnss_PORT_init(void);
void gnss_CLOCK_init(void);
void gnss_init(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_INCLUDED
