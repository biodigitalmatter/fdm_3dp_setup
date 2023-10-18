#include <Arduino.h>

#include <AccelStepper.h>
#include <Controllino.h>
#include <VoltageReference.h>

#include "DirectionController.h"

// SETTINGS
const int HOTEND_TEMP_DEGREES_C = 180;
const int EXTRUDER_RPM = 60;

// #define DEBUG

// #define USE_STEPPER_ENABLE_PIN

// Set motor
// #define E3D_HEMERA
#define MICRO_SWISS_DD

// PINS

// Comm pins
const int DI_ROBOT_HEAT_UP_PIN = CONTROLLINO_SCREW_TERMINAL_DIGITAL_ADC_IN_01;
const int DI_ROBOT_RUN_BACKWARDS_PIN =
    CONTROLLINO_SCREW_TERMINAL_DIGITAL_ADC_IN_02;
const int DI_ROBOT_RUN_FORWARD_PIN =
    CONTROLLINO_SCREW_TERMINAL_DIGITAL_ADC_IN_03;

// Stepper motor pins
#ifdef USE_STEPPER_ENABLE_PIN
const int DO_NC_ENABLE_PIN = CONTROLLINO_PIN_HEADER_DIGITAL_OUT_01;
#endif
const int DO_DIR_PIN = CONTROLLINO_PIN_HEADER_DIGITAL_OUT_03;
const int DO_STEP_PIN = CONTROLLINO_PIN_HEADER_DIGITAL_OUT_02;  // Step/Pulse

// hotend pins
const int DO_HEATING_PIN = CONTROLLINO_SCREW_TERMINAL_RELAY_04;
const int AI_THERMISTOR_PIN = CONTROLLINO_PIN_HEADER_ANALOG_ADC_IN_00;

// THERMISTOR
// interval at which to check temperature
const uint16_t TEMP_CONTROL_INTERVAL_MILLIS = 1000;
// 4k7 Ohm reference resistor
const uint32_t THERMISTOR_SETUP_FIXED_R1_OHM = 4700;

// Obtained Steinhart-Hart values from:
// https://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm
// https://download.lulzbot.com/retail_parts/Completed_Parts/100k_Semitec_NTC_Thermistor_235mm_KT-CP0110/GT-2-glass-thermistors.pdf
// beta-value (at 25 degrees): 4267K
// resistance (Ohm) | degrees (C)
// 353700           |   0
//   5556           | 100
//   439.3          | 200
const float SH_A = 0.8097317731e-03, SH_B = 2.11635527e-04,
            SH_C = 0.7066084133e-07;

uint32_t g_temp_previous_millis = 0;
float g_temperature = 999.9;
uint16_t g_thermistor_target_analog_value = 1023;

const float STEP_ANGLE_DEGREES = 1.8;

#if defined(E3D_HEMERA) && defined(MICRO_SWISS_DD)
#error "Only one motor configuration should be defined!"
#elif defined(E3D_HEMERA)
const float GEAR_RATIO = 1 / 3.32;
#elif defined(MICRO_SWISS_DD)
const float GEAR_RATIO = 1.0;
#else
#error "Please define a motor configuration!"
#endif

const uint8_t MICROSTEP_MULTIPLIER = 8;
const bool STEPPER_INVERT_DIR = true;

const uint16_t MAX_STEPS_PER_SEC = 3200;

// Create a new instance of the AccelStepper class:
AccelStepper g_stepper =
    AccelStepper(AccelStepper::DRIVER, DO_STEP_PIN, DO_DIR_PIN);

uint16_t g_reference_voltage_millivolt = 0;
VoltageReference g_VRef;

float readThermistorTemperature() {
  // TODO: Replace with <Thermistor.h>?
  uint16_t analogValue = analogRead(AI_THERMISTOR_PIN);

  uint32_t Vout = (analogValue * g_reference_voltage_millivolt) / 1023;

  // Calculate the thermistor resistance
  float thermistor_ohm = THERMISTOR_SETUP_FIXED_R1_OHM *
                         (g_reference_voltage_millivolt / Vout - 1.0);

  // Convert resistance to temperature using the Steinhart-Hart equation
  // coefficients (c1, c2, c3)
  float T = 1.0 / (SH_A + SH_B * log(thermistor_ohm) +
                   SH_C * pow(log(thermistor_ohm), 3));

  // Convert temperature from Kelvin to Celsius
  float Tc = T - 273.15;

  return Tc;
}

float rpm_to_steps_per_sec(float rpm, float step_angle_degrees,
                           float microstep_multiplier = 1.0,
                           float gear_ratio = 1.0) {
  float steps_per_rev = 360.0 / step_angle_degrees;

  steps_per_rev = steps_per_rev * microstep_multiplier;
  steps_per_rev = steps_per_rev * gear_ratio;

  float steps_per_min = rpm * steps_per_rev;

  float steps_per_sec = steps_per_min / 60.;

  return steps_per_sec;
}

const int STEPS_PER_SEC = round(rpm_to_steps_per_sec(
    EXTRUDER_RPM, STEP_ANGLE_DEGREES, MICROSTEP_MULTIPLIER, GEAR_RATIO));

void setup() {
  // Leave on default unless internal or external is required
  analogReference(DEFAULT);
  g_VRef.begin();
  g_reference_voltage_millivolt = g_VRef.readVcc();

#ifdef USE_STEPPER_ENABLE_PIN
  g_stepper.setEnablePin(DO_NC_ENABLE_PIN);
#endif

  g_stepper.setPinsInverted(/* directionInvert */ STEPPER_INVERT_DIR);
  g_stepper.setMaxSpeed(MAX_STEPS_PER_SEC);

  // setup robot input pin
  pinMode(DI_ROBOT_HEAT_UP_PIN, INPUT);
  pinMode(DI_ROBOT_RUN_BACKWARDS_PIN, INPUT);
  pinMode(DI_ROBOT_RUN_FORWARD_PIN, INPUT);

  // hotend pins
  pinMode(DO_HEATING_PIN, OUTPUT);
  digitalWrite(DO_HEATING_PIN, LOW);

#ifdef DEBUG
  Serial.begin(9600);
#endif
}

void loop() {
  Direction dir =
      determineDirection(DI_ROBOT_RUN_BACKWARDS_PIN, DI_ROBOT_RUN_FORWARD_PIN);
  int16_t signed_steps_per_sec = 0;

  switch (dir) {
    case FORWARD:
      signed_steps_per_sec = 1 * STEPS_PER_SEC;
      break;
    case BACKWARD:
      signed_steps_per_sec = -1 * STEPS_PER_SEC;
      break;
    case STOP:
      // No change in speed
      break;
  }

  g_stepper.setSpeed(signed_steps_per_sec);

  g_stepper.runSpeed();

  bool heat_up = digitalRead(DI_ROBOT_HEAT_UP_PIN) == HIGH;

  // Only check temperature if TEMP_CONTROL_INTERVAL_MILLIS has passed
  // This is because the check is not very fast
  if (millis() - g_temp_previous_millis > TEMP_CONTROL_INTERVAL_MILLIS) {
    g_temp_previous_millis = millis();
    g_temperature = readThermistorTemperature();
#ifdef DEBUG
    Serial.print("Speed:");
    Serial.print(signed_steps_per_sec);
    Serial.println(",");
    Serial.print("Current_temp:");
    Serial.print(g_temperature);
    Serial.println(",");
    Serial.print("Target_temp:");
    Serial.println(heat_up ? HOTEND_TEMP_DEGREES_C : 0);
#endif
  }

  if (!heat_up) {
    // make sure that heater is off when signal is low
    digitalWrite(DO_HEATING_PIN, LOW);
  } else {
    // but if signal is high:
    uint8_t pinstate = g_temperature < HOTEND_TEMP_DEGREES_C ? HIGH : LOW;
    digitalWrite(DO_HEATING_PIN, pinstate);
  }
}
