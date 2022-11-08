# HomeSweetHome
 No place like home

This code takes a LAT and LONG base point and calculates the distance and direction to that location based on the current location of the microcontroller. It also incorporates a clock and external UART. The external UART requires passwords.

A hidden_template.h file is included to show what to put in the hidden.h file to set the LAT and LONG base as well as the password inputs and outputs.

Modules used for this:
* Microchip SamD21 microcontroller using the xplained-pro dev board
* Digilent Pmod CMPS2 -  Memsic's MMC34160PJ
* MDOB128128CV-WS OLED module
* GNSS 4 click Mikroe-2045 breakout for SAM-M8Q patch antenna module from u-blox
