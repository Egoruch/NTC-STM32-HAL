#include "ntc.h"

const temperature_table_entry_type termo_table[] PROGMEM = {
		4033, 4031, 4028, 4026, 4024, 4021, 4018, 4016,
		4013, 4010, 4007, 4004, 4001, 3998, 3995, 3991,
		3988, 3985, 3981, 3977, 3973, 3969, 3965, 3961,
		3957, 3953, 3948, 3944, 3939, 3934, 3929, 3924,
		3919, 3914, 3909, 3903, 3897, 3892, 3886, 3880,
		3873, 3867, 3860, 3854, 3847, 3840, 3833, 3826,
		3818, 3811, 3803, 3795, 3787, 3779, 3771, 3762,
		3754, 3745, 3736, 3727, 3717, 3708, 3698, 3688,
		3678, 3668, 3658, 3647, 3636, 3625, 3614, 3603,
		3591, 3580, 3568, 3556, 3544, 3531, 3519, 3506,
		3493, 3480, 3467, 3453, 3439, 3426, 3411, 3397,
		3383, 3368, 3354, 3339, 3324, 3308, 3293, 3277,
		3261, 3246, 3229, 3213, 3197, 3180, 3163, 3147,
		3130, 3112, 3095, 3078, 3060, 3042, 3024, 3006,
		2988, 2970, 2952, 2933, 2915, 2896, 2877, 2858,
		2839, 2820, 2801, 2782, 2763, 2743, 2724, 2704,
		2685, 2665, 2645, 2625, 2606, 2586, 2566, 2546,
		2526, 2506, 2486, 2466, 2446, 2426, 2406, 2386,
		2365, 2345, 2325, 2305, 2285, 2265, 2245, 2225,
		2206, 2186, 2166, 2146, 2126, 2107, 2087, 2067,
		2048, 2029, 2009, 1990, 1971, 1952, 1932, 1914,
		1895, 1876, 1857, 1838, 1820, 1802, 1783, 1765,
		1747, 1729, 1711, 1693, 1675, 1658, 1640, 1623,
		1606, 1589, 1572, 1555, 1538, 1522, 1505, 1489,
		1473, 1457, 1441, 1425, 1409, 1394, 1378, 1363,
		1348, 1333, 1318, 1303, 1288, 1274, 1259, 1245,
		1231, 1217, 1203, 1190, 1176, 1163, 1149, 1136,
		1123, 1110, 1098, 1085, 1073, 1060, 1048, 1036,
		1024, 1012, 1000, 989, 977, 966, 955, 944,
		933, 922, 911, 900, 890, 880, 869, 859,
		849, 839, 830, 820, 810, 801, 792, 782,
		773, 764, 755, 747, 738, 729, 721, 712,
		704, 696, 688, 680, 672, 664, 657, 649,
		642, 634, 627, 620, 612, 605, 598, 592,
		585, 578, 571, 565, 558, 552, 546, 539,
		533, 527, 521, 515, 509, 504, 498, 492,
		487, 481, 476, 471, 465, 460, 455, 450,
		445, 440, 435, 430, 425, 421, 416, 411,
		407, 402, 398, 393, 389, 385, 381, 376,
		372, 368, 364, 360, 356, 352, 349, 345,
		341, 338, 334, 330, 327, 323, 320, 316,
		313, 310, 306, 303, 300, 297, 294, 291,
		288, 285, 282, 279, 276, 273, 270, 267,
		264, 262, 259, 256, 254, 251, 249, 246,
		244, 241, 239, 236, 234, 231, 229, 227,
		225
};


int16_t calc_temperature(temperature_table_entry_type adcsum) {
  temperature_table_index_type l = 0;
  temperature_table_index_type r = (sizeof(termo_table) / sizeof(termo_table[0])) - 1;
  temperature_table_entry_type thigh = TEMPERATURE_TABLE_READ(r);
  
  // Проверка выхода за пределы и граничных значений
  if (adcsum <= thigh) {
    #ifdef TEMPERATURE_OVER
      if (adcsum < thigh) 
        return TEMPERATURE_OVER;
    #endif
    return TEMPERATURE_TABLE_STEP * r + TEMPERATURE_TABLE_START;
  }
  temperature_table_entry_type tlow = TEMPERATURE_TABLE_READ(0);
  if (adcsum >= tlow) {
    #ifdef TEMPERATURE_UNDER
      if (adcsum > tlow)
        return TEMPERATURE_UNDER;
    #endif
    return TEMPERATURE_TABLE_START;
  }

  // Двоичный поиск по таблице
  while ((r - l) > 1) {
    temperature_table_index_type m = (l + r) >> 1;
    temperature_table_entry_type mid = TEMPERATURE_TABLE_READ(m);
    if (adcsum > mid) {
      r = m;
    } else {
      l = m;
    }
  }
  temperature_table_entry_type vl = TEMPERATURE_TABLE_READ(l);
  if (adcsum >= vl) {
    return l * TEMPERATURE_TABLE_STEP + TEMPERATURE_TABLE_START;
  }
  temperature_table_entry_type vr = TEMPERATURE_TABLE_READ(r);
  temperature_table_entry_type vd = vl - vr;
  int16_t res = TEMPERATURE_TABLE_START + r * TEMPERATURE_TABLE_STEP; 
  if (vd) {
    // Линейная интерполяция
    res -= ((TEMPERATURE_TABLE_STEP * (int32_t)(adcsum - vr) + (vd >> 1)) / vd);
  }
  return res;
}
