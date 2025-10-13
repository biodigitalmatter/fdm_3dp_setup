#include <Arduino.h>
// Install AccelStepper: Tools > Manage libraries > Search for and install "AccelStepper"
#include <AccelStepper.h>
// Install Controllino: Tools > Manage libraries > Search for and install "Controllino"
// Install and target board: See https://www.controllino.com/wp-content/uploads/2023/07/CONTROLLINO-Instruction-Manual-V1.3-2023-05-15.pdf
#include <Controllino.h>


#define DEBUG

//#define USE_STEPPER_ENABLE_PIN
// Set motor
//#define E3D_HEMERA
#define MICRO_SWISS_DD

// SETTINGS
const int HOTEND_TEMP_DEGREES_C = 175;
const int EXTRUDER_RPM = 20;

// PINS

#ifndef Controllino_h
#error "Pins only setup for Controllino for now."
#endif

// Pin comment:
// Arduino Uno name, port pin, physical pin, Controllino function
// https://www.controllino.com/wp-content/uploads/2023/05/CONTROLLINO_MINI_Pinout_Table.pdf
// https://github.com/CONTROLLINO-PLC/CONTROLLINO_Library/blob/master/Controllino.h

// Comm pins
const int DI_HEAT_UP_PIN = CONTROLLINO_SCREW_TERMINAL_DIGITAL_ADC_IN_01;        // A1, PC1, 24, Analog 1
const int DI_RUN_BACKWARDS_PIN = CONTROLLINO_SCREW_TERMINAL_DIGITAL_ADC_IN_02;  // A2, PC2, 25, Analog 2
const int DI_RUN_FORWARD_PIN = CONTROLLINO_SCREW_TERMINAL_DIGITAL_ADC_IN_03;    // A3, PC3, 26, Analog 3

// Stepper motor pins
#ifdef USE_STEPPER_ENABLE_PIN
const int DO_NC_ENABLE_PIN = CONTROLLINO_PIN_HEADER_DIGITAL_OUT_05;             // D9, PB1, 13, Digital 5
#endif
const int DO_STEP_PIN = CONTROLLINO_PIN_HEADER_DIGITAL_OUT_06;                  // A4, PC4, 27, Digital 6/SDA
const int DO_DIR_PIN = CONTROLLINO_PIN_HEADER_DIGITAL_OUT_07;                   // A5, PC5, 28, Digital 7/SCL

// hotend pins
const int DO_HEATING_PIN = CONTROLLINO_SCREW_TERMINAL_RELAY_03;                 // D7, PD7, 11, D3 (Relay 3)
const int AI_THERMISTOR_PIN = CONTROLLINO_PIN_HEADER_ANALOG_ADC_IN_00;          // A0, PC0, 23, Analog 0

// LED
#ifdef Controllino_h
const int DO_STEPPER_DEBUG_LED = CONTROLLINO_D0;
#else
const int DO_STEPPER_DEBUG_LED = LED_BUILTIN;
#endif

const unsigned long LED_INTERVAL = 1000;  // interval at which to blink (milliseconds)
unsigned long g_led_previous_millis = 0;
int g_LED_blink_state = LOW;

// THERMISTOR
const long TEMP_CONTROL_INTERVAL_MILLIS = 2000;  // interval at which to check temperature

const float THERMISTOR_SETUP_FIXED_R1_OHM = 4700.0;  // 4k7 Ohm reference resistor

enum AnalogReferenceType { AREF_DEFAULT,
                           AREF_INTERNAL,
                           AREF_EXTERNAL
                         };
const AnalogReferenceType ANALOG_REFERENCE_TYPE = AREF_INTERNAL;

// Obtained Steinhart-Hart values from:
// https://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.htm
// https://download.lulzbot.com/retail_parts/Completed_Parts/100k_Semitec_NTC_Thermistor_235mm_KT-CP0110/GT-2-glass-thermistors.pdf
// https://smoothieware.org/temperaturecontrol-thermistor-choice
// beta-value (at 25 degrees): 4375K
// resistance (Ohm) | degrees (C)
// 353700           |   0
//   439.3          | 200
//   125.8          | 270
const float SH_A = 0.8109916996e-03, SH_B = 2.113966629e-04, SH_C = 0.7152004502e-07;

unsigned long g_temp_previous_millis = 0;

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

const int MICROSTEP_MULTIPLIER = 8;
const bool STEPPER_INVERT_DIR = false;

const int MAX_STEPS_PER_SEC = 3200;

// Create a new instance of the AccelStepper class:
AccelStepper g_stepper = AccelStepper(AccelStepper::DRIVER, DO_STEP_PIN, DO_DIR_PIN);

// Get the reference voltage based on the current analog reference type
float getReferenceVoltage() {
  if (ANALOG_REFERENCE_TYPE == INTERNAL) {
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328PB__)
    return 1.1;  // 1.1V for ATmega328P, ATmega168, and ATmega328PB (Arduino Uno, Nano, and Pro Mini 5V)
#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    return 1.1;  // 1.1V for ATmega32U4, ATmega1280, and ATmega2560 (Arduino Leonardo, Mega 2560)
#else
    return 3.3;  // 3.3V for other 3.3V boards
#endif
  } else if (ANALOG_REFERENCE_TYPE == DEFAULT) {
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    return 5.0;  // 5.0V for ATmega32U4, ATmega1280, and ATmega2560 (Arduino Leonardo, Mega 2560)
#else
    return 3.3;  // 3.3V for other 3.3V boards
#endif
  } else {
    // For external reference, set the voltage manually (e.g., 3.3V or 5V)
    return 3.3;
  }
}

float readThermistorTemperature() {
  // TODO: Replace with <Thermistor.h>?
  // Read the analog value (0-1023) from the thermistor pin
  int analogValue = analogRead(AI_THERMISTOR_PIN);

#ifdef DEBUG
  Serial.print("V:");
  Serial.print(analogValue);
  Serial.print(",");
#endif

  float referenceVoltage = getReferenceVoltage();

  // Convert the analog value to voltage (using the internal reference voltage)
  float Vout = (analogValue * referenceVoltage) / 1023.0;

  // Calculate the thermistor resistance
  float thermistor_ohm = THERMISTOR_SETUP_FIXED_R1_OHM * (referenceVoltage / Vout - 1.0);

  // Convert resistance to temperature using the Steinhart-Hart equation coefficients (c1, c2, c3)
  float T = 1.0 / (SH_A + SH_B * log(thermistor_ohm) + SH_C * pow(log(thermistor_ohm), 3));

  // Convert temperature from Kelvin to Celsius
  float Tc = T - 273.15;

#ifdef DEBUG
  Serial.print("T:");
  Serial.println(Tc);
#endif

  return Tc;
}

float rpm_to_steps_per_sec(float rpm, float step_angle_degrees, float microstep_multiplier = 1.0, float gear_ratio = 1.0) {
  float steps_per_rev = 360.0 / step_angle_degrees;

  steps_per_rev = steps_per_rev * microstep_multiplier;
  steps_per_rev = steps_per_rev * gear_ratio;

  float steps_per_min = rpm * steps_per_rev;

  float steps_per_sec = steps_per_min / 60.;

  return steps_per_sec;
}

const int STEPS_PER_SEC = round(rpm_to_steps_per_sec(EXTRUDER_RPM, STEP_ANGLE_DEGREES, MICROSTEP_MULTIPLIER, GEAR_RATIO));

void setup() {
  analogReference(ANALOG_REFERENCE_TYPE);

#ifdef USE_STEPPER_ENABLE_PIN
  g_stepper.setEnablePin(DO_NC_ENABLE_PIN);
#endif

  g_stepper.setPinsInverted(/* directionInvert */ STEPPER_INVERT_DIR);
  g_stepper.setMaxSpeed(MAX_STEPS_PER_SEC);

  pinMode(DO_STEPPER_DEBUG_LED, OUTPUT);
  digitalWrite(DO_STEPPER_DEBUG_LED, LOW);

  // setup robot input pin
  pinMode(DI_HEAT_UP_PIN, INPUT_PULLUP);
  pinMode(DI_RUN_BACKWARDS_PIN, INPUT_PULLUP);
  pinMode(DI_RUN_FORWARD_PIN, INPUT_PULLUP);

  // hotend pins
  pinMode(DO_HEATING_PIN, OUTPUT);
  digitalWrite(DO_HEATING_PIN, LOW);

  pinMode(AI_THERMISTOR_PIN, INPUT);

  // led
  pinMode(DO_STEPPER_DEBUG_LED, OUTPUT);
  digitalWrite(DO_STEPPER_DEBUG_LED, LOW);

#ifdef DEBUG
  Serial.begin(9600);
#endif
}

void loop() {

  bool forwards = digitalRead(DI_RUN_FORWARD_PIN) == HIGH;
  bool backwards = digitalRead(DI_RUN_BACKWARDS_PIN) == HIGH;

  int signed_steps_per_sec = 0;

  if (forwards && !backwards) {
    signed_steps_per_sec = 1 * STEPS_PER_SEC;
    digitalWrite(DO_STEPPER_DEBUG_LED, HIGH);
  } else if (!forwards && backwards) {
    signed_steps_per_sec = -1 * STEPS_PER_SEC;

    // blink when reversing
    if (millis() - g_led_previous_millis >= LED_INTERVAL) {
      g_led_previous_millis = millis();
      g_LED_blink_state = g_LED_blink_state == HIGH ? LOW : HIGH;
      digitalWrite(DO_STEPPER_DEBUG_LED, g_LED_blink_state);
    }
  } else {  // both false or both true
    digitalWrite(DO_STEPPER_DEBUG_LED, LOW);
    // don't touch speed
  }

  g_stepper.setSpeed(signed_steps_per_sec);

  g_stepper.runSpeed();

  bool heat_up = digitalRead(DI_HEAT_UP_PIN) == HIGH;
  // make sure that heater is off when signal is low
  if (!heat_up) {
    digitalWrite(DO_HEATING_PIN, LOW);
  } else {  // if signal is high
    unsigned long current_millis = millis();

    // and enough time has passed since last check
    if (millis() - g_temp_previous_millis > TEMP_CONTROL_INTERVAL_MILLIS) {
      g_temp_previous_millis = millis();

      // Read the thermistor temperature
      float temperature = readThermistorTemperature();

      // TODO: Add PID controller and send PWM to MOSFET
      int pinstate = temperature < HOTEND_TEMP_DEGREES_C ? HIGH : LOW;
      digitalWrite(DO_HEATING_PIN, pinstate);
    }

  }
}
