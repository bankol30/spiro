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
const long interval = 5000;        // Interval (5 seconds) for sending data

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
}

void loop() {
  unsigned long currentMillis = millis();  // Get the current time

  // Check if the specified interval has passed
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Read and calculate values
    current = calIrms(1480);  // Sample current 1480 times
    battery = read_adc_samples(BATTERY_SENSE, 1480) * BAT_VOLTS_SCALE;  // Sample battery voltage 1480 times
    mains_raw = read_adc_samples(MAINS_SENSE, 1480);  // Sample mains voltage 1480 times
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
    if (grid > 120) {
      digitalWrite(SWAP_STATION, HIGH);
      Serial.println("SWAP STATION CONNECTED");
    } else {
      digitalWrite(SWAP_STATION, LOW);
      Serial.println("BSS CONNECTED");
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

// Function to read ADC and sample multiple times (1480 readings in this case)
int read_adc_samples(int channel, int num_samples) {
  long sum = 0;
  for (int i = 0; i < num_samples; i++) {
    sum += analogRead(channel);
    delayMicroseconds(500);  // Allow ADC to settle between readings
  }
  return sum / num_samples;  // Return the average value
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
