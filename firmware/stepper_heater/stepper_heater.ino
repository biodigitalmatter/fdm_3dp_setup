#include <Arduino.h>
// Install AccelStepper: Tools > Manage libraries > Search for and install "AccelStepper"
#include <AccelStepper.h>

#define DEBUG 0

// SETTINGS
const int HOTEND_TEMP_DEGREES_C = 210;
const int EXTRUDER_RPM = 60;

// PINS

// Comm pins
const int DI_ROBOT_HEAT_UP_PIN = 2;
const int DI_ROBOT_RUN_BACKWARDS_PIN = 4;
const int DI_ROBOT_RUN_FORWARD_PIN = 5;

// Stepper motor pins
const int DO_NC_ENABLE_PIN = 7;  // ENA - Enable when 0, disable when 1
const int DO_DIR_PIN = 12;        // DIR - Direction
const int DO_STEP_PIN = 11;       // STP/PUL - Step, Pulse

// hotend pins
const int DO_HEATING_PIN = 9;
const int DO_RELAY_PWR = 10;
const int AI_THERMISTOR_PIN = A7;

// LED
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
// beta-value (at 25 degrees): 4267K
// resistance (Ohm) | degrees (C)
// 353700           |   0
//   5556           | 100
//   439.3          | 200
const float SH_A = 0.8097317731e-03, SH_B = 2.11635527e-04, SH_C = 0.7066084133e-07;

unsigned long g_temp_previous_millis = 0;
// Hemera motor
const float STEP_ANGLE_DEGREES = 1.8;
const float GEAR_RATIO = 1 / 3.32;
const int MICROSTEP_MULTIPLIER = 8;
const bool STEPPER_INVERT_DIR = true;

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

  float referenceVoltage = getReferenceVoltage();

  // Convert the analog value to voltage (using the internal reference voltage)
  float Vout = (analogValue * referenceVoltage) / 1023.0;

  // Calculate the thermistor resistance
  float thermistor_ohm = THERMISTOR_SETUP_FIXED_R1_OHM * (referenceVoltage / Vout - 1.0);

  // Convert resistance to temperature using the Steinhart-Hart equation coefficients (c1, c2, c3)
  float T = 1.0 / (SH_A + SH_B * log(thermistor_ohm) + SH_C * pow(log(thermistor_ohm), 3));

  // Convert temperature from Kelvin to Celsius
  float Tc = T - 273.15;

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

  //g_stepper.setEnablePin(O_PIN_ENABLE);
  g_stepper.setPinsInverted(/* directionInvert */ STEPPER_INVERT_DIR);
  g_stepper.setMaxSpeed(MAX_STEPS_PER_SEC);

  // Add pinMode for enable pin and set it to LOW to enable the stepper driver
  pinMode(DO_NC_ENABLE_PIN, OUTPUT);
  digitalWrite(DO_NC_ENABLE_PIN, LOW);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);


  // setup robot input pin
  pinMode(DI_ROBOT_RUN_BACKWARDS_PIN, INPUT_PULLUP);
  pinMode(DI_ROBOT_RUN_FORWARD_PIN, INPUT_PULLUP);
  pinMode(DI_ROBOT_HEAT_UP_PIN, INPUT_PULLUP);

  // hotend pins
  pinMode(DO_HEATING_PIN, OUTPUT);
  digitalWrite(DO_HEATING_PIN, LOW);

  pinMode(DO_RELAY_PWR, OUTPUT);
  digitalWrite(DO_RELAY_PWR, HIGH);


  pinMode(AI_THERMISTOR_PIN, INPUT);

  // led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

#if DEBUG == 1
  Serial.begin(9600);
#endif
}

void loop() {

  bool forwards = digitalRead(DI_ROBOT_RUN_FORWARD_PIN) == HIGH;
  bool backwards = digitalRead(DI_ROBOT_RUN_BACKWARDS_PIN) == HIGH;

  int signed_steps_per_sec = 0;  // Defaults to 0

  if (forwards && !backwards) {
    signed_steps_per_sec = 1 * STEPS_PER_SEC;
    digitalWrite(LED_BUILTIN, HIGH);
  } else if (!forwards && backwards) {
    signed_steps_per_sec = -1 * STEPS_PER_SEC;
    if (millis() - g_led_previous_millis >= LED_INTERVAL) {
      g_led_previous_millis = millis();
      g_LED_blink_state = g_LED_blink_state == HIGH ? LOW : HIGH;
      digitalWrite(LED_BUILTIN, g_LED_blink_state);
    }
  } else { // both false or both true
    digitalWrite(LED_BUILTIN, LOW);
    // don't touch speed
  }

  g_stepper.setSpeed(signed_steps_per_sec);

  g_stepper.runSpeed();

  //bool heat_up = digitalRead(DI_ROBOT_HEAT_UP_PIN) == HIGH;
  bool heat_up = true;
  
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

#if DEBUG == 1
      Serial.print("T:");
      Serial.println(temperature);
#endif

      // TODO: Add PID controller and send PWM to MOSFET
      int pinstate = temperature < HOTEND_TEMP_DEGREES_C ? HIGH : LOW;
      digitalWrite(DO_HEATING_PIN, pinstate);
    }
  }
}
