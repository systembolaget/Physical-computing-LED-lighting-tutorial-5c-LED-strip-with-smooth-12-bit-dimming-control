// Tutorial 5c. LED strip with smooth 12-bit dimming control

// Main parts: Adafruit Metro Mini, potentiometer, capacitor, LED strip,
// Infineon Technologies IRFZ44N MOSFET, RECOM R-78B12-1.0 voltage regulator

// Variables that remain constant
const byte pinPotentiometer = A0; // Analog input pin from potentiometer
const byte pinLDB500LW = 9; // LED driver PWM input pin number
const unsigned int dutyCycleValues[512] = { // Array lookup-table generator https://victornpb.github.io/gamma-table-generator/
  0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   2,   2,   2,   2,
  3,   3,   4,   4,   5,   5,   6,   6,   7,   7,   8,   9,   9,  10,  11,  11,
  12,  13,  14,  15,  16,  17,  17,  18,  19,  20,  22,  23,  24,  25,  26,  27,
  29,  30,  31,  32,  34,  35,  37,  38,  39,  41,  42,  44,  46,  47,  49,  50,
  52,  54,  56,  57,  59,  61,  63,  65,  67,  69,  71,  73,  75,  77,  79,  81,
  83,  86,  88,  90,  92,  95,  97,  99, 102, 104, 107, 109, 112, 114, 117, 120,
  122, 125, 128, 130, 133, 136, 139, 142, 145, 148, 151, 154, 157, 160, 163, 166,
  169, 172, 175, 179, 182, 185, 189, 192, 195, 199, 202, 206, 209, 213, 216, 220,
  224, 227, 231, 235, 239, 242, 246, 250, 254, 258, 262, 266, 270, 274, 278, 282,
  287, 291, 295, 299, 303, 308, 312, 317, 321, 325, 330, 334, 339, 344, 348, 353,
  357, 362, 367, 372, 376, 381, 386, 391, 396, 401, 406, 411, 416, 421, 426, 431,
  437, 442, 447, 452, 458, 463, 469, 474, 479, 485, 490, 496, 502, 507, 513, 518,
  524, 530, 536, 542, 547, 553, 559, 565, 571, 577, 583, 589, 595, 602, 608, 614,
  620, 626, 633, 639, 645, 652, 658, 665, 671, 678, 684, 691, 698, 704, 711, 718,
  725, 731, 738, 745, 752, 759, 766, 773, 780, 787, 794, 801, 809, 816, 823, 830,
  838, 845, 852, 860, 867, 875, 882, 890, 897, 905, 913, 920, 928, 936, 943, 951,
  959, 967, 975, 983, 991, 999, 1007, 1015, 1023, 1031, 1039, 1048, 1056, 1064, 1073, 1081,
  1089, 1098, 1106, 1115, 1123, 1132, 1140, 1149, 1158, 1166, 1175, 1184, 1193, 1202, 1210, 1219,
  1228, 1237, 1246, 1255, 1264, 1273, 1283, 1292, 1301, 1310, 1320, 1329, 1338, 1348, 1357, 1366,
  1376, 1385, 1395, 1405, 1414, 1424, 1434, 1443, 1453, 1463, 1473, 1483, 1492, 1502, 1512, 1522,
  1532, 1543, 1553, 1563, 1573, 1583, 1593, 1604, 1614, 1624, 1635, 1645, 1656, 1666, 1677, 1687,
  1698, 1708, 1719, 1730, 1741, 1751, 1762, 1773, 1784, 1795, 1806, 1817, 1828, 1839, 1850, 1861,
  1872, 1883, 1894, 1906, 1917, 1928, 1940, 1951, 1962, 1974, 1985, 1997, 2009, 2020, 2032, 2043,
  2055, 2067, 2079, 2091, 2102, 2114, 2126, 2138, 2150, 2162, 2174, 2186, 2198, 2211, 2223, 2235,
  2247, 2260, 2272, 2284, 2297, 2309, 2322, 2334, 2347, 2359, 2372, 2385, 2397, 2410, 2423, 2436,
  2448, 2461, 2474, 2487, 2500, 2513, 2526, 2539, 2552, 2566, 2579, 2592, 2605, 2619, 2632, 2645,
  2659, 2672, 2686, 2699, 2713, 2726, 2740, 2753, 2767, 2781, 2795, 2808, 2822, 2836, 2850, 2864,
  2878, 2892, 2906, 2920, 2934, 2948, 2963, 2977, 2991, 3005, 3020, 3034, 3048, 3063, 3077, 3092,
  3106, 3121, 3136, 3150, 3165, 3180, 3194, 3209, 3224, 3239, 3254, 3269, 3284, 3299, 3314, 3329,
  3344, 3359, 3374, 3389, 3405, 3420, 3435, 3451, 3466, 3482, 3497, 3513, 3528, 3544, 3559, 3575,
  3591, 3606, 3622, 3638, 3654, 3670, 3686, 3702, 3718, 3734, 3750, 3766, 3782, 3798, 3814, 3830,
  3847, 3863, 3879, 3896, 3912, 3929, 3945, 3962, 3978, 3995, 4011, 4028, 4045, 4061, 4078, 4095,
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
