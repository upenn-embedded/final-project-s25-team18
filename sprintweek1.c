#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define F_CPU 16000000UL  // or whatever clock speed your ATmega328PB is running at
#define MOTOR_PIN PD6        // Use PD4 to drive MOSFET gate
#define ENTRY_SENSOR PD2      // INT0
#define EXIT_SENSOR PD3       // INT1

#define MIN_VALID_TIME 50     // Adjust based on your setup
#define MAX_VALID_TIME 500

volatile uint16_t start_time = 0;
volatile uint16_t end_time = 0;
volatile uint8_t coin_valid = 0;

ISR(INT0_vect) {
    // Entry beam broken
    if ((PIND & (1 << PIND2)) == 0) {
        start_time = TCNT1;
    }
}

ISR(INT1_vect) {
    // Exit beam broken
    if ((PIND & (1 << PIND3)) == 0) {
        end_time = TCNT1;
        uint16_t delta = end_time - start_time;
        if (delta >= MIN_VALID_TIME && delta <= MAX_VALID_TIME) {
            coin_valid = 1;
        }
        start_time = 0; // reset
    }
}

void PWM_init() {
    // Set PD6 (OC0A) as output
    DDRD |= (1 << PD6);

    // Set Fast PWM mode: WGM02:0 = 3 (Mode 3)
    TCCR0A |= (1 << WGM00) | (1 << WGM01);


    // Non-inverting mode on OC0A
    TCCR0A |= (1 << COM0A1);

    // Set prescaler to 64 → 16 MHz / 64 = 250 kHz
    TCCR0B |= (1 << CS01) | (1 << CS00);

    // Set duty cycle to 25%
    OCR0A = 0; // 64 / 255 ≈ 25%
}

int main(void) {
    PWM_init();
  

    // Enable INT0 and INT1
    EIMSK |= (1 << INT0) | (1 << INT1);

    // Trigger on falling edge (beam broken → HIGH to LOW)
    EICRA |= (1 << ISC01);  // INT0 falling edge
    EICRA |= (1 << ISC11);  // INT1 falling edge

    // Set up Timer1 with prescaler = 8
    TCCR1B |= (1 << CS11);

    sei();  // Enable global interrupts

    while (1) {
        if (coin_valid) {
            coin_valid = 0;

            // Turn on motor
            OCR0A = 64;
            _delay_ms(5000);
            OCR0A = 0;
        }
    }
}