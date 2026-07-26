#include "servos.h"

#include <ESP32Servo.h>

#include "config.h"

Servo fuel_servo, oxidizer_servo;

bool servos::begin() {
    bool is_success = true;
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    fuel_servo.setPeriodHertz(50);
    if (fuel_servo.attach(SERVO_FUEL_PIN, SERVO_MIN_WIDTH, SERVO_MAX_WIDTH) == 0) is_success = false;
    oxidizer_servo.setPeriodHertz(50);
    if (oxidizer_servo.attach(SERVO_OXIDIZER_PIN, SERVO_MIN_WIDTH, SERVO_MAX_WIDTH) == 0) is_success = false;

    return is_success;
}

bool servos::rotateFuel(int degrees) {
    fuel_servo.write(degrees);

    return true;
}

bool servos::rotateOxidizer(int degrees) {
    oxidizer_servo.write(degrees);

    return true;
}