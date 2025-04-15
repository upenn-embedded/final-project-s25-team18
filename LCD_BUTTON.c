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
#define BUTTON PC5

volatile uint16_t start_time = 0;
volatile uint16_t end_time = 0;
int i = 0;
char buffer[16]; // Make sure this buffer is large enough
int snackIndex = 0;
int userBalance = 0;
bool dispensed = false;



void Initialize(){
    lcd_init();
    DDRC &= ~(1 << BUTTON);
    
    
}

bool isButtonPressed() {
    if (!(PINC & (1 << BUTTON))) {
        _delay_ms(20); // Wait for bouncing to settle
        if (!(PINC & (1 << BUTTON))) {
            while (!(PINC & (1 << BUTTON))) {
                // Wait for button to be released
            }
            _delay_ms(20); // Optional: debounce on release too
            return true;
        }
    }
    return false;
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
        _delay_ms(2000);
        LCD_setScreen(0xFFFF);
        LCD_drawString(30, 50, "Please select snack", 0x0000, 0xFFFF);
        LCD_drawString(30, 70, "By pressing keypad", 0x0000, 0xFFFF);
        _delay_ms(5000);
        // === Step 2: Simulate user selection ===
          // Replace with actual button or keypad input later
        
        while (!isButtonPressed()) {
            
            // nothing
            
        }
        snackIndex = 1;
        LCD_setScreen(0xFFFF); // or just clear the area
        LCD_drawString(40, 50, "Selected Snack", 0x0000, 0xFFFF);
        _delay_ms(5000);
        // TODO: Check the keypad 

        // === Step 3: Show snack info ===
        if (getStock(snackIndex) > 0) {
            displaySnackInfo(snackIndex, userBalance);
            _delay_ms(2000);
            LCD_setScreen(0xFFFF); // or just clear the area
            LCD_drawString(40, 50, "Insert Coins", 0x0000, 0xFFFF);
        } else {
            LCD_setScreen(0xFFFF); // or just clear the area
            LCD_drawString(40, 50, "No stock left", 0x0000, 0xFFFF);
            LCD_drawString(20, 60, "Pick another snack", 0x0000, 0xFFFF);
            _delay_ms(2000);
            continue;
        }
        

        // === Step 4: Coin insertion loop ===
        while (!isButtonPressed()) {
            
        }

        // Coin inserted, update balance
        userBalance = getPrice(snackIndex);
        displaySnackInfo(snackIndex, userBalance);
        updateStock(snackIndex);

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
        LCD_drawString(15, 50, "Item Dispensed. Enjoy!", 0x0000, 0xFFFF);
        LCD_drawString(80, 90, ":)", 0x0000, 0xFFFF);

        _delay_ms(10000);  // Wait before looping again
    }
}

        
        
        
        
        
        
        
        
     
