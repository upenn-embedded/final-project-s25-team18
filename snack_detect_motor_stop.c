//SENSOR DETECTS THE DISTANCE 

float get_sensor_distance(uint8_t trig_pin, uint8_t echo_pin) {
    // Send a trigger pulse
    PORTD |= (1 << trig_pin);
    _delay_us(10);
    PORTD &= ~(1 << trig_pin);

    uint32_t count = 0;

    // Wait for echo HIGH
    while (!(PIND & (1 << echo_pin))) {
        if (++count > 25000) return -1;
        _delay_us(1);
    }

    count = 0;
    while (PIND & (1 << echo_pin)) {
        if (++count > 25000) return -1;
        _delay_us(1);
    }

    return (count * 0.0331) / 2.0;
}

void wait_for_snack_detection(uint8_t trig_pin, uint8_t echo_pin, int motor_number) {
    while (1) {
        float distance = get_sensor_distance(trig_pin, echo_pin);
        if (distance > 0 && distance < 20.0) {
            printf("Snack detected by sensor %d: %.2f cm\n", motor_number, distance);
            if (motor_number == 1) stop_motor();
            else if (motor_number == 2) stop_motor2();
            break;
        } else {
            printf("Waiting... Sensor %d distance: %.2f cm\n", motor_number, distance);
        }
        _delay_ms(100);
    }
}

//CHANGE IN MOTOR BLOCK AND PWM
