:START
# r9 holds the last random (to make sure the next random is different)
# initialise it to an invalid value
SET_REG r9, 0xff
SET_REG r3, @SAVE_RAND_AND_PLAY
JUMP r3

:SAVE_RAND_AND_PLAY

# get a random value between zero and four
SET_REG r0, 4
RANDOM r1, r0

# if the random is equal to the last one, get another random
SET_REG r3, @SAVE_RAND_AND_PLAY
JUMP_EQUAL r1, r9, r3

# else, set r9 to be the new random
SET_REG r10, 0
ADD_REGS r9, r1, r10

# light and play
PLAY_SOUND_BLOCKING r1

# save the new random
SET_REG r2, 0
STORE r1, r2

# now wait for an input
SET_REG r3, @WAIT_FOR_INPUT
JUMP r3


:WAIT_FOR_INPUT

# timeout is ten seconds
SET_REG r1, 50
GET_INPUT r8, r1

# convert the saved value to an expected input
LOAD r0, r2
BIT r1, r0

# if r8==r1, success
SET_REG r3, @SUCCESS_AND_CONTINUE
JUMP_EQUAL r1, r8, r3

# else, if no button is pressed, let's give another chance
SET_REG r1, 0
SET_REG r3, @ANOTHER_CHANCE
JUMP_EQUAL r8, r1, r3

# else, error and continue
SET_REG r3, @ERROR_AND_CONTINUE
JUMP r3


:ERROR_AND_CONTINUE

# play an error sound, delay, and return to the start
SET_REG r1, 5
PLAY_SOUND_BLOCKING r1
PLAY_SOUND_BLOCKING r1
PLAY_SOUND_BLOCKING r1
DELAY r1
SET_REG r3, @SAVE_RAND_AND_PLAY
JUMP r3


:ANOTHER_CHANCE

# play the sound again
LOAD r0, r2
LIGHT_ON r0
PLAY_SOUND_BLOCKING r0
LIGHT_OFF r0

# timeout is ten seconds
SET_REG r1, 50
GET_INPUT r8, r1

# convert the saved value to an expected input
LOAD r0, r2
BIT r1, r0

# if r8==r1, success
SET_REG r3, @SUCCESS_AND_CONTINUE
JUMP_EQUAL r1, r8, r3

# else, error and continue
SET_REG r3, @ERROR_AND_CONTINUE
JUMP r3


:SUCCESS_AND_CONTINUE
# randomly choose a success sound, delay, and choose a new random
SET_REG r4, 5
RANDOM r5, r4
SET_REG r4, 6
ADD_REGS r5, r5, r4
LIGHT_ON r0
PLAY_SOUND_BLOCKING r5
LIGHT_OFF r0
SET_REG r1, 8
DELAY r1
SET_REG r3, @SAVE_RAND_AND_PLAY
JUMP r3

