#include "mfrc522.h"
#include <util/delay.h>

// -------------------------------------------------------------
// Write to one MFRC522 register
// -------------------------------------------------------------
void MFRC522_WriteReg(uint8_t reg, uint8_t value)
{
    // The MSB of the first byte we send is 0 for write.
    // Next 6 bits are the register address, LSBs are not used.
    RFID_SS_Select();
    SPI_transfer((reg << 1) & 0x7E); 
    SPI_transfer(value);
    RFID_SS_Unselect();
}

// -------------------------------------------------------------
// Read from one MFRC522 register
// -------------------------------------------------------------
uint8_t MFRC522_ReadReg(uint8_t reg)
{
    uint8_t value;
    // The MSB of the first byte is 1 for read.
    RFID_SS_Select();
    SPI_transfer(((reg << 1) & 0x7E) | 0x80);
    value = SPI_transfer(0x00);
    RFID_SS_Unselect();
    return value;
}

// -------------------------------------------------------------
// Write multiple bytes to FIFO
// -------------------------------------------------------------
void MFRC522_WriteBuffer(uint8_t reg, uint8_t *buffer, uint8_t len)
{
    RFID_SS_Select();
    SPI_transfer(((reg << 1) & 0x7E)); // write mode
    for(uint8_t i=0; i<len; i++) {
        SPI_transfer(buffer[i]);
    }
    RFID_SS_Unselect();
}

// -------------------------------------------------------------
// Read multiple bytes from FIFO
// -------------------------------------------------------------
void MFRC522_ReadBuffer(uint8_t reg, uint8_t *buffer, uint8_t len)
{
    RFID_SS_Select();
    SPI_transfer(((reg << 1) & 0x7E) | 0x80); // read mode
    for(uint8_t i=0; i<len; i++) {
        buffer[i] = SPI_transfer(0x00);
    }
    RFID_SS_Unselect();
}

// -------------------------------------------------------------
// Perform a Soft Reset & Basic Setup
// -------------------------------------------------------------
void MFRC522_Init(void)
{
    // Software reset
    MFRC522_WriteReg(MFRC522_CommandReg, MFRC522_CommandSoftReset);
    _delay_ms(50);
    // Wait for PowerDown bit to be cleared
    while (MFRC522_ReadReg(MFRC522_CommandReg) & (1 << 4)) {
        // PwrDown is bit4, wait to clear
    }
    // Recommended setup from datasheet / typical usage:
    // Enable the antenna driver pins TX1 & TX2
    uint8_t val = MFRC522_ReadReg(MFRC522_TxControlReg);
    if(!(val & 0x03)) {
        MFRC522_WriteReg(MFRC522_TxControlReg, val | 0x03);
    }
    // Force 100% ASK
    MFRC522_WriteReg(MFRC522_TxASKReg, 0x40);
    // Set default mode registers
    MFRC522_WriteReg(MFRC522_ModeReg, 0x3D);   // CRC initial value 0x6363
    MFRC522_WriteReg(MFRC522_RxModeReg, 0x00);
    MFRC522_WriteReg(MFRC522_TxModeReg, 0x00);

    // Optional: Adjust receiver gain, etc. 
    // For now, we’ll trust defaults.

    // Clear any IRQ flags
    MFRC522_WriteReg(MFRC522_ComIrqReg, 0x7F);
}

// -------------------------------------------------------------
// Check if a new card has placed an ATQA response (REQA).
// This is a simplified version of PICC_IsNewCardPresent.
// Returns 1 if a card is found, 0 if not.
// -------------------------------------------------------------
uint8_t MFRC522_CheckNewCard(void)
{
    // Send REQA command, expect 2 bytes back if a card is in range.
    // See ISO/IEC 14443A specs. 
    uint8_t buffer[2];
    uint8_t validBits;
    uint8_t status;

    // Prepare the MFRC522
    MFRC522_WriteReg(MFRC522_CommandReg, MFRC522_CommandIdle);
    MFRC522_WriteReg(MFRC522_ComIrqReg, 0x7F);     // Clear IRQs
    MFRC522_WriteReg(MFRC522_FIFOLevelReg, 0x80); // Flush FIFO

    // Send REQA
    buffer[0] = PICC_CMD_REQA;
    MFRC522_WriteBuffer(MFRC522_FIFODataReg, buffer, 1);
    MFRC522_WriteReg(MFRC522_BitFramingReg, 7); // send only 7 bits
    MFRC522_WriteReg(MFRC522_CommandReg, MFRC522_CommandTransceive);

    // Start the transceive
    MFRC522_WriteReg(MFRC522_ComIrqReg, 0x7F); // Clear interrupts
    // set the StartSend bit in BitFramingReg
    MFRC522_WriteReg(MFRC522_BitFramingReg, 0x87);

    // Wait for the command to complete
    // or until an error or timeout
    uint8_t irq;
    uint16_t counter = 0xFFFF;
    do {
        irq = MFRC522_ReadReg(MFRC522_ComIrqReg);
        counter--;
    } while ((counter != 0) && !(irq & 0x30)); // RxIRq or TimerIRq

    if (counter == 0) {
        // timed out
        return 0;
    }

    // Check for errors
    uint8_t errorRegValue = MFRC522_ReadReg(MFRC522_ErrorReg);
    if (errorRegValue & 0x13) {
        // Some error
        return 0;
    }

    // If we got something in FIFO, then a card might be present
    uint8_t n = MFRC522_ReadReg(MFRC522_FIFOLevelReg);
    if (n == 0) {
        // No ATQA answer
        return 0;
    }

    // We read out the ATQA response (2 bytes typically)
    MFRC522_ReadBuffer(MFRC522_FIFODataReg, buffer, n);
    // We won't do anything with them here. If n>0, there's a card.
    return 1;
}

// -------------------------------------------------------------
// Read the card’s UID using anticollision (ISO14443 Type A).
// Returns 1 on success, 0 on error.  Fills uidBytes[] & uidSize.
// -------------------------------------------------------------
uint8_t MFRC522_ReadCardUID(uint8_t *uidBytes, uint8_t *uidSize)
{
    // This is a minimal version of "PICC_ReadCardSerial()"
    // using the 0x93 AntiColl cascade level 1 for a single UID.
    // For multi-cascade or more advanced cards, the full library is needed.

    // Prepare
    MFRC522_WriteReg(MFRC522_CommandReg, MFRC522_CommandIdle);
    MFRC522_WriteReg(MFRC522_ComIrqReg, 0x7F);
    MFRC522_WriteReg(MFRC522_FIFOLevelReg, 0x80);

    // Send [anticollision command + NVB=2]
    uint8_t cmdBuffer[2];
    cmdBuffer[0] = PICC_CMD_ANTICOLL;
    cmdBuffer[1] = 0x20; // NVB
    MFRC522_WriteBuffer(MFRC522_FIFODataReg, cmdBuffer, 2);
    MFRC522_WriteReg(MFRC522_CommandReg, MFRC522_CommandTransceive);
    MFRC522_WriteReg(MFRC522_BitFramingReg, 0x80); // StartSend

    // Wait for IRQ
    uint8_t irq;
    uint16_t counter = 0xFFFF;
    do {
        irq = MFRC522_ReadReg(MFRC522_ComIrqReg);
        counter--;
    } while ((counter != 0) && !(irq & 0x30)); // RxIRq or TimerIRq

    if (counter == 0) {
        // Timeout
        return 0;
    }

    // Check errors
    uint8_t errorVal = MFRC522_ReadReg(MFRC522_ErrorReg);
    if (errorVal & 0x13) {
        // Error
        return 0;
    }

    // Read the collision register to see if something is up
    uint8_t coll = MFRC522_ReadReg(MFRC522_CollReg);
    if (coll & 0x20) {
        // CollErr
        return 0;
    }

    // Bytes in FIFO?
    uint8_t n = MFRC522_ReadReg(MFRC522_FIFOLevelReg);
    if (n < 5) {
        // We expect 5 bytes: UID[4] + BCC
        return 0;
    }

    uint8_t data[10];
    MFRC522_ReadBuffer(MFRC522_FIFODataReg, data, n);

    // The first 4 bytes should be the UID
    for (uint8_t i=0; i<4; i++) {
        uidBytes[i] = data[i];
    }
    *uidSize = 4;
    return 1;
}