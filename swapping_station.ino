const int SWAP_STATION = 4;
const int BOOST_ACTIVATE = 3;
const int BATTERY_DIS_CHG = 2;
const int EMERGENCY_STOP = 5;
#define AVG_NUM 8
#define MAINS_SCALE 0.5 // Edit this for actual voltage value
#define MAINS_SENSE A0
#define BATTERY_SENSE A1
#define CURRENT_SENSE A2
#define BAT_VOLTS_SCALE 0.1513671875

float battery;
uint16_t mains, mains_raw;
uint16_t grid;

unsigned long previousMillis = 0;  // Store the last time data was sent
const long interval = 5000;        // Interval (5 second) for sending data

// Calibration coefficients
double ICAL = 6.5;  // Current calibration factor
double current;

// Current Sensing Loop
double offsetI;  // Low-pass filter output for current
double filteredI; // Filtered current reading
double sqI, sumI, Irms; // For RMS calculation of current

void setup() {
  Serial.begin(9600);
  pinMode(SWAP_STATION, OUTPUT);
  digitalWrite(SWAP_STATION, LOW);
    pinMode(BOOST_ACTIVATE, OUTPUT);
  digitalWrite(BOOST_ACTIVATE, LOW);
    pinMode( BATTERY_DIS_CHG, OUTPUT);
  digitalWrite( BATTERY_DIS_CHG, LOW);
}

void loop() {
  unsigned long currentMillis = millis();  // Get the current time

  // Check if 1 second has passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read and calculate values
    current = calIrms(1480);
    battery = read_adc(BATTERY_SENSE) * BAT_VOLTS_SCALE;  // Read battery voltage
    mains_raw = read_adc(MAINS_SENSE);  // Read mains voltage
    grid = mains_raw * MAINS_SCALE;  // Convert to actual mains voltage
    int WATT = grid * current;  // Calculate wattage

    // Send data to Serial Monitor
    Serial.print("AC CURRENT: ");
    Serial.print(current);
    Serial.println(" A");
    Serial.print("BATTERY: ");
    Serial.print(battery);
    Serial.println(" V");
    Serial.print("MAINS: ");
    Serial.print(grid);
    Serial.println(" V");
    Serial.print("WATTAGE: ");
    Serial.print(WATT);
    Serial.println(" W");

    // Check if grid voltage exceeds threshold and control swap station
    if (grid > 170) {
       delay(2000);
      digitalWrite(SWAP_STATION, HIGH);
      Serial.println("SWAP STATION CONNECTED");
      delay(2000);
      digitalWrite( BATTERY_DIS_CHG, HIGH);
      delay(3000);
        digitalWrite(BOOST_ACTIVATE, HIGH);
      
    } else {
      digitalWrite(SWAP_STATION, LOW);
      delay(2000);
      Serial.println("BSS CONNECTED");
      delay(2000);
       digitalWrite( BATTERY_DIS_CHG, LOW);
       delay(3000);
        digitalWrite(BOOST_ACTIVATE, LOW);
    }
  }
}

// Function to read ADC with averaging to reduce noise
int read_adc(int channel) {
  int sum = 0;
  for (int i = 0; i < AVG_NUM; i++) {
    sum += analogRead(channel);
    delayMicroseconds(500);  // Allow ADC to settle
  }
  return sum / AVG_NUM;
}

// Current RMS calculation
double calIrms(unsigned int Number_of_Samples) {
  sumI = 0; // Reset the sum before calculation

  // Perform the RMS calculation over the given number of samples
  for (unsigned int n = 0; n < Number_of_Samples; n++) {
    int sampleI = analogRead(CURRENT_SENSE);  // Read current sensor

    // Low-pass filter to remove DC offset and get the AC signal
    offsetI = (offsetI + (sampleI - offsetI) / 1024);
    filteredI = sampleI - offsetI;

    // Square the filtered current reading and accumulate the sum
    sqI = filteredI * filteredI;
    sumI += sqI;
  }

  // Calculate the RMS current using the calibration factor
  double I_RATIO = ICAL * ((5000 / 1000.0) / 1024);  // Calibration for current
  Irms = I_RATIO * sqrt(sumI / Number_of_Samples);  // Calculate RMS value

  return Irms;
}
