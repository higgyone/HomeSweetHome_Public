/*
 * GNSS_calculations.h
 *
 * Created: 15/09/2022 09:38:59
 *  Author: higgy
 */ 

#ifndef GNSS_CALCULATIONS_H_
#define GNSS_CALCULATIONS_H_

#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <utils.h>
#include "UART.h"
#include "Flash_time.h"
#include "hidden.h"



#define DISTANCE_UNIT_KM	'K'
#define DISTANCE_UNIT_MILES	'M'
#define DISTANCE_UNIT		DISTANCE_UNIT_MILES

#define GNGLL_HEADER_SIZE	7U

#define NO_TIME				((unsigned char)0xFF)

#define INVALID_DISTANCE	((double)999999)
#define INVALID_BEARING		((double)999999)

//#define ARRAY_SIZE(arr)		sizeof(arr) / sizeof(arr[0])
#define ARRAY_BYTES(arr)        (sizeof(arr))

//const char gngll_header_data[];
//const char test_gll[];

extern const char gngll_header_data[];
extern const char test_gll[];

struct GNGLL_data gnss_data;

struct GNGLL_data
{
	char type[7];
	// latitude 1234.56789
	char latitude[11];
	// N or S
	char ns[2];
	// Long 12345.67890
	char longitude[12];
	// E or W
	char ew[2];
	// hhmmss.ss 12345.67
	// ss is nano the bit to make the time more precise. Not necessary for this
	char time[10];
	// A is valid data
	// D is invalid data
	char status[2];
	// A is dead reckoning
	char posMode[2];
	char checksum [3];
};

extern void populate_GNGLL_data(char *GNGLL_buffer);
extern double get_distance_to_base(void);
extern int32_t get_GNSS_bearing_all(void);
extern double convert_latitude_GNSS_to_decimal(void);
extern double convert_longitude_GNSS_to_decimal(void);

extern uint8_t get_hours(void);
extern uint8_t get_minutes(void);
extern uint8_t get_seconds(void);


#endif /* GNSS_CALCULATIONS_H_ */