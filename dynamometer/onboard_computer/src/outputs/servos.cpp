#include "servos.h"

#include <ESP32Servo.h>

#include "config.h"

Servo quick_disconnect_servo, oxidizer_servo;

bool servos::begin() {
    bool is_success = true;
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    quick_disconnect_servo.setPeriodHertz(50);
    if (quick_disconnect_servo.attach(SERVO_FUEL_PIN, SERVO_MIN_WIDTH, SERVO_MAX_WIDTH) == 0) is_success = false;
    oxidizer_servo.setPeriodHertz(50);
    if (oxidizer_servo.attach(SERVO_OXIDIZER_PIN, SERVO_MIN_WIDTH, SERVO_MAX_WIDTH) == 0) is_success = false;

    return is_success;
}

bool servos::rotateQuickDisconnect(int degrees) {
    quick_disconnect_servo.write(degrees);

    return true;
}

bool servos::rotateOxidizer(int degrees) {
    oxidizer_servo.write(degrees);

    return true;
}