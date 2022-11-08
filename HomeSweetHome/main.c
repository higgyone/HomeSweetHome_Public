#include <atmel_start.h>
#include <string.h>
#include <stdint.h>

#include "UART.h"
#include "I2C_Compass.h"
#include "OLED_SPI.h"
#include "GNSS_calculations.h"
#include "Flash_time.h"
#include "driver_init.h"
#include "hal_gpio.h"

// arbitrary button debounce wait time
#define DEBOUNCE_TIME		20

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	GNSS_uart_init();
	
	debug_uart_init();
	
	compass_init();
	
	OLED_screen_init();
	
	OLED_display_HSH();
	
	delay_ms(2500);
	
	OLED_clear_screen();
	
	debug_uart_password_display();
	
	double dist_to_base;
	volatile int32_t GNSS_bearing;
	volatile double compass_bearing;
	volatile double bearing_diff;
	uint8_t direction;

	/* Replace with your application code */
	while (1)
	{
		check_GNSS();
		
		if (GNSS_gll_received == 1)
		{
			// display OLED time
			OLED_show_time();
			
			dist_to_base = get_distance_to_base();
			
			OLED_disply_GNSS(dist_to_base);
			
			GNSS_bearing = get_GNSS_bearing_all();
			compass_bearing = compass_get_bearing();
			
			// get the bearing difference
			bearing_diff = GNSS_bearing - compass_bearing;
			
			// convert this to between 0 & 360 if compass bearing > GNSS bearing
			bearing_diff = 360 - bearing_diff;
			bearing_diff = (uint32_t)bearing_diff % 360; // cannot do mod on a double so have to convert
			
			direction = convert_bearing_to_compass_point((double)bearing_diff); // expects a double so casting it back
			
			OLED_display_bearing(direction);

			// debounce hour change button
			if (gpio_get_pin_level(Switch) == false)
			{
				// check it is still pressed after debounce wait
				delay_ms(DEBOUNCE_TIME);
				if (gpio_get_pin_level(Switch) == false)
				{
					clock_offset_flash_increment();
				}
			}
			
			// Reset Flags
			GNSS_gll_received = 0;
		}
		
		check_password();
	}
}

