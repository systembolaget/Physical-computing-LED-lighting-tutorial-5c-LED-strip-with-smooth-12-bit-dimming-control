// Tutorial 5c. LED strip with smooth 12-bit dimming control

// Main parts: Adafruit Metro Mini, potentiometer, capacitor, LED strip,
// Infineon Technologies IRFZ44N MOSFET, RECOM R-78B12-1.0 voltage regulator

// Variables that remain constant
const byte pinPotentiometer = A0; // Analog input pin from potentiometer
const byte pinLDB500LW = 9; // LED driver PWM input pin number
const unsigned int dutyCycleValues[512] = { // Array lookup-table generator https://victornpb.github.io/gamma-table-generator/
  0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   2,
  2,   2,   3,   3,   3,   4,   4,   4,   5,   5,   6,   6,   7,   7,   8,   9,
  9,  10,  11,  11,  12,  13,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,
  23,  24,  25,  26,  27,  28,  29,  30,  32,  33,  34,  35,  37,  38,  40,  41,
  42,  44,  45,  47,  48,  50,  52,  53,  55,  57,  58,  60,  62,  64,  66,  67,
  69,  71,  73,  75,  77,  79,  81,  83,  85,  88,  90,  92,  94,  96,  99, 101,
  103, 106, 108, 111, 113, 116, 118, 121, 123, 126, 129, 131, 134, 137, 140, 142,
  145, 148, 151, 154, 157, 160, 163, 166, 169, 172, 175, 178, 182, 185, 188, 192,
  195, 198, 202, 205, 208, 212, 216, 219, 223, 226, 230, 234, 237, 241, 245, 249,
  252, 256, 260, 264, 268, 272, 276, 280, 284, 289, 293, 297, 301, 305, 310, 314,
  318, 323, 327, 332, 336, 341, 345, 350, 354, 359, 364, 368, 373, 378, 383, 388,
  393, 398, 402, 407, 413, 418, 423, 428, 433, 438, 443, 449, 454, 459, 465, 470,
  475, 481, 486, 492, 498, 503, 509, 514, 520, 526, 532, 537, 543, 549, 555, 561,
  567, 573, 579, 585, 591, 597, 604, 610, 616, 622, 629, 635, 641, 648, 654, 661,
  667, 674, 681, 687, 694, 701, 707, 714, 721, 728, 735, 742, 749, 756, 763, 770,
  777, 784, 791, 798, 806, 813, 820, 827, 835, 842, 850, 857, 865, 872, 880, 888,
  895, 903, 911, 919, 926, 934, 942, 950, 958, 966, 974, 982, 990, 998, 1007, 1015,
  1023, 1031, 1040, 1048, 1056, 1065, 1073, 1082, 1090, 1099, 1108, 1116, 1125, 1134, 1142, 1151,
  1160, 1169, 1178, 1187, 1196, 1205, 1214, 1223, 1232, 1241, 1251, 1260, 1269, 1278, 1288, 1297,
  1307, 1316, 1326, 1335, 1345, 1354, 1364, 1374, 1383, 1393, 1403, 1413, 1423, 1433, 1443, 1453,
  1463, 1473, 1483, 1493, 1503, 1513, 1524, 1534, 1544, 1555, 1565, 1576, 1586, 1597, 1607, 1618,
  1628, 1639, 1650, 1661, 1671, 1682, 1693, 1704, 1715, 1726, 1737, 1748, 1759, 1770, 1781, 1793,
  1804, 1815, 1827, 1838, 1849, 1861, 1872, 1884, 1895, 1907, 1919, 1930, 1942, 1954, 1966, 1977,
  1989, 2001, 2013, 2025, 2037, 2049, 2061, 2073, 2086, 2098, 2110, 2122, 2135, 2147, 2160, 2172,
  2185, 2197, 2210, 2222, 2235, 2248, 2260, 2273, 2286, 2299, 2312, 2325, 2338, 2351, 2364, 2377,
  2390, 2403, 2416, 2429, 2443, 2456, 2469, 2483, 2496, 2510, 2523, 2537, 2550, 2564, 2578, 2591,
  2605, 2619, 2633, 2647, 2661, 2675, 2689, 2703, 2717, 2731, 2745, 2759, 2773, 2788, 2802, 2816,
  2831, 2845, 2860, 2874, 2889, 2903, 2918, 2933, 2947, 2962, 2977, 2992, 3007, 3022, 3036, 3051,
  3067, 3082, 3097, 3112, 3127, 3142, 3158, 3173, 3188, 3204, 3219, 3235, 3250, 3266, 3281, 3297,
  3313, 3328, 3344, 3360, 3376, 3392, 3408, 3424, 3440, 3456, 3472, 3488, 3504, 3520, 3537, 3553,
  3569, 3586, 3602, 3618, 3635, 3651, 3668, 3685, 3701, 3718, 3735, 3752, 3768, 3785, 3802, 3819,
  3836, 3853, 3870, 3887, 3905, 3922, 3939, 3956, 3974, 3991, 4008, 4026, 4043, 4061, 4078, 4096,
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
  fadeUp();
  delay(2000);
  fadeDown();
  delay(2000);
}

// Only for demostrating the visually linear fade corrected for human brightness perception
void fadeUp()
{
  for (int i = 0; i <= 511; i++)
  {
    OCR1A = dutyCycleValues[i];
    delay(15);
  }
}

void fadeDown()
{
  for (int i = 511; i >= 0; i--)
  {
    OCR1A = dutyCycleValues[i];
    delay(15);
  }
}
