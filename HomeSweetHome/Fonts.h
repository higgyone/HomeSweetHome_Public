/*
 * Fonts.h
 *
 * Created: 23/09/2022 22:09:48
 *  Author: higgy
 */ 


#ifndef FONTS_H_
#define FONTS_H_

#include <stdint.h>

#define LETTER_WIDTH				((unsigned char)0x10)
#define LETTER_HEIGHT				((unsigned char)0x14)
#define COLON_WIDTH					((unsigned char)0x08)
#define DECIMAL_POINT_WIDTH			((unsigned char)0x08)
#define CHAR_SPACE					((unsigned char)0x01)
#define ARROW_WIDTH					((unsigned char)0x14)
#define ARROW_HEIGHT				((unsigned char)0x14)

#define CLEAR_LINE_ARRAY_SIZE		((unsigned char)0x80)
#define CHAR_ARRAY_SIZE				((unsigned char)0xA0) // 160
#define CHAR_HALF_ARRAY_SIZE		((unsigned char)0x50) // 80
#define ARROW_ARRAY_SIZE			((unsigned char)0xC8) // 200

extern const uint8_t clearline[CLEAR_LINE_ARRAY_SIZE];
extern const uint8_t zero_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t one_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t two_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t three_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t four_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t five_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t six_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t seven_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t eight_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t nine_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t colon_consolas[CHAR_HALF_ARRAY_SIZE];
extern const uint8_t N_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t o_lower_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t G_upper_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t P_upper_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t S_upper_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t dash_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t decimal_point_consolas[CHAR_HALF_ARRAY_SIZE];
extern const uint8_t M_upper_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t Blank_Letter[CHAR_ARRAY_SIZE];
extern const uint8_t k_lower_consolas [CHAR_ARRAY_SIZE];
extern const uint8_t m_lower_consolas [CHAR_ARRAY_SIZE];
extern const uint8_t i_lower_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t H_upper_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t e_lower_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t w_lower_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t t_lower_consolas[CHAR_ARRAY_SIZE];
extern const uint8_t North_Arrow[ARROW_ARRAY_SIZE];
extern const uint8_t South_Arrow[ARROW_ARRAY_SIZE];
extern const uint8_t East_Arrow[ARROW_ARRAY_SIZE];
extern const uint8_t West_Arrow[ARROW_ARRAY_SIZE];
extern const uint8_t North_East_Arrow[ARROW_ARRAY_SIZE];
extern const uint8_t South_East_Arrow[ARROW_ARRAY_SIZE];
extern const uint8_t South_West_Arrow[ARROW_ARRAY_SIZE];
extern const uint8_t North_West_Arrow[ARROW_ARRAY_SIZE];
extern const uint8_t Blank_Arrow[ARROW_ARRAY_SIZE];

#endif /* FONTS_H_ */