// Tutorial 5c. LED strip with smooth 12-bit dimming control

// Main parts: Adafruit Metro Mini, potentiometer, capacitor, LED strip,
// Infineon Technologies IRFZ44N MOSFET, RECOM R-78B12-1.0 voltage regulator

// Variables that remain constant
const byte pinPotentiometer = A0; // Analog input pin from potentiometer
const byte pinLDB500LW = 9; // LED driver PWM input pin number
const unsigned int dutyCycleValues[512] = { // Array lookup-table generator https://victornpb.github.io/gamma-table-generator/
  0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   2,
  2,   2,   3,   3,   3,   4,   4,   4,   5,   5,   6,   6,   7,   7,   8,   9,
  9,  10,  11,  11,  12,  13,  13,  14,  15,  16,  17,  18,  19,  20,  21,  21,
  23,  24,  25,  26,  27,  28,  29,  30,  32,  33,  34,  35,  37,  38,  40,  41,
  42,  44,  45,  47,  48,  50,  52,  53,  55,  57,  58,  60,  62,  64,  66,  67,
  69,  71,  73,  75,  77,  79,  81,  83,  85,  88,  90,  92,  94,  96,  99, 101,
  103, 106, 108, 111, 113, 116, 118, 121, 123, 126, 129, 131, 134, 137, 140, 142,
  145, 148, 151, 154, 157, 160, 163, 166, 169, 172, 175, 178, 182, 185, 188, 191,
  195, 198, 202, 205, 208, 212, 215, 219, 223, 226, 230, 234, 237, 241, 245, 249,
  252, 256, 260, 264, 268, 272, 276, 280, 284, 288, 293, 297, 301, 305, 310, 314,
  318, 323, 327, 332, 336, 341, 345, 350, 354, 359, 364, 368, 373, 378, 383, 388,
  393, 397, 402, 407, 412, 417, 423, 428, 433, 438, 443, 449, 454, 459, 465, 470,
  475, 481, 486, 492, 497, 503, 509, 514, 520, 526, 531, 537, 543, 549, 555, 561,
  567, 573, 579, 585, 591, 597, 603, 610, 616, 622, 629, 635, 641, 648, 654, 661,
  667, 674, 680, 687, 694, 700, 707, 714, 721, 728, 735, 741, 748, 755, 762, 769,
  777, 784, 791, 798, 805, 813, 820, 827, 835, 842, 850, 857, 865, 872, 880, 887,
  895, 903, 911, 918, 926, 934, 942, 950, 958, 966, 974, 982, 990, 998, 1006, 1015,
  1023, 1031, 1039, 1048, 1056, 1065, 1073, 1082, 1090, 1099, 1107, 1116, 1125, 1133, 1142, 1151,
  1160, 1169, 1178, 1187, 1196, 1205, 1214, 1223, 1232, 1241, 1250, 1260, 1269, 1278, 1287, 1297,
  1306, 1316, 1325, 1335, 1344, 1354, 1364, 1373, 1383, 1393, 1403, 1413, 1422, 1432, 1442, 1452,
  1462, 1472, 1483, 1493, 1503, 1513, 1523, 1534, 1544, 1554, 1565, 1575, 1586, 1596, 1607, 1617,
  1628, 1639, 1649, 1660, 1671, 1682, 1693, 1704, 1715, 1726, 1737, 1748, 1759, 1770, 1781, 1792,
  1804, 1815, 1826, 1838, 1849, 1860, 1872, 1883, 1895, 1907, 1918, 1930, 1942, 1953, 1965, 1977,
  1989, 2001, 2013, 2025, 2037, 2049, 2061, 2073, 2085, 2097, 2110, 2122, 2134, 2147, 2159, 2172,
  2184, 2197, 2209, 2222, 2234, 2247, 2260, 2273, 2285, 2298, 2311, 2324, 2337, 2350, 2363, 2376,
  2389, 2402, 2416, 2429, 2442, 2455, 2469, 2482, 2496, 2509, 2523, 2536, 2550, 2563, 2577, 2591,
  2605, 2618, 2632, 2646, 2660, 2674, 2688, 2702, 2716, 2730, 2744, 2758, 2773, 2787, 2801, 2816,
  2830, 2844, 2859, 2873, 2888, 2903, 2917, 2932, 2947, 2961, 2976, 2991, 3006, 3021, 3036, 3051,
  3066, 3081, 3096, 3111, 3126, 3142, 3157, 3172, 3188, 3203, 3218, 3234, 3249, 3265, 3281, 3296,
  3312, 3328, 3343, 3359, 3375, 3391, 3407, 3423, 3439, 3455, 3471, 3487, 3503, 3519, 3536, 3552,
  3568, 3585, 3601, 3618, 3634, 3651, 3667, 3684, 3700, 3717, 3734, 3751, 3767, 3784, 3801, 3818,
  3835, 3852, 3869, 3886, 3904, 3921, 3938, 3955, 3973, 3990, 4007, 4025, 4042, 4060, 4077, 4095,
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
  // over 1kHz means that flicker is much less perceptible. TOP is the value
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
  // the result (0 - 1023) by 2 to get a range of 0 - 511 for the array,
  // the lookup-table with pre-computed logarithmic dimming values
  int readingPotentiometer = analogRead(pinPotentiometer) / 2;

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
