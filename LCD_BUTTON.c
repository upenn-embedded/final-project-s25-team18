#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart2.h"
#include "ST7735.h"
#include "LCD_GFX.h"
#include <stdbool.h>



#define ENTRY_SENSOR PB0  // INT0
#define EXIT_SENSOR  PC0  // INT1
#define BUTTON PD2

volatile uint16_t start_time = 0;
volatile uint16_t end_time = 0;
int i = 0;
char buffer[16]; // Make sure this buffer is large enough
int snackIndex = 0;
int userBalance = 0;
bool dispensed = false;
char key;



void Initialize(){
    lcd_init();
    DDRD &= ~(1 << BUTTON);
    
    
}

bool isButtonPressed() {
    if (!(PIND & (1 << BUTTON))) {
        _delay_ms(20); // Wait for bouncing to settle
        if (!(PIND & (1 << BUTTON))) {
            while (!(PIND & (1 << BUTTON))) {
                // Wait for button to be released
            }
            _delay_ms(20); // Optional: debounce on release too
            return true;
        }
    }
    return false;
}

void uart_rx_init(void)
{
    // 1. Set baud rate
    uint16_t ubrr = 103; // for 9600 baud @ 16MHz
    UBRR0H = (unsigned char)(BAUD_PRESCALER >> 8);
    UBRR0L = (unsigned char) BAUD_PRESCALER;

    // 2. Set frame format: 8 data, no parity, 1 stop bit
    // UCSZ01 and UCSZ00 = 1 => 8-bit data
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit, no parity, 1 stop

    // 3. Enable transmitter
    UCSR0B = (1 << RXEN0);
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
        key = 0;
        dispensed = false;
        _delay_ms(2000);
        LCD_setScreen(0xFFFF);
        LCD_drawString(30, 50, "Please select snack", 0x0000, 0xFFFF);
        LCD_drawString(30, 70, "By pressing keypad", 0x0000, 0xFFFF);
        _delay_ms(2500);
        // === Step 2: Simulate user selection ===
          // Replace with actual button or keypad input later
        LCD_setScreen(0xFFFF);
        LCD_drawString(30, 50, "Waiting for keypad", 0x0000, 0xFFFF);
        LCD_drawString(30, 70, "input via UART...", 0x0000, 0xFFFF);
        
        // Wait for valid key from UART
        while (1) {
            key = uart_receive_char();  // Wait for UART input

            if (key >= '1' && key <= '9') {
                snackIndex = key - '0';  // Convert ASCII to index
                break;
            } else {
                LCD_setScreen(0xFFFF);
                LCD_drawString(30, 90, "Invalid key!", 0xF800, 0xFFFF);  // Optional error
                _delay_ms(1000);
                LCD_setScreen(0xFFFF);  // Clear error area
            }
        }
        
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

        
        
        
        
        
        
        
        
     
