/*
 * UART.c
 *
 * Created: 31/08/2022 20:36:11
 *  Author: higgy
 */ 

#include "UART.h"

// GNSS Receiving & Complete Flags
volatile uint8_t GNSS_receiving = 0;
volatile uint8_t GNSS_complete = 0;
volatile uint8_t GNSS_gll_received = 0;

// Bytes Received Counters
volatile uint8_t GNSS_received_bytes_counter = 0;
volatile uint8_t GNSS_total_bytes = 0;


// Receive and Transmit Buffers
volatile uint8_t GNSS_rx_buffer[GNSS_BUFFER_SIZE] = { 0x00 };
volatile uint8_t GNSS_tx_buffer[GNSS_BUFFER_SIZE];

void GNSS_rx_cb(const struct usart_async_descriptor *const io_descr);
void GNSS_tx_cb(const struct usart_async_descriptor *const io_descr);

void debug_io_rx_cb(const struct usart_async_descriptor *const io_descr);
void debug_io_tx_cb(const struct usart_async_descriptor *const io_descr);

volatile char GNGLL_buffer[GNSS_BUFFER_SIZE];

const char password_logon[] = PASSWORD_LOGON;
const char password_fail[] = PASSWORD_FAILED;
const char password1[] = PASSWORD_1;
const uint8_t password1_length = PASSWORD_1_Length;
const char password1_response[] = PASSWORD_1_SUCCESS;
const char password2[] = PASSWORD_2;
const uint8_t password2_length = PASSWORD_2_Length;
const char password2_response[] = PASSWORD_2_SUCCESS;
const char password3[] = PASSWORD_3;
const uint8_t password3_length = PASSWORD_3_Length;
const char password3_response[] = PASSWORD_3_SUCCESS;
const char password4[] = PASSWORD_4;
const uint8_t password4_length = PASSWORD_4_Length;
const char password4_response[] = PASSWORD_4_SUCCESS;
const char password5[] = PASSWORD_5;
const uint8_t password5_length = PASSWORD_5_Length;
const char password5_response[] = PASSWORD_5_SUCCESS;
const char password_character[] = PASSWORD_CHAR;

volatile bool password_fail_response = false;
volatile bool password_good = false;
volatile uint8_t password_counter = 0;
volatile bool loopback_character = false;
volatile uint8_t current_password = 0;


/*
*	Initialise  GNSS UART with the right pins and callbacks
*/
void GNSS_uart_init()
{
	usart_async_register_callback(&gnss, USART_ASYNC_TXC_CB, GNSS_tx_cb);
	usart_async_register_callback(&gnss, USART_ASYNC_RXC_CB, GNSS_rx_cb);
	
	usart_async_enable(&gnss);
}

/*
*	Initialise debug UART with the right pins and callbacks
*/
void debug_uart_init()
{
	// Initialise ASYNC Driver
	//usart_async_register_callback(&Debug_io, USART_ASYNC_TXC_CB, debug_io_tx_cb);
	usart_async_register_callback(&Debug_io, USART_ASYNC_RXC_CB, debug_io_rx_cb);
	
	usart_async_enable(&Debug_io);
}

/*
 * debug uart receive callback
 */
void debug_io_rx_cb(const struct usart_async_descriptor *const io_descr)
{
	// Counters
	char ch;
	volatile bool fail_pass = false;
	volatile bool found_pass = false;
	volatile bool reset = false;
	
	// Read a Character
	io_read(io_descr, &ch, 1);
	
	// set flag to write out a display character to show UART character has bee received
	loopback_character = true;
	
	// check for first password character to work out which password is being attempted
	if (password_counter == 0)
	{
		if (ch == password1[0])
		{
			current_password = 1;
		}
		else if (ch == password2[0])
		{
			current_password = 2;
		}
		else if (ch == password3[0])
		{
			current_password = 3;
		}
		else if (ch == password4[0])
		{
			current_password = 4;
		}
		else if (ch == password5[0])
		{
			current_password = 5;
		}
		else 
		{
			current_password = 0;
		}
	}
	
	// there is a better way to do below a TODO for the future
	
	// if a password is being attempted then check against that password
	if (current_password == 1)
	{
		// check the input character against the password character at that index 
		if (ch == password1[password_counter])
		{
			// increment to next password character to check
			password_counter +=1;
		}
		else
		{
			// if wrong character then flag it
			fail_pass = true;
		}
		
		// check all characters of the password have been checked
		if (password_counter > (password1_length - 1))
		{
			// flag to say they have
			found_pass = true;
		}
	}
	else if (current_password == 2)
	{
		if (ch == password2[password_counter])
		{
			password_counter +=1;
		}
		else
		{
			fail_pass = true;
		}
		
		if (password_counter > (password2_length - 1))
		{
			found_pass = true;
		}
	}
	else if (current_password == 3)
	{
		if (ch == password3[password_counter])
		{
			password_counter +=1;
		}
		else
		{
			fail_pass = true;
		}
		
		if (password_counter > (password3_length - 1))
		{
			found_pass = true;
		}
	}
	else if (current_password == 4)
	{
		if (ch == password4[password_counter])
		{
			password_counter +=1;
		}
		else
		{
			fail_pass = true;
		}
		
		if (password_counter > (password4_length - 1))
		{
			found_pass = true;
		}
	}
	else if (current_password == 5)
	{
		if (ch == password5[password_counter])
		{
			password_counter +=1;
		}
		else
		{
			fail_pass = true;
		}
		
		if (password_counter > (password5_length - 1))
		{
			found_pass = true;
		}
	}
	else
	{
		// if character not part of any password then say so
		password_counter = 0;
		current_password = 0;
		fail_pass = true;
	}
	
	// reset and display failed password response
	if (fail_pass == true)
	{
		password_good = false;
		password_fail_response = true;
		password_counter = 0;
		current_password = 0;
	}
	
	// password found display success
	if (found_pass == true)
	{
		password_good = true;
		password_counter = 0;
	}	
}


/*
 * debug uart transmit callback
 */
//void debug_io_tx_cb(const struct usart_async_descriptor *const io_descr)
//{
	///* Do Nothing */
//}

/*
 * GNSS uart receive callback
 */
void GNSS_rx_cb(const struct usart_async_descriptor *const io_descr)
{
	// Counters
	uint8_t ch, count;
	
	// Read a Character
	count = io_read(io_descr, &ch, 1);
	
	// Check if we're receiving
	if (GNSS_receiving == 0)
	{
		// Check for New Line or Carriage Return
		if (ch != '\r' && ch != '\n')
		{
			// Set Receiving Flag
			GNSS_receiving = 1;
			
			// Reset Byte Counter
			GNSS_received_bytes_counter = 0;
			
			// Start Filling the Buffer
			GNSS_rx_buffer[GNSS_received_bytes_counter] = ch;
			
			// Increment the Byte Counter
			GNSS_received_bytes_counter += count;
		}
	}
	else
	{
		// Continue Filling Buffer
		GNSS_rx_buffer[GNSS_received_bytes_counter] = ch;
		
		// Increment the Byte Counter
		GNSS_received_bytes_counter += count;
		
		// Check for New Line or Carriage Return
		if (ch == '\r' || ch == '\n')
		{
			// add newline characters
			GNSS_received_bytes_counter += 1;
			GNSS_rx_buffer[GNSS_received_bytes_counter] = '\r';
			
			GNSS_received_bytes_counter += 1;
			GNSS_rx_buffer[GNSS_received_bytes_counter] = '\n';
			
			// Set the Completion Flag
			GNSS_complete = 1;
			
			
			// Total Bytes
			GNSS_total_bytes = GNSS_received_bytes_counter;
		}
		
		// Check for Buffer Overflow
		if (GNSS_received_bytes_counter >= GNSS_BUFFER_SIZE)
		{
			// Reset Buffer Counter
			GNSS_received_bytes_counter = 0;
		}
	}
}

/**
 * GNSS uart transmit callback
 */
void GNSS_tx_cb(const struct usart_async_descriptor *const io_descr)
{
	// do nothing	
}

/* 
* string to token - return string of characters up to the delimiter.
* If nothing in the string then the delimiter character is returned
*
*Updated strtok to get something back when there is nothing between consecutive delimiters
** Based on https://stackoverflow.com/questions/26597977/split-string-with-multiple-delimiters-using-strtok-in-c 
*/
char *zstring_strtok(char *str, const char *delim) 
{
    static char *static_str=0;      /* var to store last address */
    int index=0, strlength=0;       /* integers for indexes */
    int found = 0;                  /* check if delim is found */

    /* delimiter cannot be NULL
    * if no more char left, return NULL as well
    */
    if (delim==0 || (str == 0 && static_str == 0))
        return 0;

    if (str == 0)
        str = static_str;

    /* get length of string */
    while(str[strlength])
        strlength++;

    /* find the first occurrence of delim */
    for (index=0;index<strlength;index++)
        if (str[index]==delim[0]) {
            found=1;
            break;
        }

    /* if delim is not contained in str, return str */
    if (!found) {
        static_str = 0;
        return str;
    }

    /* check for consecutive delimiters
    *if first char is delim, return delim
    */
    if (str[0]==delim[0]) {
        static_str = (str + 1);
        return (char *)delim;
    }

    /* terminate the string
    * this assignment requires char[], so str has to
    * be char[] rather than *char
    */
    str[index] = '\0';

    /* save the rest of the string */
    if ((str + index + 1)!=0)
	{
        static_str = (str + index + 1);
	}
    else
	{
        static_str = 0;
	}

    return str;
}

// Check what has come in from the GNSS module and if it is the right header ($GNGLL - with time and coordinates) populate the GNGLL data struct to be made use of
void check_GNSS()
{
	// Check if receiving
	if (GNSS_receiving == 1)
	{
		// Check if sending is complete
		if (GNSS_complete == 1)
		{
			// Reset Flags
			GNSS_receiving = 0;
			GNSS_complete = 0;

			// Copy Message to TX Buffer
			memcpy(&GNSS_tx_buffer[0], &GNSS_rx_buffer[0], GNSS_BUFFER_SIZE); // this is to output what has come in from gnss if necessary
			memcpy(&GNGLL_buffer[0], &GNSS_rx_buffer[0], GNSS_BUFFER_SIZE);
			
			if (strncmp (GNGLL_buffer, gngll_header_data, GNGLL_HEADER_SIZE - 1) == 0)  // wait for the right one. -1 as no null termination character in buffer wanted in compare
			{
				GNSS_gll_received = 1;

				populate_GNGLL_data(GNGLL_buffer);	
				
				//Populate_GNGLL_data(test_gll);	
			}
			
			// Clear Memory
			memset(&GNSS_rx_buffer, 0x00, GNSS_BUFFER_SIZE);		
		}
	}
}

void check_password()
{
	// write out a character to show something has been input
	if (loopback_character == true)
	{
		io_write(&Debug_io.io, &password_character, 1);
		loopback_character = false;
	}
	
	// if password is good display the password response 
	if (password_good)
	{
		switch (current_password)
		{
			case 1:
				io_write(&Debug_io.io, password1_response, strlen(password1_response));
			break;
			case 2:
				io_write(&Debug_io.io, password2_response, strlen(password2_response));
			break;
			case 3:
				io_write(&Debug_io.io, password3_response, strlen(password3_response));
			break;
			case 4:
				io_write(&Debug_io.io, password4_response, strlen(password4_response));
			break;
			case 5:
				io_write(&Debug_io.io, password5_response, strlen(password5_response));
			break;
			default:
			/* Do nothing */
			break;
		}
		password_good = false;
		current_password = 0;
	}
	
	// display password failed
	if (password_fail_response)
	{
		io_write(&Debug_io.io, password_fail, strlen(password_fail));
		password_fail_response = false;
	}
}

void debug_uart_password_display()
{
	io_write(&Debug_io.io, password_logon, strlen(password_logon));
}