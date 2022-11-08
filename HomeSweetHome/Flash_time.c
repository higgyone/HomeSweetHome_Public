/*
 * Flash_time.c
 *
 * Created: 07/10/2022 14:15:51
 *  Author: higgy
 */ 

#include "Flash_time.h"

static uint8_t write_data[128];
static uint8_t read_data[128];

void clock_offset_flash_increment()
{
	uint8_t offset = get_time_offset();
	
	offset += 1;
	
	set_time_offset(offset);
	
}

uint8_t get_time_offset()
{
	/* Read data from flash */
	flash_read(&Clock_Offset_Flash, 0x3200, read_data, 1);
	
	return read_data[0];
}

void set_time_offset(uint8_t offset)
{
	uint8_t hour_offset = offset;
	
	if (hour_offset > 23 )
	{
		hour_offset = 0;
	}
	
	write_data[0] = hour_offset;

	/* Write data to flash */
	flash_write(&Clock_Offset_Flash, 0x3200, write_data, 1);
}