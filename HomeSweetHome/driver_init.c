/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>
#include <hpl_gclk_base.h>
#include <hpl_pm_base.h>

/*! The buffer size for USART */
#define DEBUG_IO_BUFFER_SIZE 16

/*! The buffer size for USART */
#define GNSS_BUFFER_SIZE 16

struct usart_async_descriptor Debug_io;
struct usart_async_descriptor gnss;

static uint8_t Debug_io_buffer[DEBUG_IO_BUFFER_SIZE];
static uint8_t gnss_buffer[GNSS_BUFFER_SIZE];

struct flash_descriptor Clock_Offset_Flash;

struct spi_m_async_descriptor OLED;

struct i2c_m_sync_desc cmps_i2c;

void Clock_Offset_Flash_CLOCK_init(void)
{

	_pm_enable_bus_clock(PM_BUS_APBB, NVMCTRL);
}

void Clock_Offset_Flash_init(void)
{
	Clock_Offset_Flash_CLOCK_init();
	flash_init(&Clock_Offset_Flash, NVMCTRL);
}

void OLED_PORT_init(void)
{

	gpio_set_pin_level(PA04,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(PA04, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(PA04, PINMUX_PA04D_SERCOM0_PAD0);

	gpio_set_pin_level(PA05,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(PA05, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(PA05, PINMUX_PA05D_SERCOM0_PAD1);

	// Set pin direction to input
	gpio_set_pin_direction(PA06, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(PA06,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(PA06, PINMUX_PA06D_SERCOM0_PAD2);
}

void OLED_CLOCK_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM0);
	_gclk_enable_channel(SERCOM0_GCLK_ID_CORE, CONF_GCLK_SERCOM0_CORE_SRC);
}

void OLED_init(void)
{
	OLED_CLOCK_init();
	spi_m_async_init(&OLED, SERCOM0);
	OLED_PORT_init();
}

void cmps_i2c_PORT_init(void)
{

	gpio_set_pin_pull_mode(PA08,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(PA08, PINMUX_PA08D_SERCOM2_PAD0);

	gpio_set_pin_pull_mode(PA09,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_OFF);

	gpio_set_pin_function(PA09, PINMUX_PA09D_SERCOM2_PAD1);
}

void cmps_i2c_CLOCK_init(void)
{
	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM2);
	_gclk_enable_channel(SERCOM2_GCLK_ID_CORE, CONF_GCLK_SERCOM2_CORE_SRC);
	_gclk_enable_channel(SERCOM2_GCLK_ID_SLOW, CONF_GCLK_SERCOM2_SLOW_SRC);
}

void cmps_i2c_init(void)
{
	cmps_i2c_CLOCK_init();
	i2c_m_sync_init(&cmps_i2c, SERCOM2);
	cmps_i2c_PORT_init();
}

/**
 * \brief USART Clock initialization function
 *
 * Enables register interface and peripheral clock
 */
void Debug_io_CLOCK_init()
{

	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM3);
	_gclk_enable_channel(SERCOM3_GCLK_ID_CORE, CONF_GCLK_SERCOM3_CORE_SRC);
}

/**
 * \brief USART pinmux initialization function
 *
 * Set each required pin to USART functionality
 */
void Debug_io_PORT_init()
{

	gpio_set_pin_function(PA22, PINMUX_PA22C_SERCOM3_PAD0);

	gpio_set_pin_function(PA23, PINMUX_PA23C_SERCOM3_PAD1);
}

/**
 * \brief USART initialization function
 *
 * Enables USART peripheral, clocks and initializes USART driver
 */
void Debug_io_init(void)
{
	Debug_io_CLOCK_init();
	usart_async_init(&Debug_io, SERCOM3, Debug_io_buffer, DEBUG_IO_BUFFER_SIZE, (void *)NULL);
	Debug_io_PORT_init();
}

/**
 * \brief USART Clock initialization function
 *
 * Enables register interface and peripheral clock
 */
void gnss_CLOCK_init()
{

	_pm_enable_bus_clock(PM_BUS_APBC, SERCOM4);
	_gclk_enable_channel(SERCOM4_GCLK_ID_CORE, CONF_GCLK_SERCOM4_CORE_SRC);
}

/**
 * \brief USART pinmux initialization function
 *
 * Set each required pin to USART functionality
 */
void gnss_PORT_init()
{

	gpio_set_pin_function(PB08, PINMUX_PB08D_SERCOM4_PAD0);

	gpio_set_pin_function(PB09, PINMUX_PB09D_SERCOM4_PAD1);
}

/**
 * \brief USART initialization function
 *
 * Enables USART peripheral, clocks and initializes USART driver
 */
void gnss_init(void)
{
	gnss_CLOCK_init();
	usart_async_init(&gnss, SERCOM4, gnss_buffer, GNSS_BUFFER_SIZE, (void *)NULL);
	gnss_PORT_init();
}

void system_init(void)
{
	init_mcu();

	// GPIO on PA15

	// Set pin direction to input
	gpio_set_pin_direction(Switch, GPIO_DIRECTION_IN);

	gpio_set_pin_pull_mode(Switch,
	                       // <y> Pull configuration
	                       // <id> pad_pull_config
	                       // <GPIO_PULL_OFF"> Off
	                       // <GPIO_PULL_UP"> Pull-up
	                       // <GPIO_PULL_DOWN"> Pull-down
	                       GPIO_PULL_UP);

	gpio_set_pin_function(Switch, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PA17

	gpio_set_pin_level(OLED_CS,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   true);

	// Set pin direction to output
	gpio_set_pin_direction(OLED_CS, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(OLED_CS, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB16

	gpio_set_pin_level(OLED_RST,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   true);

	// Set pin direction to output
	gpio_set_pin_direction(OLED_RST, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(OLED_RST, GPIO_PIN_FUNCTION_OFF);

	// GPIO on PB17

	gpio_set_pin_level(OLED_D_C,
	                   // <y> Initial level
	                   // <id> pad_initial_level
	                   // <false"> Low
	                   // <true"> High
	                   false);

	// Set pin direction to output
	gpio_set_pin_direction(OLED_D_C, GPIO_DIRECTION_OUT);

	gpio_set_pin_function(OLED_D_C, GPIO_PIN_FUNCTION_OFF);

	Clock_Offset_Flash_init();

	OLED_init();

	cmps_i2c_init();
	Debug_io_init();
	gnss_init();
}
