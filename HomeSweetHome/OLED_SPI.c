/*
 * OLED_SPI.c
 *
 * Created: 05/09/2022 20:42:06
 *  Author: higgy
 */ 

#include "OLED_SPI.h"

void OLED_NoGPS_Display();
void OLED_display_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
void OLED_screen_setup(void);
static void OLED_write_command_2(unsigned char c, unsigned char d);
static void OLED_write_command(unsigned char c);
void OLED_clear_GPS(void);

// just a guess to max size of any return
static uint8_t rxBuff[0x64];
static uint8_t txBuff[0x64];
	
bool GNSS_Good = false;
bool previous_GNSS_Good = false;

bool decimal_point_display = true;
bool prev_decimal_point_display = true;

bool no_time_toggle = true;

								   
uint8_t *numbers[10] = { zero_consolas,
						 one_consolas,	
						 two_consolas,
						 three_consolas,
						 four_consolas,
						 five_consolas,
						 six_consolas,
						 seven_consolas,
						 eight_consolas,
						 nine_consolas };

/**
 * Callback function for SPI transfer completed on EXTFLASH SPI.
 *
 * @param spi_m_async_descriptor		IO descriptor for SPI object.
 *
 * @return void
 */
static void SPI_OLED_complete_cb(const struct spi_m_async_descriptor *const io_descr)
{
	/* Transfer Completed */
}


/**
 * Method for initialising SPI on the EXTFLASH pins.
 *
 * @return void
 */
void OLED_screen_init()
{
	// Get the IO Descriptor
	spi_m_async_get_io_descriptor(&OLED, &oled_io);
	
	// Register the Callback Function
	spi_m_async_register_callback(&OLED, SPI_M_ASYNC_CB_XFER, (FUNC_PTR)SPI_OLED_complete_cb);
	
	// Enable Asynchronous SPI
	spi_m_async_enable(&OLED);
	
	OLED_screen_setup();
}

void OLED_screen_setup(void)
{
	OLED_display_setup();
	OLED_clear_screen();
}

void OLED_reset()
{
	gpio_set_pin_level(OLED_RST, 1);
	delay_ms(10);
	gpio_set_pin_level(OLED_RST, 0);
	delay_ms(10);
	gpio_set_pin_level(OLED_RST, 1);
	delay_ms(10);
}

// Send a single byte command to the OLED controller
static void OLED_write_command(unsigned char c)
{
	txBuff[0] = OLED_COMMAND; // command introducer
	txBuff[1] = c;
	OLED_write(2, txBuff);
}

static void OLED_write_command_2(unsigned char c, unsigned char d)
{
	txBuff[0] = OLED_COMMAND;
	txBuff[1] = c;
	txBuff[2] = d;
	OLED_write(3, txBuff);
} 


void OLED_power(unsigned char bOn)
{
	if (bOn)
	OLED_write_command(OLED_DISPLAYON); // turn on OLED
	else
	OLED_write_command(OLED_DISPLAYOFF); // turn off OLED
	} /* oledPower() */

void OLED_On_Off()
{
	OLED_power(true);
	
	delay_ms(1000);
	
	OLED_power(false);
	
}

/**
 * Send a write request using 24-bit addressing.
 *
 * @param uint8_t length		The number of bytes to write.
 * @param uint8_t *buf		The buffer where data to be written is stored. Must be at least {length} bytes in size.
 *
 * @return bool
 */
bool OLED_write(uint16_t length, const uint8_t *buf)
{
	// Status of SPI async
	struct spi_m_async_status p_stat;
		
	// data versus command
	gpio_set_pin_level(OLED_D_C, buf[0]);
	
	// Start the Combined Write Sequence
	gpio_set_pin_level(OLED_CS, OLED_CHIP_SELECT);
		
		
	// Now write data. Start at 2nd byte in the buffer as the first is whether to assert the data/command level
	if (spi_m_async_transfer(&OLED, &buf[1], rxBuff, length-1) == ERR_NONE)
	{
		// Wait until transfer completes
		while (spi_m_async_get_status(&OLED, &p_stat) == ERR_BUSY)
		{
			// Delay and Re-Check
			delay_us(10);
		}
	}
	else
	{
		// Failure
		gpio_set_pin_level(OLED_CS, OLED_CHIP_DESELECT);
			
		// Return FALSE
		return false;
	}
		
	// Complete the combined write sequence
	gpio_set_pin_level(OLED_CS, OLED_CHIP_DESELECT);
	
	// Return TRUE
	return true;
}

bool OLED_write_data(uint16_t length, const uint8_t *buf)
{
	// Status of SPI async
	struct spi_m_async_status p_stat;
	
	// data versus command
	gpio_set_pin_level(OLED_D_C, OLED_DATA);
	
	// Start the Combined Write Sequence
	gpio_set_pin_level(OLED_CS, OLED_CHIP_SELECT);
	
	
	// Now write data. Start at 2nd byte in the buffer as the first is whether to assert the data/command level
	if (spi_m_async_transfer(&OLED, &buf[0], rxBuff, length) == ERR_NONE)
	{
		// Wait until transfer completes
		while (spi_m_async_get_status(&OLED, &p_stat) == ERR_BUSY)
		{
			// Delay and Re-Check
			delay_us(10);
		}
	}
	else
	{
		// Failure
		gpio_set_pin_level(OLED_CS, OLED_CHIP_DESELECT);
		
		return false;
	}
	
	// Complete the combined write sequence
	gpio_set_pin_level(OLED_CS, OLED_CHIP_DESELECT);
	
	return true;
}

//
// Send commands to position the "cursor" (aka memory write address)
// to the given row and column as well as the ending col/row
//
void OLED_set_position(uint8_t x, uint8_t y, uint8_t deltax, uint8_t deltay)
{
	txBuff[0] = OLED_COMMAND; // command introducer
	txBuff[1] = OLED_SETCOLUMNADDRESS; // column start/end

	txBuff[2] = x/2; // start address
	txBuff[3] = (uint8_t)(((x+deltax)/2)-1); // end address
	
	txBuff[4] = OLED_SETROWADDRESS; // row start/end
	txBuff[5] = y; // start row
	txBuff[6] = y+deltay-1; // end row
	
	OLED_write(7, txBuff);
	
} /* ssd1327SetPosition() */

void OLED_display_setup()
{
	/*
	* All values here taken from OLED datasheet
	*/
	
	OLED_write_command(OLED_DISPLAYOFF); // display off
	OLED_write_command_2(OLED_SETCONTRAST, 0x9B); // set contrast control	
	OLED_write_command_2(OLED_SETREMAP, 0x51); // set remap	
	OLED_write_command_2(OLED_SETSTARTLINE, 0x00); // set display start line	
	OLED_write_command_2(OLED_SETOFFSET, 0x00); // set display offset	
	OLED_write_command(OLED_NORMALDISPLAY); // set display mode	
	OLED_write_command_2(OLED_SETMUXRATIO, 0x7F); // set mux ratio	
	OLED_write_command_2(OLED_FUNCTIONSELECTIONA, 0x01); // set function selection A	
	OLED_write_command_2(OLED_SETPHASELENGTH, 0xF1); // set phase length	
	OLED_write_command_2(OLED_SETFRONTCLOCKDIVIDER, 0x00); // set display clock divide ratio
	OLED_write_command_2(OLED_SETPRECHARGEVOLTAGE, 0x07); // set pre-charge period	
	OLED_write_command_2(OLED_SETVCOMHVOLTAGE, 0x07); // set VcomH voltage	
	OLED_write_command_2(OLED_SETSECONDPRECHARGEPERIOD, 0x0F); // set 2nd pre-charge period	
	OLED_write_command_2(OLED_FUNCTIONSELECTIONB, 0x62); // set function selection B	
	OLED_write_command(OLED_INVERTDISPLAY); // this is the way I ended up doing it	
	OLED_write_command(OLED_DISPLAYON); // set display on
}

void OLED_clear_screen()
{
	OLED_set_full_screen();	
		
	for (uint8_t i = 0; i < 128; i++)
	{
		OLED_write_data(ARRAY_BYTES(clearline), clearline);
	}	
}

void OLED_clear_GPS()
{
	OLED_set_position(0, NO_GNSS_START_POS_Y, 128, LETTER_HEIGHT);
		
	for (uint8_t i = 0; i < LETTER_HEIGHT; i++)
	{
		OLED_write_data(ARRAY_BYTES(clearline), clearline);
	}	
}

void OLED_set_full_screen()
{
		txBuff[0] = OLED_COMMAND; // command introducer
		txBuff[1] = OLED_SETCOLUMNADDRESS; // column start/end

		txBuff[2] = OLED_START_POS_0; // start address
		txBuff[3] = OLED_MAX_COLUMNS; // end address
		
		txBuff[4] = OLED_SETROWADDRESS; // row start/end
		txBuff[5] = OLED_START_POS_0; // start row
		txBuff[6] = OLED_MAX_ROWS; // end row
		
		OLED_write(7, txBuff);
}

void OLED_test()
{
	OLED_NoGPS_Display();
	OLED_display_time(12, 34, 56);
}

void OLED_NoGPS_Display()
{
	uint8_t initial_pos_x = NO_GNSS_START_POS_X;
	
	OLED_set_position(initial_pos_x, NO_GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
	OLED_write_data(ARRAY_BYTES(N_consolas),N_consolas);
	
	initial_pos_x += LETTER_WIDTH + CHAR_SPACE;
	
	OLED_set_position(initial_pos_x, NO_GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
	OLED_write_data(ARRAY_BYTES(o_lower_consolas),o_lower_consolas);
	
	initial_pos_x += LETTER_WIDTH + COLON_WIDTH;
	
	OLED_set_position(initial_pos_x, NO_GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
	OLED_write_data(ARRAY_BYTES(G_upper_consolas),G_upper_consolas);
	
	initial_pos_x += LETTER_WIDTH + CHAR_SPACE;
	
	OLED_set_position(initial_pos_x, NO_GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
	OLED_write_data(ARRAY_BYTES(P_upper_consolas),P_upper_consolas);
	
	initial_pos_x += LETTER_WIDTH; // to make it look better
	
	OLED_set_position(initial_pos_x, NO_GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
	OLED_write_data(ARRAY_BYTES(S_upper_consolas),S_upper_consolas);
}

void OLED_show_time()
{
	uint8_t hours = get_hours();
	uint8_t minutes = get_minutes();
	uint8_t seconds = get_seconds();
	
	OLED_display_time(hours,minutes, seconds);
}

void OLED_display_time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	uint8_t *display_time [6] = {0x00};
		
	uint8_t hour_tens;
	uint8_t hour_units;
	uint8_t minutes_tens;
	uint8_t minutes_units;
	uint8_t seconds_tens;
	uint8_t seconds_units;	
	
	uint8_t *dashes;
	
	if (hours == NO_TIME || minutes == NO_TIME || seconds == NO_TIME)
	{
		if (no_time_toggle == true)
		{
			dashes = dash_consolas;
		}
		else
		{
			dashes = Blank_Letter;
		}
		
		for (uint8_t i = 0; i < 6; i++)
		{
			display_time[i] = dashes;
		}
		
		no_time_toggle = !no_time_toggle;
	} 
	else
	{
		hour_tens = (uint8_t)(hours/10);
		display_time[0] = numbers[hour_tens];
		
		hour_units = (uint8_t)(hours % 10);
		display_time[1] = numbers[hour_units];
		
		minutes_tens = (uint8_t)(minutes/10);
		display_time[2] = numbers[minutes_tens];
		
		minutes_units = (uint8_t)(minutes % 10);
		display_time[3] = numbers[minutes_units];
		
		seconds_tens = (uint8_t)(seconds/10);
		display_time[4] = numbers[seconds_tens];
		
		seconds_units = (uint8_t)(seconds % 10);
		display_time[5] = numbers[seconds_units];
	}
	
	uint8_t pos_x = TIME_START_POS_X;
	
	/*****hours*****/
	OLED_set_position(pos_x, TIME_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
	OLED_write_data(ARRAY_BYTES(nine_consolas),display_time[0]);
	pos_x += LETTER_WIDTH + CHAR_SPACE;
		
	OLED_set_position(pos_x, TIME_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
	OLED_write_data(ARRAY_BYTES(nine_consolas),display_time[1]);
	pos_x += LETTER_WIDTH + CHAR_SPACE;
	
	OLED_set_position(pos_x, TIME_START_POS_Y, COLON_WIDTH, LETTER_HEIGHT);
	OLED_write_data(ARRAY_BYTES(colon_consolas),colon_consolas);
	pos_x +=  COLON_WIDTH + 1; // +1 makes it space a bit nicer
	
	/******* minutes*****/
	OLED_set_position(pos_x, TIME_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
	OLED_write_data(ARRAY_BYTES(nine_consolas),display_time[2]);
	pos_x += LETTER_WIDTH + CHAR_SPACE;
	
	OLED_set_position(pos_x, TIME_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
	OLED_write_data(ARRAY_BYTES(nine_consolas),display_time[3]);
	pos_x += LETTER_WIDTH + CHAR_SPACE;
	
	OLED_set_position(pos_x, TIME_START_POS_Y, COLON_WIDTH, LETTER_HEIGHT);
	OLED_write_data(ARRAY_BYTES(colon_consolas),colon_consolas);
	pos_x += COLON_WIDTH + 1; // +1 makes it space a bit nicer
	
	/***** seconds******/
	OLED_set_position(pos_x, TIME_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
	OLED_write_data(ARRAY_BYTES(nine_consolas),display_time[4]);
	pos_x += LETTER_WIDTH + CHAR_SPACE;
	
	OLED_set_position(pos_x, TIME_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
	OLED_write_data(ARRAY_BYTES(nine_consolas),display_time[5]);
	pos_x += LETTER_WIDTH + CHAR_SPACE;
}

void OLED_disply_GNSS(double dist)
{
	double new_dist = dist;
	uint32_t dist_int = (uint32_t)(dist); // this is to work out the character spacing on screen
	uint8_t tens_thousands = 0; 
	uint8_t thousands = 0; 
	uint8_t hundreds = 0; 
	uint8_t tens = 0; 
	uint8_t units = 0; 
	uint8_t one_dp = 0; 
	
	uint8_t start_pos_x = GNSS_START_POS_X;
	
	
	if (get_seconds() == NO_TIME || (uint32_t)dist >= (uint32_t)(INVALID_DISTANCE))
	{
		GNSS_Good = false;
		
		if (previous_GNSS_Good != GNSS_Good)
		{
			OLED_clear_GPS();
		}
		
		OLED_NoGPS_Display();
	} 
	else
	{
		GNSS_Good = true;
		
		if (previous_GNSS_Good != GNSS_Good)
		{
			OLED_clear_GPS();
		}
		
		if (new_dist >= 10000)
		{
			tens_thousands = (uint8_t)(new_dist/10000);
			
			new_dist -= tens_thousands * 10000; 
		}
		
		if (new_dist >= 1000)
		{
			thousands = (uint8_t)(new_dist/1000);
			
			new_dist -= thousands * 1000;
		}
		
		if (new_dist >= 100)
		{
			hundreds = (uint8_t)(new_dist/100);
			
			new_dist -= hundreds * 100;
		}
		
		if (new_dist >= 10)
		{
			tens = (uint8_t)(new_dist/10);
			
			new_dist -= tens * 10;
		}
		
		if (new_dist >= 1)
		{
			units = (uint8_t)(new_dist);
			
			new_dist -= units;
		}
		
		one_dp = (uint8_t)(new_dist * 10);
	
	
		if (dist_int < 100)
		{
			decimal_point_display = true;
		}
		else
		{
			decimal_point_display = false;
		}
	
		if (decimal_point_display)
		{
			start_pos_x += LETTER_WIDTH + CHAR_SPACE + DECIMAL_POINT_WIDTH; // dp width is only half a character so move it along abit more to make it nice looking
		
			if (dist_int >= 10)
			{
				OLED_set_position(start_pos_x, GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
				OLED_write_data(ARRAY_BYTES(nine_consolas), numbers[tens]);
			}
		
			start_pos_x += LETTER_WIDTH + CHAR_SPACE;
		
			OLED_set_position(start_pos_x, GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
			OLED_write_data(ARRAY_BYTES(nine_consolas), numbers[units]);
			start_pos_x += LETTER_WIDTH + CHAR_SPACE;
		
			OLED_set_position(start_pos_x, GNSS_START_POS_Y, DECIMAL_POINT_WIDTH, LETTER_HEIGHT);
			OLED_write_data(ARRAY_BYTES(decimal_point_consolas),decimal_point_consolas);
			start_pos_x += DECIMAL_POINT_WIDTH + 1; // +1 makes it space a bit nicer
		
			OLED_set_position(start_pos_x, GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
			OLED_write_data(ARRAY_BYTES(nine_consolas), numbers[one_dp]);
		}
		else
		{
			OLED_set_position(start_pos_x, GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		
			if (dist_int >= 10000)
			{
				OLED_write_data(ARRAY_BYTES(nine_consolas),numbers[tens_thousands]);
			}
			else
			{
				OLED_write_data(ARRAY_BYTES(nine_consolas),Blank_Letter);
			}
		
			start_pos_x += LETTER_WIDTH + CHAR_SPACE;		
			OLED_set_position(start_pos_x, GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		
			if (dist_int >= 1000)
			{
				OLED_write_data(ARRAY_BYTES(nine_consolas),numbers[thousands]);
			}
			else
			{
				OLED_write_data(ARRAY_BYTES(nine_consolas),Blank_Letter);
			}
		
			start_pos_x += LETTER_WIDTH + CHAR_SPACE;
			OLED_set_position(start_pos_x, GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		
			if (dist_int >= 100)
			{
				OLED_write_data(ARRAY_BYTES(nine_consolas),numbers[hundreds]);
			}
			else
			{
				OLED_write_data(ARRAY_BYTES(nine_consolas),Blank_Letter);
			}
		
			start_pos_x += LETTER_WIDTH + CHAR_SPACE;
			OLED_set_position(start_pos_x, GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		
			if (dist_int >= 10)
			{
				OLED_write_data(ARRAY_BYTES(nine_consolas),numbers[tens]);
			}
			else
			{
				OLED_write_data(ARRAY_BYTES(nine_consolas),Blank_Letter);
			}
		
			start_pos_x += LETTER_WIDTH + CHAR_SPACE;
			OLED_set_position(start_pos_x, GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		
			OLED_write_data(ARRAY_BYTES(nine_consolas),numbers[units]);
		}
	
		start_pos_x += LETTER_WIDTH + CHAR_SPACE + 1;	// +1 makes it space a bit nicer
		OLED_set_position(start_pos_x, GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
	
		if (DISTANCE_UNIT == DISTANCE_UNIT_MILES)
		{
			OLED_write_data(ARRAY_BYTES(nine_consolas), m_lower_consolas);
		}
		else
		{
			OLED_write_data(ARRAY_BYTES(nine_consolas), k_lower_consolas);
		}
	
		start_pos_x += LETTER_WIDTH + CHAR_SPACE + 1;	// +1 makes it space a bit nicer
		OLED_set_position(start_pos_x, GNSS_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
	
		if (DISTANCE_UNIT == DISTANCE_UNIT_MILES)
		{
			OLED_write_data(ARRAY_BYTES(nine_consolas), i_lower_consolas);
		} 
		else
		{
			OLED_write_data(ARRAY_BYTES(nine_consolas), m_lower_consolas);
		}
	}
	
	previous_GNSS_Good = GNSS_Good;
	prev_decimal_point_display = decimal_point_display;
}

void OLED_display_bearing(uint8_t direction)
{
	OLED_set_position(COMPASS_MID_POS_X, COMPASS_POS_Y, ARROW_WIDTH, ARROW_HEIGHT);
	
	uint8_t *display;
	
	switch (direction)
	{
		case 1:
			display = North_Arrow;
		break;
		case 2:
			display = North_West_Arrow;
		break;
		case 3:
			display = West_Arrow;
		break;
		case 4:
			display = South_West_Arrow;
		break;
		case 5:
			display = South_Arrow;
		break;
		case 6:
			display = South_East_Arrow;
		break;
		case 7:
			display = East_Arrow;
		break;
		case 8:
			display = North_East_Arrow;
		break;
		default:
			display = Blank_Arrow;
		break;
	}
	
	OLED_write_data(ARRAY_BYTES(North_Arrow), display);
	
}

void OLED_display_HSH()
{
	uint8_t pos_x = HOME_START_POS_X;
	
		OLED_set_position(pos_x, HOME1_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		OLED_write_data(ARRAY_BYTES(nine_consolas),H_upper_consolas);
		pos_x += LETTER_WIDTH + CHAR_SPACE;
		
		OLED_set_position(pos_x, HOME1_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		OLED_write_data(ARRAY_BYTES(nine_consolas), o_lower_consolas);
		pos_x += LETTER_WIDTH + CHAR_SPACE;
		
		OLED_set_position(pos_x, HOME1_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		OLED_write_data(ARRAY_BYTES(nine_consolas), m_lower_consolas);
		pos_x +=  LETTER_WIDTH + CHAR_SPACE;
		
		OLED_set_position(pos_x, HOME1_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		OLED_write_data(ARRAY_BYTES(nine_consolas), e_lower_consolas);
		
		pos_x = SWEET_START_POS_X;
		
		OLED_set_position(pos_x, SWEET_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		OLED_write_data(ARRAY_BYTES(nine_consolas),S_upper_consolas);
		pos_x += LETTER_WIDTH + CHAR_SPACE;
		
		OLED_set_position(pos_x, SWEET_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		OLED_write_data(ARRAY_BYTES(nine_consolas), w_lower_consolas);
		pos_x += LETTER_WIDTH + CHAR_SPACE;
		
		OLED_set_position(pos_x, SWEET_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		OLED_write_data(ARRAY_BYTES(nine_consolas), e_lower_consolas);
		pos_x +=  LETTER_WIDTH + CHAR_SPACE;
		
		OLED_set_position(pos_x, SWEET_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		OLED_write_data(ARRAY_BYTES(nine_consolas), e_lower_consolas);
		pos_x +=  LETTER_WIDTH + CHAR_SPACE;
		
		OLED_set_position(pos_x, SWEET_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		OLED_write_data(ARRAY_BYTES(nine_consolas), t_lower_consolas);
		
		
		pos_x = HOME_START_POS_X;
		OLED_set_position(pos_x, HOME2_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		OLED_write_data(ARRAY_BYTES(nine_consolas),H_upper_consolas);
		pos_x += LETTER_WIDTH + CHAR_SPACE;
		
		OLED_set_position(pos_x, HOME2_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		OLED_write_data(ARRAY_BYTES(nine_consolas), o_lower_consolas);
		pos_x += LETTER_WIDTH + CHAR_SPACE;
		
		OLED_set_position(pos_x, HOME2_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		OLED_write_data(ARRAY_BYTES(nine_consolas), m_lower_consolas);
		pos_x +=  LETTER_WIDTH + CHAR_SPACE;
		
		OLED_set_position(pos_x, HOME2_START_POS_Y, LETTER_WIDTH, LETTER_HEIGHT);
		OLED_write_data(ARRAY_BYTES(nine_consolas), e_lower_consolas);
}