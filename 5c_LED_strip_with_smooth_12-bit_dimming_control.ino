// Tutorial 5c. LED strip with smooth 12-bit dimming control

// Main parts: Adafruit Metro Mini, potentiometer, capacitor, LED strip,
// Infineon Technologies IRFZ44N MOSFET, RECOM R-78B12-1.0 voltage regulator

// Variables that remain constant
const byte pinPotentiometer = A0; // Analog input pin from potentiometer
const byte pinLDB500LW = 9; // LED driver PWM input pin number
const unsigned int dutyCycleValues[256] = { // Array lookup-table http://jared.geek.nz/2013/feb/linear-led-pwm

  0,    2,    4,    5,    7,    9,   11,   12,   14,   16,   18,   20,   21,   23,   25,   27,
  28,   30,   32,   34,   36,   37,   39,   41,   43,   45,   47,   49,   52,   54,   56,   59,
  61,   64,   66,   69,   72,   75,   77,   80,   83,   87,   90,   93,   96,  100,  103,  107,
  111,  115,  118,  122,  126,  131,  135,  139,  144,  148,  153,  157,  162,  167,  172,  177,
  182,  187,  193,  198,  204,  209,  215,  221,  227,  233,  239,  246,  252,  259,  265,  272,
  279,  286,  293,  300,  308,  315,  323,  330,  338,  346,  354,  362,  371,  379,  388,  396,
  405,  414,  423,  432,  442,  451,  461,  470,  480,  490,  501,  511,  521,  532,  543,  553,
  564,  576,  587,  598,  610,  622,  634,  646,  658,  670,  683,  695,  708,  721,  734,  748,
  761,  775,  788,  802,  816,  831,  845,  860,  874,  889,  904,  920,  935,  951,  966,  982,
  999, 1015, 1031, 1048, 1065, 1082, 1099, 1116, 1134, 1152, 1170, 1188, 1206, 1224, 1243, 1262,
  1281, 1300, 1320, 1339, 1359, 1379, 1399, 1420, 1440, 1461, 1482, 1503, 1525, 1546, 1568, 1590,
  1612, 1635, 1657, 1680, 1703, 1726, 1750, 1774, 1797, 1822, 1846, 1870, 1895, 1920, 1945, 1971,
  1996, 2022, 2048, 2074, 2101, 2128, 2155, 2182, 2209, 2237, 2265, 2293, 2321, 2350, 2378, 2407,
  2437, 2466, 2496, 2526, 2556, 2587, 2617, 2648, 2679, 2711, 2743, 2774, 2807, 2839, 2872, 2905,
  2938, 2971, 3005, 3039, 3073, 3107, 3142, 3177, 3212, 3248, 3283, 3319, 3356, 3392, 3429, 3466,
  3503, 3541, 3578, 3617, 3655, 3694, 3732, 3772, 3811, 3851, 3891, 3931, 3972, 4012, 4054, 4095
};

void setup()
{
  // Initialise Metro Mini's digital pin specified above
  pinMode(pinLDB500LW, OUTPUT);

  // Timer 1 Control Registers A and B, set first

  // COM1A1 bit set to 1 means non-inverted PWM for the Output Compare
  // Register A (pin 9) is selected. WGM11, WGM13 and WGM12 bits set to
  // 1 means mode 14 Fast PWM is enabled. CS10 bit set to 1 means no
  // pre-scaling (16MHz / 1 = 16MHz) is used
  TCCR1A = (1 << COM1A1) | (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);

  // Timer 1 Input Capture Register, set second

  // ICR1 holds the TOP value in mode 14 Fast PWM. 0 - 4095 = 4096 values,
  // which means 12-bit PWM resolution at 3906Hz (16GHz / 4096). A frequency
  // over 2kHz also means that dimming is flicker-free. TOP is the value
  // the timer counts up to before it starts again at 0
  ICR1 = 4095; // Not 4096, because ICR1 starts at 0!

  // Timer 1 Output Compare Register, set third

  // OCR1A at 0 means the PWM output (and the LED) is off at the start
  OCR1A = 0;
}

void loop()
{
  // The potentiometer is hardware smoothed with a 10uF capacitor; no code
  // is necessary. Read the voltage from the potentiometer pin and divide
  // the result (0 - 1023) by 4 to get a range of 0 - 255 for the array,
  // the lookup-table with pre-computed logarithmic dimming values
  int readingPotentiometer = analogRead(pinPotentiometer) / 4;

  // Fetch the matching lookup-table value and assign it to the Timer 1
  // Output Compare Register to set the LED driver's duty cycle
  OCR1A = dutyCycleValues[readingPotentiometer];

  // Only for demostrating the visually linear fade corrected for human brightness perception
  // response
  /*fadeUp();
    delay(1000);
    fadeDown();
    delay(1000);*/
}

// Only for demostrating the visually linear fade corrected for human eye
// response
/*void fadeUp()
{
  for (int i = 0; i <= 255; i++)
  {
    OCR1A = dutyCycleValues[i];
    delay(25);
  }
}

void fadeDown()
{
  for (int i = 255; i >= 0; i--)
  {
    OCR1A = dutyCycleValues[i];
    delay(25);
  }
}*/
