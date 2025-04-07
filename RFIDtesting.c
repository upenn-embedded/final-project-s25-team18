/*
 * File:   newmain.c
 * Author: jattse
 *
 * Created on April 4, 2025, 3:14 PM
 */

#define F_CPU 16000000UL  // or whatever clock speed your ATmega328PB is running at
#include <util/delay.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>     // For sprintf, etc.
#include "uart.h"


#include "mfrc522.h"   // Our minimal MFRC522 driver

// --------------------
// SPI PIN DEFINITIONS
// Adjust to match your wiring
// --------------------
#define PIN_SS   PB2
#define PIN_MOSI PB3
#define PIN_MISO PB4
#define PIN_SCK  PB5

// We'll assume the MFRC522 reset pin is on PD7 (optional).
#define RFID_RST_DDR   DDRD
#define RFID_RST_PORT  PORTD
#define RFID_RST_PIN   PD7





// --------------------
// SPI Functions
// --------------------
void RFID_SS_Select(void)
{
    PORTB &= ~(1 << PIN_SS); // SS low
}

void RFID_SS_Unselect(void)
{
    PORTB |= (1 << PIN_SS);  // SS high
}

uint8_t SPI_transfer(uint8_t data)
{
    SPDR0 = data;
    while(!(SPSR0 & (1 << SPIF)));
    return SPDR0; // Return received data
}

// Initialize hardware SPI at F_CPU/16
void SPI_init(void)
{
    // MOSI, SCK, SS as outputs
    // PB0 output pin to light up LED
  
    DDRB |= (1 << PIN_MOSI) | (1 << PIN_SCK) | (1 << PIN_SS);
    DDRB &= ~(1 << PIN_MISO); // MISO input

    // Pull SS high
    PORTB |= (1 << PIN_SS);
    

    // Enable SPI, Master, clk/16
    SPCR0 = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

// ----------------------------------------------------
// MAIN
// ----------------------------------------------------
int main(void)
{
    // If running at 16 MHz, 9600 baud => 103
    // If running at 8 MHz, 9600 baud => 51
   
    SPI_init();
    uart_init();
    // (Optional) If RST pin is used, set as output + high
    RFID_RST_DDR |= (1 << RFID_RST_PIN);
    RFID_RST_PORT |= (1 << RFID_RST_PIN);

    // Initialize the MFRC522
    MFRC522_Init();

    uint8_t ver = MFRC522_ReadReg(MFRC522_VersionReg);

    // Example authorized UID
    uint8_t authorizedUid[4] = {0x83, 0x7A, 0xA9, 0xD9};
    DDRD |= (1 << PD5);
   

    while(1)
    {
        // Check for new card
        
       PORTD |= (1 << PD5);
        if(MFRC522_CheckNewCard()) {
            // If found, read UID
            uint8_t uid[10];
            uint8_t uidSize = 0;
            
            
            printf("read card \n");
            if(MFRC522_ReadCardUID(uid, &uidSize)) {
                PORTD &= ~(1 << PD5);
                // Print what we got
                printf(" Card UID:");
                for (uint8_t i = 0; i < uidSize; i++) {
                    printf("%02X", uid[i]); // Print in hex, zero-padded
                    if (i < uidSize - 1) {
                        printf(":"); // Optional separator
                    }
                }
                printf("\n");
           

//                // Compare to our known UID
//                if((uidSize == 4) && 
//                    (uid[0] == authorizedUid[0]) &&
//                    (uid[1] == authorizedUid[1]) &&
//                    (uid[2] == authorizedUid[2]) &&
//                    (uid[3] == authorizedUid[3])) 
//                {
//                    //PORTB |= (1 << PINB0); // Turn LED ON
//                } else {
//                   //PORTB &= ~(1 << PINB0); // Turn LED OFF
//                }
            }
        }      
       _delay_ms(2000);
       
    }
    return 0;
}
