This script generates sounds that can be used in games.
Each sound file is 16KHz, mono, 8-bit unsigned.

The volume envelope is designed to provide a ~1 second file.

The script can receive arguments to set parameters like:
base frequency, low-pass cutoff, vibrato, etc.

Here's some examples of sounds generated using this script:
./sound_gen.py --freq 400 --framp -15 downstep.wav
./sound_gen.py --freq 1600 --fslur 4 --framp 15 climbstep.wav

./sound_gen.py -t saw --freq 220 simon01.wav
./sound_gen.py -t saw --freq 275 simon02.wav
./sound_gen.py -t saw --freq 330 simon03.wav
./sound_gen.py -t saw --freq 440 simon04.wav

