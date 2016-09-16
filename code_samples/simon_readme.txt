Simon:
======

two flavours:
simon.asm - 5 sounds, 5 buttons
simon_4_btn.asm - 4 sounds, 4 buttons

sounds:
01 - first button
02 - second button
03 - third button
04 - fourth button
05 - fifth button (optional)
06 - error sound
07 - success sound

Example of a 4-button simon sound generation:
./sound_gen.py -t saw --freq 220 simon01.wav
./sound_gen.py -t saw --freq 275 simon02.wav
./sound_gen.py -t saw --freq 330 simon03.wav
./sound_gen.py -t saw --freq 440 simon04.wav
./sound_gen.py -t saw --freq 440 simon05.wav
./sound_gen.py --freq 2400 --framp -15 simon06.wav
./sound_gen.py --freq 1600 --fslur 4 --framp 15 simon07.wav

Example of a 5-button simon sound generation:
./sound_gen.py --freq 220 simon01.wav
./sound_gen.py --freq 247 simon02.wav
./sound_gen.py --freq 275 simon03.wav
./sound_gen.py --freq 330 simon04.wav
./sound_gen.py --freq 352 simon05.wav
./sound_gen.py --freq 371 simon05.wav
./sound_gen.py --freq 2400 --framp -15 simon06.wav
./sound_gen.py --freq 1600 --fslur 4 --framp 15 simon07.wav

