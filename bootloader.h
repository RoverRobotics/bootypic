#ifndef _BOOTLOADER_H
#define _BOOTLOADER_H
#include "boot_user.h"

/** @brief the version of the transmission protocol
 */
#define VERSION_STRING "0.1"

#define TX_BUF_LEN ((MAX_PROG_SIZE * 4) + 0x10)
#define RX_BUF_LEN ((MAX_PROG_SIZE * 4) + 0x10)

/**
 * @brief the byte that indicates the start of a frame
 */
#define START_OF_FRAME 0xf7

/**
 * @brief the byte that indicates the end of a frame
 */
#define END_OF_FRAME 0x7f

/**
 * @brief the escape byte, which indicates that the following byte will be
 * XORed with the ESC_XOR byte before being transmitted
 */
#define ESC 0xf6

/**
 * @brief the value to use to escape characters
 */
#define ESC_XOR 0x20

/**
 * @brief commands available for the bootloader
 */
typedef enum {
    /* textual commands */
    CMD_READ_PLATFORM = 0x00,
    CMD_READ_VERSION = 0x01,
    CMD_READ_ROW_LEN = 0x02,
    CMD_READ_PAGE_LEN = 0x03,
    CMD_READ_PROG_LEN = 0x04,
    CMD_READ_MAX_PROG_SIZE = 0x05,
    CMD_READ_APP_START_ADDR = 0x06,
    CMD_READ_BOOT_START_ADDR = 0x07,

    /* erase operations */
    CMD_ERASE_PAGE = 0x10,

    /* flash read memory operations */
    CMD_READ_ADDR = 0x20,
    CMD_READ_MAX = 0x21,

    /* flash write operations */
    CMD_WRITE_ROW = 0x30,
    CMD_WRITE_MAX_PROG_SIZE = 0x31,

    /* application */
    CMD_START_APP = 0x40
} CommCommand;

/**
 * @brief initializes the pins
 */
void initPins(void);

/**
 * @brief receives data from the UART
 */
void receiveBytes(void);

/**
 * @brief processes the rx buffer into commands with parameters from packets
 */
void processReceived(void);

/**
 * @brief processes commands
 * @param data byte buffer of data that has been stripped of transmission
 * protocols
 */
void processCommand(uint8_t *data);

/**
 * @brief send the start byte and initialize fletcher checksum accumulator
 */
void txStart(void);

/**
 * @brief receive a single UART byte, assigning it to outbyte
 * @return true if successful
 */
extern bool tryRxByte(uint8_t *outbyte);

/**
 * @brief transmits a single byte, escaping if necessary, along with
 * accumulating the fletcher checksum
 * @param byte a byte of data to transmit
 */
void txByte(uint8_t byte);

/**
 * @brief convenience function for transmitting an array of bytes with the
 * associated command
 * @param cmd the type of message
 * @param bytes an array of bytes to transmit
 * @param len the number of bytes to transmit from the array
 */
void txBytes(uint8_t cmd, uint8_t *bytes, uint16_t len);

/**
 * @brief convenience function for transmitting an array of bytes with the
 * associated command
 * @param cmd the type of message
 * @param bytes an array of bytes to transmit
 * @param len the number of bytes to transmit from the array
 */
void txArray8bit(uint8_t cmd, uint8_t *bytes, uint16_t len);

/**
 * @brief convenience function for transmitting an array of 16-bit words
 * with the associated command
 * @param cmd the type of message
 * @param bytes an array of 16-bit words to transmit
 * @param len the number of bytes to transmit from the array
 */
void txArray16bit(uint8_t cmd, uint16_t *words, uint16_t len);

/**
 * @brief convenience function for transmitting an array of 32-bit words
 * with the associated command
 * @param cmd the type of message
 * @param bytes an array of 32-bit words to transmit
 * @param len the number of bytes to transmit from the array
 */
void txArray32bit(uint8_t cmd, uint32_t *words, uint16_t len);

/**
 * @brief convenience function for transmitting a string
 * @param cmd the type of message
 * @param str a pointer to a string buffer to transmit
 */
void txString(uint8_t cmd, char *str);

/**
 * @brief appends the checksum, properly escaping the sequence where necessary,
 * and sends the end byte
 */
void txEnd(void);

/**
 * @brief accumulates a single byte into the running accumulator
 * @param byte the byte to accumulate
 * @return the current fletcher16 value
 */
uint16_t fletcher16Accum(uint8_t byte);

/**
 * @brief calculate the fletcher16 value of an array given the array pointer
 * and length
 * @param data an 8-bit array pointer
 * @param length the length of the array
 * @return the fletcher16 value
 */
uint16_t fletcher16(uint8_t *data, uint16_t length);

/**
 * @brief starts the application
 * @param applicationAddress address of the instruction to jump to.
 * Note the address must be in the first tblpage of program memory (first 32K instructions)
 */
void startApp(uint16_t applicationAddress);

/**
 * @brief initializes the oscillator
 */
extern void initOsc(void);

/**
 * @brief initializes the pins
 */
extern void initPins(void);

/**
 * @brief initializes the UART
 */
extern void initUart(void);

/**
 * @brief initializes timers for bootloader timeout
 */
extern void initTimers(void);

#endif