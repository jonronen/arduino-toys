# wait for an input
:WAIT_FOR_INPUT

# no timeout
SET_REG r0, 0
GET_INPUT r1, r0
# if we're here, we have an input, so r1 is definitely nonzero
LOG r2, r1

# if the sound is 4 (well, more than 3), nothing to play
SET_REG r3, @WAIT_FOR_INPUT
SET_REG r4, 3
JUMP_GREATER r2, r4, r3

# the sound is valid - play it
PLAY_SOUND_BLOCKING r2
JUMP r3

