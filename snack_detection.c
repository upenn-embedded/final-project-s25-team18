#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "uart.h"

#define F_CPU 16000000UL

// Pin mappings for Sensor 1 and Sensor 2
#define TRIG1_PIN PD3
#define ECHO1_PIN PD4

#define TRIG2_PIN PD7
#define ECHO2_PIN PD5

// Speed of sound = 0.0331 cm/us
#define SOUND_SPEED_CM_PER_US 0.0331

// Ultrasonic trigger pulse and timeout
#define TRIGGER_PULSE_US 10
#define MAX_ECHO_WAIT_US 25000  // Timeout threshold (~8 ft)

// Distance threshold for detection
#define DETECTION_THRESHOLD_CM 5.0

// ========== Function Prototypes ========== //
void init_pins();
void trigger_sensor(uint8_t trig_pin);
float get_distance(uint8_t trig_pin, uint8_t echo_pin);
void debug_sensor(uint8_t trig_pin, uint8_t echo_pin, const char* sensor_label);

// ========== Main ========== //
int main(void) {
    uart_init();
    init_pins();

    printf("System Initialized: Dual Ultrasonic Sensors (UART Mode)\r\n");

    while (1) {
        debug_sensor(TRIG1_PIN, ECHO1_PIN, "Snack 1");
        debug_sensor(TRIG2_PIN, ECHO2_PIN, "Snack 2");
        _delay_ms(100);  // Small delay between readings
    }

    return 0;
}

// ========== Initialize I/O Pins ========== //
void init_pins() {
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

// ========== Print Debug Info ========== //
void debug_sensor(uint8_t trig_pin, uint8_t echo_pin, const char* sensor_label) {
    float distance = get_distance(trig_pin, echo_pin);

    if (distance < 0) {
        printf("%s: No reading (timeout)\r\n", sensor_label);
    } else if (distance <= DETECTION_THRESHOLD_CM) {
        printf("%s detected: %.2f cm\r\n", sensor_label, distance);
    } else {
        printf("%s: No object detected (%.2f cm)\r\n", sensor_label, distance);
    }
}
