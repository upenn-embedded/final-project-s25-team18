#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "uart2.h"

#define F_CPU 16000000UL

// === Pin Definitions ===
#define TRIG_PIN  PB1
#define ECHO_PIN  PB0
#define MOTOR_PIN PD5  // PWM output on OC0B

// === Timer Settings ===
#define TIMER2_OCR_VALUE  39      // Timer2 = 20us intervals
#define TIMER1_PRESCALER  256
#define TICKS_VAL         62500UL

#define SPEED_OF_SOUND_IN_CM_S (331.0 / 10.0)
#define DETECTION_THRESHOLD_CM 5.0

// === Globals ===
volatile uint16_t ticks_t1 = 0;
volatile uint16_t ticks_t2 = 0;
volatile bool got_falling_edge = false;

typedef enum { INIT_RISING, RISING, FALLING } edge_state_t;
volatile edge_state_t next_edge = INIT_RISING;

volatile uint16_t tick_count = 0;

uint8_t item_count = 0;
bool object_in_front = false;
bool motor_started = false;

// === Function Prototypes ===
static void init_input_capture(void);
static void init_timer2(void);
static float compute_distance_cm(uint16_t t1, uint16_t t2);
void pwm_init(void);
void start_motor_75(void);
void stop_motor(void);

// === PWM Motor Setup (Timer0 on OC0B / PD5) ===
void pwm_init(void) {
    DDRD |= (1 << MOTOR_PIN);  // Set PD5 as output
    TCCR0A |= (1 << COM0B1) | (1 << WGM01) | (1 << WGM00); // Fast PWM, non-inverting
    TCCR0B |= (1 << CS01) | (1 << CS00); // Prescaler = 64
    OCR0B = 0; // Motor off initially
}

void start_motor_75(void) {
    OCR0B = 191;  // 75% duty cycle (191/255)
    printf("Motor started at 75%% speed\n");
}

void stop_motor(void) {
    OCR0B = 0;
    printf("Motor stopped\n");
}

// === Input Capture Init ===
static void init_input_capture(void) {
    TCCR1A = 0;
    TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS12);
    TCCR1C = 0;
    TIFR1  = (1 << ICF1);
    TIMSK1 = (1 << ICIE1);
}

// === Timer2 Init (Trigger Pulse) ===
static void init_timer2(void) {
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS21);
    OCR2A  = TIMER2_OCR_VALUE;
    TIFR2  = (1 << OCF2A);
    TIMSK2 = (1 << OCIE2A);
}

void tx_uart_setup(void) {
    // Set baud rate
    uint16_t ubrr = 103; // 9600 baud @ 16MHz
    UBRR0H = (ubrr >> 8);
    UBRR0L = ubrr;

    // Enable transmitter
    UCSR0B = (1 << TXEN0);

    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// === Distance Calculation ===
static float compute_distance_cm(uint16_t t1, uint16_t t2) {
    float temp_us;
    if (t2 >= t1) {
        temp_us = (float)(t2 - t1) / (float)TICKS_VAL;
    } else {
        temp_us = (float)((65536UL - t1) + t2) / (float)TICKS_VAL;
    }
    temp_us *= 1000000.0;
    return (SPEED_OF_SOUND_IN_CM_S * temp_us) / 2000.0;
}

// === Main ===
int main(void) {
    // I/O setup
    DDRB |= (1 << TRIG_PIN);
    PORTB &= ~(1 << TRIG_PIN);

    DDRB &= ~(1 << ECHO_PIN);
    PORTB &= ~(1 << ECHO_PIN);
    
    
    

    uart_init();
    pwm_init();
    tx_uart_setup();

    printf("System Ready: Item Counter + Motor Logic\n");

    init_input_capture();
    init_timer2();
    sei();

    while (1) {
        
        if (got_falling_edge) {
            got_falling_edge = false;

            float distance = compute_distance_cm(ticks_t1, ticks_t2);

            if (distance > 0.0 && distance < 1000.0) {
                if (distance <= DETECTION_THRESHOLD_CM) {
                    if (!object_in_front) {
                        item_count++;
                        printf("Object detected! Count: %d\n", item_count);
                        object_in_front = true;
                    }
                } else {
                    if (object_in_front) {
                        printf("Object moved away. Count: %d\n", item_count);
                    } else {
                        printf("No object. Count: %d\n", item_count);
                    }
                    object_in_front = false;
                }
            } else {
                printf("Invalid reading\n");
                object_in_front = false;
            }

            // === Motor Trigger ===
            if (item_count == 4 && !motor_started) {
                uart_send_int(1);
                motor_started = true;
                start_motor_75();

                // Run motor for 30 seconds
                for (int i = 0; i < 300; i++) {
                    _delay_ms(100);
                }

                stop_motor();

                // === Reset logic ===
                item_count = 0;
                motor_started = false;
                object_in_front = false;
                printf("Counter reset after motor spin. Ready for new items!\n");
            }

            _delay_ms(500); // Debounce / reading delay
        }
    }

    return 0;
}

// === ISR: Input Capture ===
ISR(TIMER1_CAPT_vect) {
    uint16_t capture = ICR1;

    if ((TCCR1B & (1 << ICES1)) != 0) {
        ticks_t1 = capture;
        TCCR1B &= ~(1 << ICES1);  // next: falling edge
        next_edge = FALLING;
    } else {
        ticks_t2 = capture;
        got_falling_edge = true;
        TCCR1B |= (1 << ICES1);  // next: rising edge
        next_edge = RISING;
    }
}

// === ISR: Timer2 for Trigger Pulse ===
ISR(TIMER2_COMPA_vect) {
    tick_count++;

    if (tick_count == 1) {
        PORTB |= (1 << TRIG_PIN);  // Trigger HIGH
    } else if (tick_count == 2) {
        PORTB &= ~(1 << TRIG_PIN); // Trigger LOW
    } else if (tick_count == 2500) {
        tick_count = 0; // ~50ms trigger interval
    }
}
