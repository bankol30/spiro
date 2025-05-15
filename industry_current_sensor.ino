
const int SWAP_STATION =4;
const int BOOST_ACTIVATE=3;
const int BATTERY_DIS_CHG=2;
const int EMERGENCY_STOP=5;
#define  AVG_NUM 8
#define MAINS_SCALE 0.5000 //edit this for actual voltage value

#define MAINS_SENSE A0
#define BATTERY_SENSE A1
#define CURRENT_SENSE A2
#define BAT_VOLTS_SCALE 0.1513671875
float battery;
int mains, mains_raw;
float grid;

int read_adc(int channel) {
    int sum = 0;
    for (int i = 0; i < AVG_NUM; i++) {
        sum += analogRead(channel);
        delayMicroseconds(5000);
    }
    return sum / AVG_NUM;
}

////////////////////////////////////////////CURRENT SENSING LOOP
#define ADC_BITS    10
#define ADC_COUNTS  (1<<ADC_BITS)
  unsigned int inPinV;
    unsigned int inPinI;
    //Calibration coefficients
    //These need to be set in order to obtain accurate results
    double VCAL;
    double ICAL=6.5; //calibration factor
    double PHASECAL;
double current;
    //--------------------------------------------------------------------------------------
    // Variable declaration for emon_calc procedure
    //--------------------------------------------------------------------------------------
    int sampleV;                        //sample_ holds the raw analog read value
    int sampleI;

    double lastFilteredV,filteredV;          //Filtered_ is the raw analog value minus the DC offset
    double filteredI;
    double offsetV;                          //Low-pass filter output
    double offsetI;                          //Low-pass filter output
    double phaseShiftedV, Irms;                             //Holds the calibrated phase shifted voltage.

    double sqV,sumV,sqI,sumI,instP,sumP;              //sq = squared, sum = Sum, inst = instantaneous
    ////////////////////////////////////////////////END OF CURRENT SENSING LOOP
 
void setup() {
Serial.begin(9600);
}
 
void loop() {
current=calIrms(1480);
battery=read_adc(BATTERY_SENSE) * BAT_VOLTS_SCALE;
mains_raw=read_adc(MAINS_SENSE);
grid=mains_raw * 0.7071067812 * MAINS_SCALE; 
grid = (uint16_t)grid;
Serial.println(current);
Serial.println(battery);
Serial.println(grid);
delay(100);
}

double calIrms(unsigned int Number_of_Samples){
  for (unsigned int n = 0; n < Number_of_Samples; n++)
  {
    sampleI = analogRead(A0);

    // Digital low pass filter extracts the 2.5 V or 1.65 V dc offset,
    //  then subtract this - signal is now centered on 0 counts.
    offsetI = (offsetI + (sampleI-offsetI)/1024);
    filteredI = sampleI - offsetI;

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum
    sumI += sqI;
  }

  double I_RATIO = ICAL *((5000/1000.0) / (1024));
  Irms = I_RATIO * sqrt(sumI / Number_of_Samples);

  //Reset accumulators
  sumI = 0;
  //--------------------------------------------------------------------------------------

  return Irms;
}
