#!/usr/bin/python

import struct

# 0 for rough sawtooth, 1 for sawtooth, 2 for sine, 3 for square
g_wave_type = 3

# tone frequency
g_base_freq = 440
g_curr_freq = g_base_freq
g_freq_ramp = 0
g_freq_slur = 0
g_freq_ramp_cnt = 0

# base frequency generation
g_phase = 0

# envelope
g_env_stage = 0
g_env_time = 0
g_env_lengths = [0,0]

# low-pass filter
g_lpf_resonance = 0
g_lpf_base_freq = 0
g_lpf_curr_freq = 0
g_lpf_ramp = 0
g_lpf_ramp_cnt = 0
g_lpf_prev = 0
g_lpf_prev_delta = 0

# vibrato
g_vib_phase = 0
g_vib_strength = 0
g_vib_speed = 0
g_vib_fix_accum = 0

g_interrupt_cnt = 0
g_trem_status = 0
g_dist_status = 0

phase_to_delta = [1,-1,-1,1]

mysin_table = [
   0,  3,  6,  9,  12,  15,  18,  21,  
   25,  28,  31,  34,  37,  40,  43,  47,  
   50,  53,  56,  59,  62,  65,  69,  72,  
   75,  78,  81,  84,  87,  90,  94,  97,  
   100,  103,  106,  109,  112,  115,  119,  122,  
   125,  128,  131,  134,  137,  140,  144,  147,  
   150,  153,  156,  159,  162,  165,  168,  171,  
   175,  178,  181,  184,  187,  190,  193,  196,  
   199,  202,  205,  209,  212,  215,  218,  221,  
   224,  227,  230,  233,  236,  239,  242,  245,  
   248,  251,  254,  257,  260,  264,  267,  270,  
   273,  276,  279,  282,  285,  288,  291,  294,  
   297,  300,  303,  306,  309,  312,  315,  318,  
   321,  324,  327,  330,  333,  336,  339,  342,  
   344,  347,  350,  353,  356,  359,  362,  365,  
   368,  371,  374,  377,  380,  383,  386,  388,  
   391,  394,  397,  400,  403,  406,  409,  412,  
   414,  417,  420,  423,  426,  429,  432,  434,  
   437,  440,  443,  446,  449,  451,  454,  457,  
   460,  463,  466,  468,  471,  474,  477,  479,  
   482,  485,  488,  491,  493,  496,  499,  501,  
   504,  507,  510,  512,  515,  518,  521,  523,  
   526,  529,  531,  534,  537,  539,  542,  545,  
   547,  550,  553,  555,  558,  561,  563,  566,  
   568,  571,  574,  576,  579,  581,  584,  587,  
   589,  592,  594,  597,  599,  602,  604,  607,  
   609,  612,  615,  617,  620,  622,  625,  627,  
   629,  632,  634,  637,  639,  642,  644,  647,  
   649,  652,  654,  656,  659,  661,  664,  666,  
   668,  671,  673,  675,  678,  680,  683,  685,  
   687,  690,  692,  694,  696,  699,  701,  703,  
   706,  708,  710,  712,  715,  717,  719,  721,  
   724,  726,  728,  730,  732,  735,  737,  739,  
   741,  743,  745,  748,  750,  752,  754,  756,  
   758,  760,  762,  765,  767,  769,  771,  773,  
   775,  777,  779,  781,  783,  785,  787,  789,  
   791,  793,  795,  797,  799,  801,  803,  805,  
   807,  809,  811,  813,  814,  816,  818,  820,  
   822,  824,  826,  828,  829,  831,  833,  835,  
   837,  839,  840,  842,  844,  846,  847,  849,  
   851,  853,  854,  856,  858,  860,  861,  863,  
   865,  866,  868,  870,  871,  873,  875,  876,  
   878,  879,  881,  883,  884,  886,  887,  889,  
   890,  892,  894,  895,  897,  898,  900,  901,  
   903,  904,  906,  907,  908,  910,  911,  913,  
   914,  916,  917,  918,  920,  921,  922,  924,  
   925,  927,  928,  929,  930,  932,  933,  934,  
   936,  937,  938,  939,  941,  942,  943,  944,  
   946,  947,  948,  949,  950,  951,  953,  954,  
   955,  956,  957,  958,  959,  960,  962,  963,  
   964,  965,  966,  967,  968,  969,  970,  971,  
   972,  973,  974,  975,  976,  977,  978,  978,  
   979,  980,  981,  982,  983,  984,  985,  986,  
   986,  987,  988,  989,  990,  990,  991,  992,  
   993,  994,  994,  995,  996,  997,  997,  998,  
   999,  999,  1000,  1001,  1001,  1002,  1003,  1003,  
   1004,  1004,  1005,  1006,  1006,  1007,  1007,  1008,  
   1008,  1009,  1009,  1010,  1010,  1011,  1011,  1012,  
   1012,  1013,  1013,  1014,  1014,  1015,  1015,  1015,  
   1016,  1016,  1017,  1017,  1017,  1018,  1018,  1018,  
   1019,  1019,  1019,  1019,  1020,  1020,  1020,  1020,  
   1021,  1021,  1021,  1021,  1022,  1022,  1022,  1022,  
   1022,  1022,  1023,  1023,  1023,  1023,  1023,  1023,  
   1023,  1023,  1023,  1023,  1023,  1023,  1023,  1023,  
   1023,  1023,  1023,  1023,  1023,  1023,  1023,  1023,  
   1023,  1023,  1023,  1023,  1023,  1023,  1023,  1022,  
   1022,  1022,  1022,  1022,  1022,  1021,  1021,  1021,  
   1021,  1020,  1020,  1020,  1020,  1019,  1019,  1019,  
   1019,  1018,  1018,  1018,  1017,  1017,  1017,  1016,  
   1016,  1015,  1015,  1015,  1014,  1014,  1013,  1013,  
   1012,  1012,  1011,  1011,  1010,  1010,  1009,  1009,  
   1008,  1008,  1007,  1007,  1006,  1006,  1005,  1004,  
   1004,  1003,  1003,  1002,  1001,  1001,  1000,  999,  
   999,  998,  997,  997,  996,  995,  994,  994,  
   993,  992,  991,  990,  990,  989,  988,  987,  
   986,  986,  985,  984,  983,  982,  981,  980,  
   979,  978,  978,  977,  976,  975,  974,  973,  
   972,  971,  970,  969,  968,  967,  966,  965,  
   964,  963,  962,  960,  959,  958,  957,  956,  
   955,  954,  953,  951,  950,  949,  948,  947,  
   946,  944,  943,  942,  941,  939,  938,  937,  
   936,  934,  933,  932,  930,  929,  928,  927,  
   925,  924,  922,  921,  920,  918,  917,  916,  
   914,  913,  911,  910,  908,  907,  906,  904,  
   903,  901,  900,  898,  897,  895,  894,  892,  
   890,  889,  887,  886,  884,  883,  881,  879,  
   878,  876,  875,  873,  871,  870,  868,  866,  
   865,  863,  861,  860,  858,  856,  854,  853,  
   851,  849,  847,  846,  844,  842,  840,  839,  
   837,  835,  833,  831,  829,  828,  826,  824,  
   822,  820,  818,  816,  814,  813,  811,  809,  
   807,  805,  803,  801,  799,  797,  795,  793,  
   791,  789,  787,  785,  783,  781,  779,  777,  
   775,  773,  771,  769,  767,  765,  762,  760,  
   758,  756,  754,  752,  750,  748,  745,  743,  
   741,  739,  737,  735,  732,  730,  728,  726,  
   724,  721,  719,  717,  715,  712,  710,  708,  
   706,  703,  701,  699,  696,  694,  692,  690,  
   687,  685,  683,  680,  678,  675,  673,  671,  
   668,  666,  664,  661,  659,  656,  654,  652,  
   649,  647,  644,  642,  639,  637,  634,  632,  
   629,  627,  625,  622,  620,  617,  615,  612,  
   609,  607,  604,  602,  599,  597,  594,  592,  
   589,  587,  584,  581,  579,  576,  574,  571,  
   568,  566,  563,  561,  558,  555,  553,  550,  
   547,  545,  542,  539,  537,  534,  531,  529,  
   526,  523,  521,  518,  515,  512,  510,  507,  
   504,  501,  499,  496,  493,  491,  488,  485,  
   482,  479,  477,  474,  471,  468,  466,  463,  
   460,  457,  454,  451,  449,  446,  443,  440,  
   437,  434,  432,  429,  426,  423,  420,  417,  
   414,  412,  409,  406,  403,  400,  397,  394,  
   391,  388,  386,  383,  380,  377,  374,  371,  
   368,  365,  362,  359,  356,  353,  350,  347,  
   344,  342,  339,  336,  333,  330,  327,  324,  
   321,  318,  315,  312,  309,  306,  303,  300,  
   297,  294,  291,  288,  285,  282,  279,  276,  
   273,  270,  267,  264,  260,  257,  254,  251,  
   248,  245,  242,  239,  236,  233,  230,  227,  
   224,  221,  218,  215,  212,  209,  205,  202,  
   199,  196,  193,  190,  187,  184,  181,  178,  
   175,  171,  168,  165,  162,  159,  156,  153,  
   150,  147,  144,  140,  137,  134,  131,  128,  
   125,  122,  119,  115,  112,  109,  106,  103,  
   100,  97,  94,  90,  87,  84,  81,  78,  
   75,  72,  69,  65,  62,  59,  56,  53,  
   50,  47,  43,  40,  37,  34,  31,  28,  
   25,  21,  18,  15,  12,  9,  6,  3,  
   0,  -3,  -6,  -9,  -12,  -15,  -18,  -21,  
   -25,  -28,  -31,  -34,  -37,  -40,  -43,  -47,  
   -50,  -53,  -56,  -59,  -62,  -65,  -69,  -72,  
   -75,  -78,  -81,  -84,  -87,  -90,  -94,  -97,  
   -100,  -103,  -106,  -109,  -112,  -115,  -119,  -122,  
   -125,  -128,  -131,  -134,  -137,  -140,  -144,  -147,  
   -150,  -153,  -156,  -159,  -162,  -165,  -168,  -171,  
   -175,  -178,  -181,  -184,  -187,  -190,  -193,  -196,  
   -199,  -202,  -205,  -209,  -212,  -215,  -218,  -221,  
   -224,  -227,  -230,  -233,  -236,  -239,  -242,  -245,  
   -248,  -251,  -254,  -257,  -260,  -264,  -267,  -270,  
   -273,  -276,  -279,  -282,  -285,  -288,  -291,  -294,  
   -297,  -300,  -303,  -306,  -309,  -312,  -315,  -318,  
   -321,  -324,  -327,  -330,  -333,  -336,  -339,  -342,  
   -344,  -347,  -350,  -353,  -356,  -359,  -362,  -365,  
   -368,  -371,  -374,  -377,  -380,  -383,  -386,  -388,  
   -391,  -394,  -397,  -400,  -403,  -406,  -409,  -412,  
   -414,  -417,  -420,  -423,  -426,  -429,  -432,  -434,  
   -437,  -440,  -443,  -446,  -449,  -451,  -454,  -457,  
   -460,  -463,  -466,  -468,  -471,  -474,  -477,  -479,  
   -482,  -485,  -488,  -491,  -493,  -496,  -499,  -501,  
   -504,  -507,  -510,  -512,  -515,  -518,  -521,  -523,  
   -526,  -529,  -531,  -534,  -537,  -539,  -542,  -545,  
   -547,  -550,  -553,  -555,  -558,  -561,  -563,  -566,  
   -568,  -571,  -574,  -576,  -579,  -581,  -584,  -587,  
   -589,  -592,  -594,  -597,  -599,  -602,  -604,  -607,  
   -609,  -612,  -615,  -617,  -620,  -622,  -625,  -627,  
   -629,  -632,  -634,  -637,  -639,  -642,  -644,  -647,  
   -649,  -652,  -654,  -656,  -659,  -661,  -664,  -666,  
   -668,  -671,  -673,  -675,  -678,  -680,  -683,  -685,  
   -687,  -690,  -692,  -694,  -696,  -699,  -701,  -703,  
   -706,  -708,  -710,  -712,  -715,  -717,  -719,  -721,  
   -724,  -726,  -728,  -730,  -732,  -735,  -737,  -739,  
   -741,  -743,  -745,  -748,  -750,  -752,  -754,  -756,  
   -758,  -760,  -762,  -765,  -767,  -769,  -771,  -773,  
   -775,  -777,  -779,  -781,  -783,  -785,  -787,  -789,  
   -791,  -793,  -795,  -797,  -799,  -801,  -803,  -805,  
   -807,  -809,  -811,  -813,  -814,  -816,  -818,  -820,  
   -822,  -824,  -826,  -828,  -829,  -831,  -833,  -835,  
   -837,  -839,  -840,  -842,  -844,  -846,  -847,  -849,  
   -851,  -853,  -854,  -856,  -858,  -860,  -861,  -863,  
   -865,  -866,  -868,  -870,  -871,  -873,  -875,  -876,  
   -878,  -879,  -881,  -883,  -884,  -886,  -887,  -889,  
   -890,  -892,  -894,  -895,  -897,  -898,  -900,  -901,  
   -903,  -904,  -906,  -907,  -908,  -910,  -911,  -913,  
   -914,  -916,  -917,  -918,  -920,  -921,  -922,  -924,  
   -925,  -927,  -928,  -929,  -930,  -932,  -933,  -934,  
   -936,  -937,  -938,  -939,  -941,  -942,  -943,  -944,  
   -946,  -947,  -948,  -949,  -950,  -951,  -953,  -954,  
   -955,  -956,  -957,  -958,  -959,  -960,  -962,  -963,  
   -964,  -965,  -966,  -967,  -968,  -969,  -970,  -971,  
   -972,  -973,  -974,  -975,  -976,  -977,  -978,  -978,  
   -979,  -980,  -981,  -982,  -983,  -984,  -985,  -986,  
   -986,  -987,  -988,  -989,  -990,  -990,  -991,  -992,  
   -993,  -994,  -994,  -995,  -996,  -997,  -997,  -998,  
   -999,  -999,  -1000,  -1001,  -1001,  -1002,  -1003,  -1003,  
   -1004,  -1004,  -1005,  -1006,  -1006,  -1007,  -1007,  -1008,  
   -1008,  -1009,  -1009,  -1010,  -1010,  -1011,  -1011,  -1012,  
   -1012,  -1013,  -1013,  -1014,  -1014,  -1015,  -1015,  -1015,  
   -1016,  -1016,  -1017,  -1017,  -1017,  -1018,  -1018,  -1018,  
   -1019,  -1019,  -1019,  -1019,  -1020,  -1020,  -1020,  -1020,  
   -1021,  -1021,  -1021,  -1021,  -1022,  -1022,  -1022,  -1022,  
   -1022,  -1022,  -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  
   -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  
   -1024,  -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  
   -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  -1023,  -1022,  
   -1022,  -1022,  -1022,  -1022,  -1022,  -1021,  -1021,  -1021,  
   -1021,  -1020,  -1020,  -1020,  -1020,  -1019,  -1019,  -1019,  
   -1019,  -1018,  -1018,  -1018,  -1017,  -1017,  -1017,  -1016,  
   -1016,  -1015,  -1015,  -1015,  -1014,  -1014,  -1013,  -1013,  
   -1012,  -1012,  -1011,  -1011,  -1010,  -1010,  -1009,  -1009,  
   -1008,  -1008,  -1007,  -1007,  -1006,  -1006,  -1005,  -1004,  
   -1004,  -1003,  -1003,  -1002,  -1001,  -1001,  -1000,  -999,  
   -999,  -998,  -997,  -997,  -996,  -995,  -994,  -994,  
   -993,  -992,  -991,  -990,  -990,  -989,  -988,  -987,  
   -986,  -986,  -985,  -984,  -983,  -982,  -981,  -980,  
   -979,  -978,  -978,  -977,  -976,  -975,  -974,  -973,  
   -972,  -971,  -970,  -969,  -968,  -967,  -966,  -965,  
   -964,  -963,  -962,  -960,  -959,  -958,  -957,  -956,  
   -955,  -954,  -953,  -951,  -950,  -949,  -948,  -947,  
   -946,  -944,  -943,  -942,  -941,  -939,  -938,  -937,  
   -936,  -934,  -933,  -932,  -930,  -929,  -928,  -927,  
   -925,  -924,  -922,  -921,  -920,  -918,  -917,  -916,  
   -914,  -913,  -911,  -910,  -908,  -907,  -906,  -904,  
   -903,  -901,  -900,  -898,  -897,  -895,  -894,  -892,  
   -890,  -889,  -887,  -886,  -884,  -883,  -881,  -879,  
   -878,  -876,  -875,  -873,  -871,  -870,  -868,  -866,  
   -865,  -863,  -861,  -860,  -858,  -856,  -854,  -853,  
   -851,  -849,  -847,  -846,  -844,  -842,  -840,  -839,  
   -837,  -835,  -833,  -831,  -829,  -828,  -826,  -824,  
   -822,  -820,  -818,  -816,  -814,  -813,  -811,  -809,  
   -807,  -805,  -803,  -801,  -799,  -797,  -795,  -793,  
   -791,  -789,  -787,  -785,  -783,  -781,  -779,  -777,  
   -775,  -773,  -771,  -769,  -767,  -765,  -762,  -760,  
   -758,  -756,  -754,  -752,  -750,  -748,  -745,  -743,  
   -741,  -739,  -737,  -735,  -732,  -730,  -728,  -726,  
   -724,  -721,  -719,  -717,  -715,  -712,  -710,  -708,  
   -706,  -703,  -701,  -699,  -696,  -694,  -692,  -690,  
   -687,  -685,  -683,  -680,  -678,  -675,  -673,  -671,  
   -668,  -666,  -664,  -661,  -659,  -656,  -654,  -652,  
   -649,  -647,  -644,  -642,  -639,  -637,  -634,  -632,  
   -629,  -627,  -625,  -622,  -620,  -617,  -615,  -612,  
   -609,  -607,  -604,  -602,  -599,  -597,  -594,  -592,  
   -589,  -587,  -584,  -581,  -579,  -576,  -574,  -571,  
   -568,  -566,  -563,  -561,  -558,  -555,  -553,  -550,  
   -547,  -545,  -542,  -539,  -537,  -534,  -531,  -529,  
   -526,  -523,  -521,  -518,  -515,  -512,  -510,  -507,  
   -504,  -501,  -499,  -496,  -493,  -491,  -488,  -485,  
   -482,  -479,  -477,  -474,  -471,  -468,  -466,  -463,  
   -460,  -457,  -454,  -451,  -449,  -446,  -443,  -440,  
   -437,  -434,  -432,  -429,  -426,  -423,  -420,  -417,  
   -414,  -412,  -409,  -406,  -403,  -400,  -397,  -394,  
   -391,  -388,  -386,  -383,  -380,  -377,  -374,  -371,  
   -368,  -365,  -362,  -359,  -356,  -353,  -350,  -347,  
   -344,  -342,  -339,  -336,  -333,  -330,  -327,  -324,  
   -321,  -318,  -315,  -312,  -309,  -306,  -303,  -300,  
   -297,  -294,  -291,  -288,  -285,  -282,  -279,  -276,  
   -273,  -270,  -267,  -264,  -260,  -257,  -254,  -251,  
   -248,  -245,  -242,  -239,  -236,  -233,  -230,  -227,  
   -224,  -221,  -218,  -215,  -212,  -209,  -205,  -202,  
   -199,  -196,  -193,  -190,  -187,  -184,  -181,  -178,  
   -175,  -171,  -168,  -165,  -162,  -159,  -156,  -153,  
   -150,  -147,  -144,  -140,  -137,  -134,  -131,  -128,  
   -125,  -122,  -119,  -115,  -112,  -109,  -106,  -103,  
   -100,  -97,  -94,  -90,  -87,  -84,  -81,  -78,  
   -75,  -72,  -69,  -65,  -62,  -59,  -56,  -53,  
   -50,  -47,  -43,  -40,  -37,  -34,  -31,  -28,  
   -25,  -21,  -18,  -15,  -12,  -9,  -6,  -3 
]

g_rand_shit = [
  127, 245, 223, 255, 254, 255, 255, 255,
  255, 191, 254, 255, 255, 255, 255, 223,
  255, 191, 254, 247, 255, 255, 255, 255,
  223, 255, 61,  255, 183, 255, 255, 255,
  255, 191, 249, 255, 255, 245, 255, 250,
  215, 255, 255, 255, 127, 255, 255, 255,
  255, 223, 255, 255, 255, 255, 255, 255,
  255, 127, 255, 254, 243, 255, 239, 255,
  237, 255, 255, 255, 255, 247, 255, 255,
  255, 255, 187, 255, 251, 251, 255, 223,
  255, 253, 255, 255, 255, 255, 127, 255,
  191, 255, 255, 255, 255, 255, 223, 255,
  255, 251, 123, 255, 253, 254, 239, 251,
  255, 255, 223, 255, 255, 255, 255, 255,
  251, 255, 251, 223, 255, 231, 251, 255,
  255, 255, 221, 255, 251, 255, 255, 223,
  255, 219, 255, 245, 255, 255, 255, 255,
  255, 255, 251, 255, 59,  255, 255, 223,
  255, 191, 255, 127, 255, 223, 255, 127,
  255, 255, 255, 255, 255, 255, 255, 119,
  255, 255, 255, 253, 255, 255, 253, 255,
  255, 254, 252, 247, 255, 255, 255, 62,
  255, 251, 247, 255, 189, 255, 255, 255,
  255, 126, 251, 253, 255, 255, 191, 255,
  255, 127, 222, 255, 159, 255, 255, 249,
  255, 255, 191, 255, 254, 255, 255, 255,
  255, 207, 221, 251, 253, 255, 255, 255,
  190, 191, 255, 255, 255, 255, 127, 255,
  255, 243, 255, 247, 255, 255, 253, 255,
  255, 223, 255, 255, 255, 251, 255, 255,
  255, 191, 255, 254, 191, 255, 255, 255,
  251, 255, 255, 191, 255, 255, 255, 255
]


def setup(wave_type, freq, 
          freq_ramp=0, freq_slur=0,
          lpf_base=255, lpf_ramp=0, lpf_resonance=0,
          vib_speed=1, vib_strength=0,
          tremolo=False, distortion=False
         ):
  global g_wave_type, g_env_lengths, g_base_freq
  global g_freq_ramp, g_freq_slur, g_freq_ramp_cnt
  global g_lpf_base_freq, g_lpf_ramp, g_lpf_resonance
  global g_vib_speed, g_vib_strength
  global g_dist_status, g_trem_status

  # set up the initial values for all the controls
  g_wave_type = wave_type
  g_base_freq = freq

  g_env_lengths[0] = 1000
  g_env_lengths[1] = 1500

  g_freq_ramp = freq_ramp
  g_freq_slur = freq_slur

  g_lpf_base_freq = lpf_base
  g_lpf_ramp = lpf_ramp
  g_lpf_resonance = lpf_resonance
  
  g_vib_speed = vib_speed
  g_vib_strength = vib_strength
  
  g_trem_status = tremolo
  g_dist_status = distortion

  # start playing
  reset_sample()


def reset_sample():
  global g_wave_type, g_env_lengths, g_base_freq
  global g_freq_ramp, g_freq_slur, g_freq_ramp_cnt
  global g_lpf_base_freq, g_lpf_ramp, g_lpf_resonance
  global g_vib_speed, g_vib_strength
  global g_phase, g_curr_freq
  global g_lpf_prev, g_lpf_prev_delta, g_lpf_curr_freq, g_lpf_ramp_cnt
  global g_vib_phase, g_vib_fix_accum, g_interrupt_cnt
  global g_env_stage, g_env_time
  
  g_phase = 0
  g_curr_freq = g_base_freq
  if g_freq_slur:
    g_curr_freq = max(40, g_base_freq/(g_freq_slur+1))
  g_freq_ramp_cnt = 0
  
  # reset filters
  g_lpf_prev = 0
  g_lpf_prev_delta = 0
  g_lpf_curr_freq = g_lpf_base_freq
  g_lpf_ramp_cnt = 0
  
  # reset vibrato
  g_vib_phase = 0
  g_vib_fix_accum = 0
  
  # reset tremolo, distortion, and shit
  g_interrupt_cnt = 0
  
  # reset envelope
  g_env_stage=0
  g_env_time=0


# interrupt handler - that's where the synthesis happens
def next_sample():
  global g_wave_type, g_env_lengths, g_base_freq
  global g_freq_ramp, g_freq_slur, g_freq_ramp_cnt
  global g_lpf_base_freq, g_lpf_ramp, g_lpf_resonance
  global g_vib_speed, g_vib_strength
  global g_phase, g_curr_freq
  global g_lpf_prev, g_lpf_prev_delta, g_lpf_curr_freq, g_lpf_ramp_cnt
  global g_vib_phase, g_vib_fix_accum, g_interrupt_cnt
  global g_env_stage, g_env_time
  
  # use another oscillator with a lower frequency for the vibrato
  g_vib_phase = (g_vib_phase + g_vib_speed) & 0xffff
  
  if g_vib_strength > 0:
    # vib_fix should be between -0x80 and 0x7f
    g_vib_fix_accum += phase_to_delta[g_vib_phase/0x4000]
    vib_fix = (g_vib_fix_accum / 0x80) * g_vib_strength / 0x10
    
    # fix the frequency according to the vibrato
    if vib_fix < 0 and -vib_fix >= g_curr_freq:
      vibrated_freq = 1
    else:
      vibrated_freq = g_curr_freq + vib_fix
  else:
    vibrated_freq = g_curr_freq
  
  #
  # volume envelope
  #
  
  # compute by stage, keeping env_vol between zero and 0xff
  if g_env_stage == 0:
    g_env_time += 1
    if g_env_time >= g_env_lengths[0]:
      g_env_time = 0
      g_env_stage = 1
    env_vol = g_env_time / (g_env_lengths[0]/0x100 + 1)
  elif g_env_stage == 2:
    g_env_time += 1
    if g_env_time >= g_env_lengths[1]:
      g_env_time = 0
      g_env_stage = 3
    env_vol=255-(g_env_time / (g_env_lengths[1]/0x100 + 1))
  elif g_env_stage == 1:
    env_vol = 255
  else:
    env_vol = 0
  
  g_interrupt_cnt += 1
  # tremolo adjustments
  if g_trem_status:
    # do the shit at ~16Hz
    if g_interrupt_cnt & 0x400:
      env_vol /= 2
  
  # distortion adjustments
  if g_dist_status:
    env_vol &= g_rand_shit[g_interrupt_cnt & 0xFF]
  
  #
  # phase of the current wave
  #
  
  g_phase += vibrated_freq
  if g_phase & 0x8000:
    g_phase &= 0x7FFF
    
    #
    # wave length wraparound. this is a good time to make changes in the frequency
    #
    
    # fix the frequency according to the ramp
    if g_freq_ramp:
      g_freq_ramp_cnt += 30
      g_curr_freq += (g_freq_ramp_cnt/g_freq_ramp)
      g_freq_ramp_cnt %= g_freq_ramp
      #g_freq_ramp_cnt += 1
      #if g_freq_ramp_cnt == g_freq_ramp:
      #  g_freq_ramp_cnt = 0
      #  g_curr_freq += 1
    # or slur
    elif g_freq_slur and g_curr_freq < g_base_freq:
      g_freq_ramp_cnt += g_freq_slur
      g_curr_freq += 8+(g_freq_ramp_cnt/8)
      g_freq_ramp_cnt %= 8
      if g_curr_freq > g_base_freq:
        g_curr_freq = g_base_freq
  
  #
  # current sample computation
  #
  
  # base waveform
  fp = g_phase >> 4 # keep fp between zero and 2047
  sample = 0
  if g_wave_type < 2:
    if g_wave_type == 0: # rough sawtooth
      sample = 1023-fp
    else: # sawtooth
      sample = (1023-fp*2) if (fp < 1024) else (fp*2-3072)
  elif g_wave_type == 2: # sine
    sample = mysin_table[fp]
  else: # square
    sample = 1023 if (fp & 1024) else -1023
  
  # sample is between -1024 and 1023
  
  #
  # low-pass filter
  #
  
  if g_lpf_base_freq != 256:
    # to avoid ramping the low-pass frequency too quickly, use a counter
    g_lpf_ramp_cnt += 1
    # adjust the low-pass filter current frequency
    if g_lpf_ramp < 0 and g_lpf_ramp_cnt == -g_lpf_ramp:
      g_lpf_ramp_cnt = 0
      if g_lpf_curr_freq:
        g_lpf_curr_freq -= 1
    elif g_lpf_ramp > 0 and g_lpf_ramp_cnt == g_lpf_ramp:
      g_lpf_ramp_cnt = 0
      if g_lpf_curr_freq + 1 != 0:
        g_lpf_curr_freq += 1
    
    # start with the resonance - multiply g_lpf_prev_delta by the factor
    g_lpf_prev_delta /= 0x10
    g_lpf_prev_delta *= g_lpf_resonance
    g_lpf_prev_delta /= 0x10

    #
    # now add the low-pass part
    #
    # since sample and g_lpf_prev are both between -1024 and 1023,
    # we can be sure that g_lpf_prev_delta will not be
    # incremented/decremented by more than 2047
    #
    g_lpf_prev_delta += (sample-g_lpf_prev) / 0x10 * g_lpf_curr_freq / 0x10
    if g_lpf_prev_delta > 2047:
      g_lpf_prev_delta = 2047
    elif g_lpf_prev_delta < -2048:
      g_lpf_prev_delta = -2048
    
    # accumulate it to the filter's output
    g_lpf_prev += g_lpf_prev_delta
    
    if g_lpf_prev > 1023:
      g_lpf_prev = 1023
    if g_lpf_prev < -1024:
      g_lpf_prev = -1024
    
    # filter output
    sample = g_lpf_prev
  else:
    g_lpf_prev = sample
    g_lpf_prev_delta = 0
  
  # now sample is between -1024 and 1023
  # scale it between -128 and 127
  sample = sample / 8
  
  # adjust with the volume envelope
  sample *= env_vol
  sample = sample / 256
  
  return (sample + 128) & 0xff


WAVE_TYPES = ["saw", "triangle", "sine", "square"]

if __name__ == "__main__":
  import sys
  from optparse import OptionParser

  op = OptionParser()

  op.add_option(
    "-t", "--type", dest="wave_type",
    help="wave type (possible options: %s, %s, %s, %s)" % tuple(WAVE_TYPES),
    type="string", default="saw",
  )
  op.add_option(
    "-f", "--freq", dest="freq", default=440,
    help="wave base frequency", type="int"
  )
  op.add_option(
    "--framp", dest="freq_ramp",
    help="frequency ramp (amount of change in frequency after the base)",
    type="int", default=0,
  )
  op.add_option(
    "--fslur", dest="freq_slur",
    help="frequency slur (amount of change in frequency before the base)",
    type="int", default=0,
  )
  op.add_option(
    "-v", "--vib-speed", dest="vib_speed",
    help="vibrato speed", type="int", default=1,
  )
  op.add_option(
    "--vib-amp", dest="vib_amp",
    help="vibrato amplitude", type="int", default=0,
  )
  op.add_option(
    "-l", "--low-pass", dest="lpf_base",
    help="low-pass filter base frequency", type="int", default=255,
  )
  op.add_option(
    "--low-pass-ramp", dest="lpf_ramp",
    help="low-pass filter frequency ramp", type="int", default=0,
  )
  op.add_option(
    "-r", "--resonance", dest="lpf_resonance",
    help="low-pass filter resonance", type="int", default=0,
  )
  op.add_option(
    "--tremolo", dest="tremolo",
    help="add a low-frequency tremolo", action="store_true", default=False,
  )
  op.add_option(
    "--distortion", dest="distort",
    help="add distortion", action="store_true", default=False,
  )

  (options, args) = op.parse_args()
  
  if len(args) != 1:
    print "Usage: %s [options] FILENAME" % sys.argv[0]
    sys.exit()

  if options.wave_type not in WAVE_TYPES:
    print "unknown wave type %s. use --help for more information" % \
      options.wave_type
    sys.exit()

  if options.freq < 20 or options.freq > 14000:
    print "wrong frequency %d. expected values are 20 to 14000" % options.freq
    sys.exit()

  if options.freq_ramp < -128 or options.freq_ramp > 127:
    print "wrong frequency ramp %d. expected values are 0 to 255" % \
      options.freq_ramp
    sys.exit()
  if options.freq_slur < 0 or options.freq_slur > 255:
    print "wrong frequency slur %d. expected values are 0 to 255" % \
      options.freq_slur
    sys.exit()
  if options.vib_speed < 0 or options.vib_speed > 255:
    print "wrong vibrato speed %d. expected values are 0 to 255" % \
      options.vib_speed
    sys.exit()
  if options.vib_amp < 0 or options.vib_amp > 255:
    print "wrong vibrato amplitude %d. expected values are 0 to 255" % \
      options.vib_amp
    sys.exit()
  if options.lpf_ramp < -128 or options.lpf_ramp > 127:
    print "wrong low-pass ramp %d. expected values are 0 to 255" % \
      options.lpf_ramp
    sys.exit()
  if options.lpf_base < 0 or options.lpf_base > 255:
    print "wrong low-pass frequency %d. expected values are 0 to 255" % \
      options.lpf_base
    sys.exit()
  if options.lpf_resonance < 0 or options.lpf_resonance > 255:
    print "wrong resonance %d. expected values are 0 to 255" % \
      options.lpf_resonance
    sys.exit()

  f = open(args[0], "wb")

  setup(
    wave_type=WAVE_TYPES.index(options.wave_type),
    freq = options.freq, freq_ramp = options.freq_ramp,
    freq_slur = options.freq_slur,
    vib_strength = options.vib_amp, vib_speed = options.vib_speed,
    lpf_ramp = options.lpf_ramp, lpf_base = options.lpf_base,
    lpf_resonance = options.lpf_resonance,
    distortion = options.distort, tremolo = options.tremolo,
  )

  data = ""
  for i in range(20000):
    data += chr(next_sample())

  f.write("RIFF")
  f.write(struct.pack("<I", len(data)+0x24))
  f.write("WAVEfmt ")
  f.write(struct.pack("<IHHIIHH", 0x10, 1, 1, 32000, 32000, 1, 8))
  f.write("data")
  f.write(struct.pack("<I", len(data)))
  f.write(data)
  f.close()

