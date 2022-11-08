/*
 * OLED_SPI.h
 *
 * Created: 05/09/2022 20:32:55
 *  Author: higgy
 */ 


#ifndef OLED_SPI_H_
#define OLED_SPI_H_

// Include Drivers
#include "driver_init.h"
#include "Fonts.h"
#include "GNSS_calculations.h"
#include <string.h>
#include <stdbool.h>

// IO descriptor for OLED
struct io_descriptor *oled_io;

// OLED functions
extern void OLED_screen_init(void);
extern void OLED_reset(void);
extern bool OLED_write(uint16_t length, const uint8_t *buf);
extern bool OLED_write_data(uint16_t length, const uint8_t *buf);
extern void OLED_On_Off(void);
extern void OLED_test(void);
extern void OLED_display_setup(void);
extern void OLED_clear_screen(void);
extern void OLED_set_full_screen(void);
extern void OLED_set_position(uint8_t x, uint8_t y, uint8_t deltax, uint8_t deltay);
extern void OLED_show_time(void);
extern void OLED_disply_GNSS(double dist);
extern void OLED_display_bearing(uint8_t direction);
extern void OLED_display_HSH(void);

// display positions
#define TIME_START_POS_X						((unsigned char)0x04)
#define TIME_START_POS_Y						((unsigned char)0x20)

#define GNSS_START_POS_X						((unsigned char)0x04)
#define GNSS_START_POS_Y						((unsigned char)0x50)
#define NO_GNSS_START_POS_X						((unsigned char)0x10)
#define NO_GNSS_START_POS_Y						((unsigned char)0x50)

#define COMPASS_MID_POS_X						((unsigned char)0x36)	
#define COMPASS_POS_Y							((unsigned char)0x00)	

#define HOME_START_POS_X						((unsigned char)0x22)	
#define SWEET_START_POS_X						((unsigned char)0x18)	

#define HOME1_START_POS_Y						((unsigned char)0x1A)	
#define SWEET_START_POS_Y						((unsigned char)0x3A)		
#define HOME2_START_POS_Y						((unsigned char)0x5A)

#define OLED_COMMAND							((unsigned char)0x00)
#define OLED_DATA								((unsigned char)0x01)

#define OLED_CHIP_SELECT						((unsigned char)0x00)
#define OLED_CHIP_DESELECT						((unsigned char)0x01)

// Command codes
#define OLED_START_POS_0						((unsigned char)0x00) // start position for cursor
#define OLED_MAX_COLUMNS						((unsigned char)0x3f) // each column has two segments
#define OLED_MAX_ROWS							((unsigned char)0x7f) // 

#define OLED_MAX_CONTRAST						((unsigned char)0x7f)
#define OLED_COLORMASK							((unsigned char)0x0f)
#define OLED_COLORSHIFT							((unsigned char)0x04)
#define OLED_PIXELSPERBYTE						((unsigned char)0x02)
#define OLED_SETCOLUMNADDRESS					((unsigned char)0x15)
#define OLED_SETROWADDRESS						((unsigned char)0x75)
#define OLED_SETCONTRAST						((unsigned char)0x81)
#define OLED_SETREMAP							((unsigned char)0xA0)
#define OLED_SETSTARTLINE						((unsigned char)0xA1)
#define OLED_SETOFFSET							((unsigned char)0xA2)

/*** Display Mode ***/
#define OLED_NORMALDISPLAY						((unsigned char)0xA4)
#define OLED_DISPLAYALLON						((unsigned char)0xA5)
#define OLED_DISPLAYALLOFF						((unsigned char)0xA6)
#define OLED_INVERTDISPLAY						((unsigned char)0xA7)
/*******************/

#define OLED_SETMUXRATIO						((unsigned char)0xA8)
#define OLED_FUNCTIONSELECTIONA					((unsigned char)0xAB)

/*** Display ON/OFF ***/
#define OLED_DISPLAYOFF							((unsigned char)0xAE)
#define OLED_DISPLAYON							((unsigned char)0xAF)
/**********************/

#define OLED_SETPHASELENGTH						((unsigned char)0xB1)
#define OLED_SETFRONTCLOCKDIVIDER				((unsigned char)0xB3)
#define OLED_SETGPIO							((unsigned char)0xB5)
#define OLED_SETSECONDPRECHARGEPERIOD			((unsigned char)0xB6)
#define OLED_SETGRAYSCALETABLE					((unsigned char)0xB8)
#define OLED_SELECTDEFAULTLINEARGRAYSCALETABLE	((unsigned char)0xB9)
#define OLED_SETPRECHARGEVOLTAGE				((unsigned char)0xBC)
#define OLED_SETVCOMHVOLTAGE					((unsigned char)0xBE)
#define OLED_FUNCTIONSELECTIONB					((unsigned char)0xD5)
#define OLED_SETCOMMANDLOCK						((unsigned char)0xFD)

/*** continuous horizontal scroll ***/
#define OLED_HORIZONTALSCROLLRIGHTSETUP			((unsigned char)0x26)
#define OLED_HORIZONTALSCROLLLEFTSETUP			((unsigned char)0x27)
/************************************/

#define OLED_DEACTIVATESCROLL					((unsigned char)0x2E)
#define OLED_ACTIVATESCROLL						((unsigned char)0x2F)



#endif /* OLED_SPI_H_ */