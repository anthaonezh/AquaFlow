/*
 * AquaFlow - Liquid Flow Sensor Integration
 * Reads data from a liquid flow sensor (YF-S201) and calculates instantaneous flow rate.
 * Sensor output: pulse frequency proportional to flow rate.
 * Calibration: 7.5 pulses per second per liter per minute (YF-S201 typical).
 */

// Pin definitions
const int FLOW_SENSOR_PIN = 2;   // Interrupt capable pin (D2 on Arduino Uno)
const float CALIBRATION_FACTOR = 7.5; // Pulses per second per L/min (adjust based on sensor)

// Variables for pulse counting
volatile int pulseCount = 0;
unsigned long lastTime = 0;
float flowRate = 0.0;        // Liters per minute
float totalVolume = 0.0;     // Total liters passed

// Interrupt service routine (ISR) for counting pulses
void pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  // Attach interrupt on falling edge (sensor pulses are active low)
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);
  lastTime = millis();
  Serial.println("AquaFlow Flow Sensor Initialized");
}

void loop() {
  // Calculate flow rate every second
  if (millis() - lastTime >= 1000) {
    // Disable interrupts while reading pulse count to avoid race condition
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
    float pulsesPerSecond = pulseCount; // pulseCount accumulated over 1 second
    pulseCount = 0; // Reset for next interval
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);

    // Calculate flow rate in liters per minute
    flowRate = pulsesPerSecond / CALIBRATION_FACTOR;
    
    // Calculate volume in liters (flowRate L/min * time interval in minutes)
    float intervalMinutes = 1.0 / 60.0; // 1 second = 1/60 minutes
    float volumeThisInterval = flowRate * intervalMinutes;
    totalVolume += volumeThisInterval;

    // Output to serial monitor
    Serial.print("Flow Rate: ");
    Serial.print(flowRate);
    Serial.print(" L/min\tTotal Volume: ");
    Serial.print(totalVolume);
    Serial.println(" L");

    lastTime = millis();
  }
}