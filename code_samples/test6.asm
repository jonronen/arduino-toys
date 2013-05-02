:SAVE_RAND_AND_PLAY

# get a random value between zero and four
SET_REG r0, 5
RANDOM r1, r0

# light and play
LIGHT_ON r1
PLAY_SOUND_BLOCKING r1
LIGHT_OFF r1

# save it
SET_REG r2, 0
STORE r1, r2

# now wait for an input
SET_REG r3, @WAIT_FOR_INPUT
JUMP r3


:WAIT_FOR_INPUT

# timeout is ten seconds
SET_REG r1, 100
GET_INPUT r8, r1

# convert the saved value to an expected input
LOAD r0, r2
SET_REG r3, @CONVERT_INPUT
JUMP r3


:CONVERT_INPUT
# r0 is a parameter
SET_REG r1, 1
SET_REG r2, 0
SET_REG r3, @CONVERT_INPUT_LOOP
JUMP r3

:CONVERT_INPUT_LOOP
# r0 is the limit, r1 is the accumulator, r2 is the index
SET_REG r4, 2
SET_REG r5, 1

# if r2==r0, we're done and r1 is the answer
SET_REG r3, @COMPARE_INPUT
JUMP_EQUAL r2, r0, r3

# else, r2++ and r1 *= 2
MUL_REGS r1, r1, r4
ADD_REGS r2, r2, r5

# continue
SET_REG r3, @CONVERT_INPUT_LOOP
JUMP r3


:COMPARE_INPUT

# if r8==r1, success
SET_REG r3, @SUCCESS_AND_CONTINUE
JUMP_EQUAL r1, r8, r3

# else, play an error sound, delay, and return to the start
SET_REG r1, 5
PLAY_SOUND_BLOCKING r1
PLAY_SOUND_BLOCKING r1
PLAY_SOUND_BLOCKING r1
DELAY r1
SET_REG r3, @SAVE_RAND_AND_PLAY
JUMP r3


:SUCCESS_AND_CONTINUE
SET_REG r1, 8
DELAY r1
SET_REG r3, @SAVE_RAND_AND_PLAY
JUMP r3

