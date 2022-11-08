/*
 * I2C_Compass.h
 *
 * Created: 03/09/2022 11:44:16
 *  Author: higgy
 */ 


#ifndef I2C_COMPASS_H_
#define I2C_COMPASS_H_

#include <math.h>
#include <string.h>
// Include Drivers
#include "driver_init.h"

// Struct for I2C IO
struct io_descriptor *cmps_io;

// Define some commands
#define CMPS_CMD_XOUT_LOW			((unsigned char)0x00)
#define CMPS_CMD_XOUT_HIGH			((unsigned char)0x01)
#define CMPS_CMD_YOUT_LOW			((unsigned char)0x02)
#define CMPS_CMD_YOUT_HIGH			((unsigned char)0x03)
#define CMPS_CMD_ZOUT_LOW			((unsigned char)0x04)
#define CMPS_CMD_ZOUT_HIGH			((unsigned char)0x05)
#define CMPS_CMD_STATUS				((unsigned char)0x06)
#define CMPS_CMD_INT_CTRL_0			((unsigned char)0x07)
#define CMPS_CMD_INT_CTRL_1			((unsigned char)0x08)
#define CMPS_CMD_R0					((unsigned char)0x1B)
#define CMPS_CMD_R1					((unsigned char)0x1C)
#define CMPS_CMD_R2					((unsigned char)0x1D)
#define CMPS_CMD_R3					((unsigned char)0x1E)
#define CMPS_CMD_R4					((unsigned char)0x1F)
#define CMPS_CMD_ID					((unsigned char)0x20)

#define  CMPS_MASK_TAKE_MEAS		((unsigned char)0x01)
#define  CMPS_MASK_MEAS_DONE		((unsigned char)0x01)
#define  CMPS_MASK_SW_RESET			((unsigned char)0x80)
#define  CMPS_MASK_FILL_CAP			((unsigned char)0x80)
#define  CMPS_MASK_SET_CMPS			((unsigned char)0x20)
#define  CMPS_MASK_RESET_CMPS		((unsigned char)0x40)
#define  CMPS_MASK_SET_RES			((unsigned char)0x00)

#define MGAUSS_CONVERSION			((double)0.48828125)

//#define CMPS_ADDR					0b0110000
#define CMPS_ADDR					((unsigned char)0x30)
#define CMPS_ADDR_BIT_COUNT			I2C_M_SEVEN		

		

// compass Methods
extern void compass_init(void);
extern void read_compass(char *id);
extern void compass_read_id(char *id);
extern void compass_read_xyz(void);
extern void compass_set_resolution(void);
extern void compass_sw_reset(void);
extern double compass_get_bearing(void);
extern void check_compass(void);
extern uint8_t convert_bearing_to_compass_point(double bearing);

#endif /* I2C_COMPASS_H_ */