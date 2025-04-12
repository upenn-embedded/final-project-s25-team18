#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart2.h"
#include "ST7735.h"
#include "LCD_GFX.h"
#include <stdbool.h>


#define ENTRY_SENSOR PB0  // INT0
#define EXIT_SENSOR  PC0  // INT1
#define BUTTON PD0

volatile uint16_t start_time = 0;
volatile uint16_t end_time = 0;
int i = 0;
char buffer[16]; // Make sure this buffer is large enough
int snackIndex = 0;
int userBalance = 0;
bool dispensed = false;



void Initialize(){
    lcd_init();
    DDRD &= ~(1 << BUTTON);
    
    
}



// Step 1: Show the beginning display until keypad is pressed
// Step 2: Display snack info for 10000ms 
// Step 3: While loop till user 
// Step 4:


int main(void) {
    uart_init();
    Initialize();

    while (1) {
        // === Step 1: Initial State ===
        resetDisplayAfterDispense();  // Show welcome screen or reset LCD
        snackIndex = 0;
        userBalance = 0;
        dispensed = false;

        // === Step 2: Simulate user selection ===
        _delay_ms(5000);  // Replace with actual button or keypad input later
        
        while (!(PIND & (1 << BUTTON))) {
            
            _delay_ms(1000); // Simulate waiting for coin
            
        }
        snackIndex = 1;
        LCD_setScreen(0xFFFF); // or just clear the area
        LCD_drawString(30, 50, "Selected Snack", 0x0000, 0xFFFF);
        _delay_ms(5000);
        // TODO: Check the keypad 

        // === Step 3: Show snack info ===
        displaySnackInfo(snackIndex, userBalance);
        _delay_ms(10000);

        // === Step 4: Coin insertion loop ===
        while (!(PIND & (1 << BUTTON))) {
            _delay_ms(3000); // Simulate waiting for coin
            LCD_setScreen(0xFFFF); // or just clear the area
            LCD_drawString(30, 50, "Insufficient balance.", 0x0000, 0xFFFF);
        }

        // Coin inserted, update balance
        userBalance = getPrice(snackIndex);
        updateStock(snackIndex);
        displaySnackInfo(snackIndex, userBalance);

        _delay_ms(6000);  // Optional delay

        LCD_setScreen(0xFFFF);
        LCD_drawString(30, 50, "Sufficient balance.", 0x0000, 0xFFFF);
        _delay_ms(5000);

        // === Step 5: Dispense snack ===
        while (!dispensed) {
            dispenseSnack(snackIndex);
            _delay_ms(10000);
            dispensed = true; // Replace with IR sensor check in final
            
        }
        LCD_setScreen(0xFFFF);
        LCD_drawString(20, 50, "Item Dispensed. Enjoy!", 0x0000, 0xFFFF);

        _delay_ms(10000);  // Wait before looping again
    }
}

        
        
        
        
     
