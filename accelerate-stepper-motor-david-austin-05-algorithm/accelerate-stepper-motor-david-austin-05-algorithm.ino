// Copyright (C) 2024 Joseph Morgridge
//
// Licensed under MIT License.
// See the LICENSE file in the project root for full license details.

/// @file accelerate-stepper-motor-david-austin-05-algorithm.ino
/// @brief An example to illustrate how to accelerate a stepper motor using the algorithm presented in:
/// ‘Embedded Systems Programming’ January 2005, ‘Generate stepper-motor speed profiles in real time’, an article by David Austin.

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
float Cn = 0.0; ///< nth speed (us), used to set microstep_period_in_flux_us.
const float f = 1000000.0; ///< Timer frequency (count of timer ticks per sec) (Hz).
uint64_t reference_time_us; ///< Reference time (us) for the microstep period.
/// Acceleration.
const float kAcceleration_microsteps_per_s_per_s = 3000.0; ///< Target acceleration (microsteps/s^2).
int32_t n = 0; ///< Iteration counter.
/// @}

/// @brief Other properties.
uint16_t kStartupTime_ms = 1000; ///< Minimum startup/boot time in milliseconds (ms); based on the stepper driver.

/// @brief Benchmarking.
bool benchmarked = false; ///< Flag to indicate if the acceleration time has been obtained.
uint64_t reference_calculation_time_us; ///< Reference time (us) for benchmarking.
uint32_t calculation_time_us; ///< Time taken (us) to calculate a new speed.

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

  // Calculate the speed/microstep period for n = 0. 
  Cn = 0.676 * f * sqrt(2.0 / kAcceleration_microsteps_per_s_per_s); // Equation 15.
  //Serial.print(F("C0 = ")); Serial.println(Cn);
  microstep_period_in_flux_us = Cn;
  n = 1;

  // Delay for the startup time.
  delay(kStartupTime_ms);
}

/// @brief The continuously running function for repetitive tasks.
void loop() {
  // Move until target distance is reached.
  if (distance_microsteps > 0) AccelerateAndMoveAtSpeed();
}

/// @brief Move by a microstep.
void MoveByMicrostep() {
  digitalWrite(kPulPin, LOW);
  delayMicroseconds(kPulDelay);
  digitalWrite(kPulPin, HIGH);
  //delayMicroseconds(kPulDelay); // May not be needed.

  if (distance_microsteps != 0) {
    distance_microsteps--;
  }
}

/// @brief Accelerate the motor to the max speed and then move at the max speed.
void AccelerateAndMoveAtSpeed() {
  unsigned long current_time_us = micros();
  if ((current_time_us - reference_time_us) >= microstep_period_in_flux_us) {    
    MoveByMicrostep();
    if (benchmarked == false) reference_calculation_time_us = micros();
    CalculateNewSpeed();
    if (benchmarked == false) {
      calculation_time_us = micros() - reference_calculation_time_us;
      benchmarked = true;
      Serial.print(F("calculation time (us) = "));
      Serial.println(calculation_time_us);
    }
    reference_time_us = current_time_us;
  }
}

/// @brief Calculate the new speed/microstep period.
void CalculateNewSpeed() {
  if (microstep_period_in_flux_us > kMicrostepPeriod_us) {
    Cn = Cn - ((2.0 * Cn) / ((4.0 * n) + 1)); // Equation 13.
    //Serial.print(F("C")); Serial.print(n); Serial.print(F(" = ")); Serial.println(Cn);
    n++;
  }
  else {
    Cn = kMicrostepPeriod_us;
  }

  microstep_period_in_flux_us = Cn;
}