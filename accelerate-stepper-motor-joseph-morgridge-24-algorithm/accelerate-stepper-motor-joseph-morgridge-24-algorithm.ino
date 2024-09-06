// Copyright (C) 2024 Joseph Morgridge
//
// Licensed under MIT License.
// See the LICENSE file in the project root for full license details.

/// @file accelerate-stepper-motor-joseph-morgridge-24-algorithm.ino
/// @brief An example to illustrate how to accelerate a stepper motor using an algorithm developed based on the equations;
/// v = u + at and s = ut + 1/2at^2
/// These are simplified based on the conditions under which the algorithm is implemented on a microcontroller.

/// @{
/// @brief GPIO pins.
const uint16_t kPulPin = 11; ///< For the stepper driver PUL/STP/CLK (pulse/step) pin.
const uint16_t kDirPin = 12; ///< For the stepper driver DIR/CW (direction) pin.
const uint16_t kEnaPin = 13; ///< For the stepper driver ENA/EN (enable) pin.
/// @}

/// @brief Serial properties.
int kBaudRate = 9600; ///< The serial communication speed.

/// @{
/// @brief Stepper driver properties.
const uint16_t kMicrostepMode = 16; ///< Microstep mode (1/16). Remember to change the setting on the stepper driver to match.
const uint8_t kPulDelay = round(2.5); ///< Minimum time in microseconds (us) to delay after a low or high-level pulse of the PUL pin. Obtained from the stepper driver data sheet.
/// @}

/// @{
/// @brief Stepper motor/drive system properties.
const float kFullStepAngle_degrees = 1.8; ///< The full step angle (degrees).
const float kGearRatio = 1.0; ///< Gear ratio for motors coupled with a gearbox in the drive system.
const float kMicrostepAngle_degrees = kFullStepAngle_degrees / (kGearRatio * kMicrostepMode); ///< = 0.1125 Microstep angle (degrees).
/// @}

/// @{
/// @brief Motor states and targets.
/// Distance.
const float kDistance_degrees = 3600.0; ///< Target distance (degrees).
uint32_t distance_microsteps = kDistance_degrees / kMicrostepAngle_degrees; ///< = 32000 Target distance (microsteps).
/// Speed.
const float kSpeed_RPM = 150.0; ///< Target speed (RPM).
const float kSpeed_microsteps_per_s = (6.0 * kSpeed_RPM) / kMicrostepAngle_degrees; ///< = 8000.0 Target speed (microsteps/s).
const uint32_t kMicrostepPeriod_us = 1000000.0 / kSpeed_microsteps_per_s; ///< = 125 Target speed based on the microstep period (us) between microsteps.
uint32_t microstep_period_in_flux_us; ///< The microstep period (us) that is changing due to acceleration.
float vi_microsteps_per_s = 0.0; ///< ith speed (microsteps/s), used to calculate microstep_period_in_flux_us.
float Ti_us = 0.0; ///< ith speed (us), used to set the microstep_period_in_flux_us.
uint64_t reference_time_us; ///< Reference time (us) for the microstep period.
/// Acceleration.
const float kAcceleration_microsteps_per_s_per_s = 3000.0; ///< Target acceleration (microsteps/s^2).
uint32_t i = 1; ///< Iteration counter.
/// @}

/// @brief Other properties.
uint16_t kStartupTime_ms = 1000; ///< Minimum startup/boot time in milliseconds (ms); based on the stepper driver.

/// @brief The main application entry point for initialisation tasks.
void setup() {
  // Initialise the serial port.
  Serial.begin(kBaudRate);

  // Initialise the output pins.
  pinMode(kPulPin, OUTPUT); // Pulse/PUL/CLK.
  pinMode(kDirPin, OUTPUT); // Direction/DIR/CW.
  pinMode(kEnaPin, OUTPUT); // Enable/ENA/EN.

  // Activate the stepper driver.
  //digitalWrite(kEnaPin, LOW); // This is usually activated by default, hence this may not be required.

  // Set motion direction (if required).
  //digitalWrite(kDirPin, HIGH);

  // Calculate the speed/microstep period for i = 1.
  vi_microsteps_per_s = kAcceleration_microsteps_per_s_per_s * sqrt(2.0 / kAcceleration_microsteps_per_s_per_s);
  Ti_us = 1000000.0 / vi_microsteps_per_s;
  //Serial.print(F("v1_microsteps_per_s = ")); Serial.println(vi_microsteps_per_s);
  //Serial.print(F("T1_us = ")); Serial.println(Ti_us);
  microstep_period_in_flux_us = Ti_us;
  i = 2;

  // Delay for the startup time.
  delay(kStartupTime_ms);
}

/// @brief The continuously running function for repetitive tasks.
void loop() {
  if (distance_microsteps <= 0) {
    // Reached target distance.
    return;
  }

  AccelerateAndMoveAtSpeed();
}

/// @brief Move by a microstep.
void MoveByMicrostep() {
  digitalWrite(kPulPin, LOW);
  delayMicroseconds(kPulDelay);
  digitalWrite(kPulPin, HIGH);

  if (distance_microsteps != 0) {
    distance_microsteps--;
  }
}

/// @brief Accelerate the motor to the max speed and then move at the max speed.
void AccelerateAndMoveAtSpeed() {
  unsigned long current_time_us = micros();
  if ((current_time_us - reference_time_us) >= microstep_period_in_flux_us) {    
    MoveByMicrostep();
    CalculateNewSpeed();
    reference_time_us = current_time_us;
  }
}

/// @brief Calculate the new speed/microstep period.
void CalculateNewSpeed() {
  if (microstep_period_in_flux_us > kMicrostepPeriod_us) {
    vi_microsteps_per_s = vi_microsteps_per_s + (kAcceleration_microsteps_per_s_per_s / vi_microsteps_per_s);
    Ti_us = 1000000.0 / vi_microsteps_per_s;
    //Serial.print(F("v")); Serial.print(i); Serial.print(F("_microsteps_per_s = ")); Serial.println(vi_microsteps_per_s);
    //Serial.print(F("T")); Serial.print(i); Serial.print(F("_us = ")); Serial.println(Ti_us);
    i++;
  }
  else {
    Ti_us = kMicrostepPeriod_us;
  }

  microstep_period_in_flux_us = Ti_us;
}