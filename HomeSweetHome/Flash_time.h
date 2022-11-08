/*
 * Flash_time.h
 *
 * Created: 07/10/2022 14:15:28
 *  Author: higgy
 */ 


#ifndef FLASH_TIME_H_
#define FLASH_TIME_H_

#include <stdint.h>
#include <atmel_start.h>

extern void clock_offset_flash_increment(void);
extern uint8_t get_time_offset(void);
extern void set_time_offset(uint8_t offset);


#endif /* FLASH_TIME_H_ */