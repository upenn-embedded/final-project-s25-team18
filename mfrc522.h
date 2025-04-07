#ifndef MFRC522_H
#define MFRC522_H

#include <avr/io.h>

// -----------------------------------------------------
// User must provide these SPI helpers in their code
// (e.g. from your main .c). 
// They should match your hardware pin connections.
// -----------------------------------------------------
uint8_t SPI_transfer(uint8_t data);
void    RFID_SS_Select(void);
void    RFID_SS_Unselect(void);

// -----------------------------------------------------
// MFRC522 Register Addresses
// (Subset of the full list)
// -----------------------------------------------------
#define MFRC522_CommandReg       0x01
#define MFRC522_ComIEnReg        0x02
#define MFRC522_ComIrqReg        0x04
#define MFRC522_ErrorReg         0x06
#define MFRC522_Status2Reg       0x08
#define MFRC522_FIFODataReg      0x09
#define MFRC522_FIFOLevelReg     0x0A
#define MFRC522_ControlReg       0x0C
#define MFRC522_BitFramingReg    0x0D
#define MFRC522_CollReg          0x0E
#define MFRC522_ModeReg          0x11
#define MFRC522_TxModeReg        0x12
#define MFRC522_RxModeReg        0x13
#define MFRC522_TxControlReg     0x14
#define MFRC522_TxASKReg         0x15
#define MFRC522_VersionReg       0x37
#define MFRC522_CommandIdle      0x00
#define MFRC522_CommandMem       0x01
#define MFRC522_CommandCalcCRC   0x03
#define MFRC522_CommandTransceive 0x0C
#define MFRC522_CommandSoftReset 0x0F

// Some commands used for ISO/IEC 14443A (Type A) PICCs
#define PICC_CMD_REQA            0x26
#define PICC_CMD_ANTICOLL        0x93

// -----------------------------------------------------
// Function prototypes
// -----------------------------------------------------
void    MFRC522_Init(void);
uint8_t MFRC522_CheckNewCard(void);
uint8_t MFRC522_ReadCardUID(uint8_t *uidBytes, uint8_t *uidSize);

// -----------------------------------------------------
// Low-level register access
// -----------------------------------------------------
void    MFRC522_WriteReg(uint8_t reg, uint8_t value);
uint8_t MFRC522_ReadReg(uint8_t reg);

// For FIFO operations:
void    MFRC522_WriteBuffer(uint8_t reg, uint8_t *buffer, uint8_t len);
void    MFRC522_ReadBuffer(uint8_t reg, uint8_t *buffer, uint8_t len);

#endif