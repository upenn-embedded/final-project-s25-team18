#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
// Number of rows & columns
#define NUM_ROWS 1
#define NUM_COLS 3

// TRIG1, ECHO1
#define TRIG1_PIN PC4
#define ECHO1_PIN PC5

// TRIG2, ECHO2
#define TRIG2_PIN PD3
#define ECHO2_PIN PD4

// Speed of sound = 0.0331 cm/us
#define SOUND_SPEED_CM_PER_US 0.0331

// Ultrasonic trigger pulse and timeout
#define TRIGGER_PULSE_US 10
#define MAX_ECHO_WAIT_US 25000  // Timeout threshold (~8 ft)

// Distance threshold for detection
#define DETECTION_THRESHOLD_CM 5.0



#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart2.h"
#include "ST7735.h"
#include "LCD_GFX.h"
#include <stdbool.h>



#define BUTTON PD2



volatile uint16_t start_time = 0;
volatile uint16_t end_time = 0;
int i = 0;
char buffer[16]; // Make sure this buffer is large enough
int snackIndex = 0;
int userBalance = 0;
int numberQuarters = 0;
float distance1 = 50;
float distance2 = 50;
bool dispensed = false;
bool coinDetection = false;
bool waitingForMotorRequest = true;
bool motorPicked = false;
bool quartersRecieved = false;
bool atmegaReadyRecieve = false;
char key;

static const char KEYS[NUM_ROWS][NUM_COLS] =
{
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
};



void Initialize(){
    lcd_init();
    DDRD &= ~(1 << BUTTON);
    
    // Set trigger pins as output
    DDRD |= (1 << TRIG1_PIN) | (1 << TRIG2_PIN);
    PORTD &= ~((1 << TRIG1_PIN) | (1 << TRIG2_PIN));

    // Set echo pins as input
    DDRD &= ~((1 << ECHO1_PIN) | (1 << ECHO2_PIN));
    PORTD &= ~((1 << ECHO1_PIN) | (1 << ECHO2_PIN));  // No pull-ups
    
    
}

// ========== Send Trigger Pulse ========== //
void trigger_sensor(uint8_t trig_pin) {
    PORTD |= (1 << trig_pin);
    _delay_us(TRIGGER_PULSE_US);
    PORTD &= ~(1 << trig_pin);
}

// ========== Measure Distance (in cm) ========== //
float get_distance(uint8_t trig_pin, uint8_t echo_pin) {
    trigger_sensor(trig_pin);

    uint32_t count = 0;

    // Wait for echo to go HIGH (start pulse)
    while (!(PIND & (1 << echo_pin))) {
        if (++count > MAX_ECHO_WAIT_US) return -1.0;
        _delay_us(1);
    }

    count = 0;
    // Measure HIGH duration (pulse width)
    while (PIND & (1 << echo_pin)) {
        if (++count > MAX_ECHO_WAIT_US) return -1.0;
        _delay_us(1);
    }

    // Convert time to distance (cm)
    float distance = (count * SOUND_SPEED_CM_PER_US) / 2.0;
    return distance;
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
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    //UCSR0B = (1 << TXEN0);
}

// Row pins on PB0..PB3
static const uint8_t rowPins[NUM_ROWS] = {PC5};
// Column pins on PD2..PD4
static const uint8_t colPins[NUM_COLS] = { PC4, PC3, PC2 };

// ---------------------------------------------------------------------------
//  initIO()
//     Sets up UART, then row pins as outputs, column pins as inputs w/ pull-ups,
//     prints debug info on DDR and PORT registers.
// ---------------------------------------------------------------------------
static void initIO(void)
{
    // 1) Init UART (so printf works)
    uart_init();  // Implementation depends on your setup; must set BAUD, etc.
    _delay_ms(10);

    //printf("\r\n--- KEYPAD DEBUG INIT ---\r\n");
   
    // 2) Rows = PB0..PB3 as outputs
    DDRC |= (1 << PC5);
    // 3) Cols = PD2..PD4 as inputs, with pull-ups
    DDRC &= ~((1 << PC2) | (1 << PC3) | (1 << PC4));  // Input
    PORTC |= (1 << PC2) | (1 << PC3) | (1 << PC4);    // Pull-up on

    // Print initial register states (in hex) for debugging
//    printf("DDRB=0x%02X, PORTB=0x%02X\r\n", DDRB, PORTB);
//    printf("DDRD=0x%02X, PORTD=0x%02X\r\n", DDRD, PORTD);
//
//    printf("--- INIT COMPLETE ---\r\n\r\n");
    _delay_ms(100);
}

// ---------------------------------------------------------------------------
//  driveRow(r):
//    - Sets all rows HIGH, then drives only row r LOW
//    - Prints debug info about which row is driven low.
// ---------------------------------------------------------------------------
static void driveRow(uint8_t r)
{
    // Set all rows HIGH
    //PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);

    PORTC |= (1 << PC5);
    // Now drive row r LOW
    PORTC &= ~(1 << rowPins[r]);

    // Debug: show which row we drove low
    //printf("[driveRow] Driving row %d low (PB%d), PORTB=0x%02X\r\n",
           //r, rowPins[r], PORTB);
}

// ---------------------------------------------------------------------------
//  scanKeypad():
//    - Goes through each row, drives that row LOW, checks columns
//    - Returns the first pressed key's char, or 0 if none
//    - Prints debug info about columns read, etc.
// ---------------------------------------------------------------------------
static char scanKeypad(void)
{
    for (uint8_t r = 0; r < NUM_ROWS; r++)
    {
        // Drive one row low, others high
        driveRow(r);
        _delay_us(5);  // let signals settle

        // Check each column in this row
        for (uint8_t c = 0; c < NUM_COLS; c++)
        {
            // We'll read PIND for that bit
            uint8_t mask = (1 << colPins[c]);
            uint8_t pindVal = PINC & mask;

            // Debug: show column reading
            //printf("   [scanRow=%d col=%d] PIND & 0x%02X = 0x%02X\r\n",
                  // r, c, mask, pindVal);

            // If pin is LOW => key pressed
            if (pindVal == 0)
            {
                // Debounce
                _delay_ms(50);

                // Wait until release (optional)
                while ((PINC & mask) == 0) {
                    // Could do small delay here
                }

                // Return the character from our KEYS array
                return KEYS[r][c];
            }
        }
    }
    // If we scanned all rows and found no press:
    return 0;
}

 void send_quarters(uint8_t snackIndex) {
     uint8_t numberQuarters = getPrice(snackIndex) / 0.25;
    
     while (!quartersRecieved) {
         // Wait for ATmega to be ready to receive
        // Send the number of quarters
        uart_send_int(numberQuarters);
            
        if (uart_data_available() && uart_receive_int() == 1) {
                 printf("Quarters acknowledged by ATmega\n");
                 quartersRecieved = true;
        }
             
         
     }
 }




// Step 1: Show the beginning display until keypad is pressed
// Step 2: Display snack info for 10000ms 
// Step 3: While loop till user 
// Step 4:


int main(void) {
    uart_init();
    Initialize();
    initIO();
    uart_rx_init();
    DDRD |= (1 << PD3);
    DDRD |= (1 << PD4);
    
    while (1) {
        // === Step 1: Initial State ===
        resetDisplayAfterDispense();  // Show welcome screen or reset LCD
        snackIndex = 0;
        userBalance = 0;
        key = 0;
        dispensed = false;
        coinDetection = false;
        quartersRecieved = false;
        atmegaReadyRecieve = false;
        numberQuarters = 0;
        _delay_ms(2500);
        LCD_setScreen(0xFFFF);
        LCD_drawString(30, 50, "Please select snack", 0x0000, 0xFFFF);
        LCD_drawString(30, 70, "By pressing keypad", 0x0000, 0xFFFF);
        _delay_ms(2500);
        // === Step 2: Simulate user selection ===
          // Replace with actual button or keypad input later
        LCD_setScreen(0xFFFF);
        LCD_drawString(30, 50, "Waiting for keypad", 0x0000, 0xFFFF);
        _delay_ms(2500);
        
      
        


        while (1) {
            key = scanKeypad();

            if (key >= '1' && key <= '9') {
                snackIndex = (key - '0') - 1;  // Convert ASCII to index
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

        // Going to send the quarters here 
        send_quarters(snackIndex);
        

        
        // MOTOR ATMEGA OUTPUT PIN, 
        // Need to check snackIndex first, send 
        // PD3, PD4 OUTPUT PINS FOR SNACK INDEX
        
        
        // TODO: Also got to set up UART here 
    
        
        
        // INPUT PIN TO CHECK IF COIN INSERTING IS DONE - PC0
        // === Step 4: Coin insertion loop ===
        while (coinDetection == false) {
            printf("Waiting for coin insert signal...\n");

            if (uart_data_available()) {
                uint8_t val = uart_receive_int();
                if (val == 1) {
                    printf("received 1\n");
                    coinDetection = true;
        
                    // Send back ACK (value 100)
                    uart_send_int(100);
                }
            }
        }
        // motor request
        while (waitingForMotorRequest) {
             if (uart_data_available()) {
                uint8_t request = uart_receive_int();
                if (request == 2) {
                    printf("received 2");
                    // Got signal from motor ATmega, send motor ID
                    _delay_ms(100);  // ensure the other side is ready
                    while (!motorPicked) {
                        uart_send_int(snackIndex == 0 ? 1 : 2);
                        if (uart_receive_int() == 3) {
                            motorPicked = true;
                        }
                    }
                    
                    printf("sent motor signal");
                    motorPicked = false;
                    waitingForMotorRequest = false;
                }
            }
        }
        waitingForMotorRequest = true;
        // if (snackIndex == 0) {
        //     uart_send_int(1);
        // } else if (snackIndex == 1) {
        //     uart_send_int(2);
        // }

        // Coin inserted, update balance
        userBalance = getPrice(snackIndex);
        displaySnackInfo(snackIndex, userBalance);
        updateStock(snackIndex);

        _delay_ms(2000);  // Optional delay

        LCD_setScreen(0xFFFF);
        LCD_drawString(30, 50, "Sufficient balance.", 0x0000, 0xFFFF);
        _delay_ms(2000);

        // === Step 5: Dispense snack ===
        // TODO: Dispensed = true when ultrasonic detects snack dispensed
        // Send a uart with a specific value when its detected the snack so motor stops 
        while (!dispensed) {
            dispenseSnack(snackIndex);
            distance1 = get_distance(TRIG1_PIN, ECHO1_PIN);
            distance2 = get_distance(TRIG2_PIN, ECHO2_PIN);
            if (distance1 < 0 && distance2 < 0) {
                printf("invalid reading \n");
            } else if (distance1 <= DETECTION_THRESHOLD_CM || distance2 <= DETECTION_THRESHOLD_CM) {
                printf("snack dispensed \n");
                dispensed = true;
                uart_send_int(30); // send to tell atmega snack dispensed.
            } else {
                printf("%s: No object detected \r\n");
            }
            _delay_ms(2000);
            
            
        }
        dispensed = false;
        
        LCD_setScreen(0xFFFF);
        LCD_drawString(15, 50, "Item Dispensed. Enjoy!", 0x0000, 0xFFFF);
        LCD_drawString(80, 90, ":)", 0x0000, 0xFFFF);

        _delay_ms(10000);  // Wait before looping again
    }
}

        
        
        
        
     
