# conventions:
# r0 is always zero
# r1 is always one
# r3 is the next state to jump to
# r13 is the location of the current sound
# r14 is the length of the current sequence
# r15 is the maximal length of the sequence

:INIT
SET_REG r0, 0
SET_REG r1, 1
SET_REG r13, 0
SET_REG r14, 0
SET_REG r15, 10
SET_REG r3, @WAIT_STATE
JUMP r3

# wait for the first two buttons to be pressed
:WAIT_STATE

# set the lights to show which buttons should be pressed
SET_REG r2, 0
LIGHT_ON r2
SET_REG r2, 1
LIGHT_ON r2

SET_REG r5, 5
GET_INPUT r4, r5

# turn off the lights before checking and (possibly) jumping
SET_REG r2, 0
LIGHT_OFF r2
SET_REG r2, 1
LIGHT_OFF r2

# if the buttons were pressed - start the game
SET_REG r5, 0x03
AND_REGS r4, r4, r5
SET_REG r3, @START_GAME
JUMP_EQUAL r4, r5, r3

# wait some more with the lights off
SET_REG r5, 5
GET_INPUT r4, r5
SET_REG r5, 0x03
AND_REGS r4, r4, r5
SET_REG r3, @START_GAME
JUMP_EQUAL r4, r5, r3

# wait some more...
SET_REG r3, @WAIT_STATE
JUMP r3


# small delay before we begin the actual game
:START_GAME
SET_REG r2, 5
DELAY r2
SET_REG r3, @FILL_NEXT_RANDOM
JUMP r3


# keep r8 as a global offset within the buffer
:FILL_NEXT_RANDOM

# if r14 < r15, there's more randoms to fill.
# else, we're done - we have a winner!
SET_REG r3, @WIN
JUMP_EQUAL r14, r15, r3

# get the random and fill it in
SET_REG r4, 4
RANDOM r5, r4
STORE r5, r14
ADD_REGS r14, r14, r1

# now play all the sequence
SET_REG r13, 0
SET_REG r3, @PLAY_SEQUENCE
JUMP r3


:PLAY_SEQUENCE
# get the sequence, one by one, and play
LOAD r2, r13
LIGHT_ON r2
PLAY_SOUND_BLOCKING r2
LIGHT_OFF r2

# increment r13 to play the next
ADD_REGS r13, r13, r1
SET_REG r3, @PLAY_SEQUENCE
JUMP_GREATER r14, r13, r3

# we're done playing the sequence, now wait for a repetition
SET_REG r13, 0
SET_REG r3, @WAIT_FOR_KEY_PRESS
JUMP r3



:WAIT_FOR_KEY_PRESS
# timeout is 5 seconds
SET_REG r4, 50
GET_INPUT r2, r4

# timeout or incorrect key?
LOAD r4, r13
BIT r5, r4
SET_REG r3, @ERROR
JUMP_GREATER r5, r2, r3
JUMP_GREATER r2, r5, r3

# correct key - play it and move to the next one
LIGHT_ON r4
PLAY_SOUND_BLOCKING r4
LIGHT_OFF r4
ADD_REGS r13, r13, r1

# end of sequence?
SET_REG r3, @WAIT_FOR_KEY_PRESS
JUMP_GREATER r14, r13, r3

# end of sequence - wait a bit and get next random
SET_REG r2, 8
DELAY r2
SET_REG r13, 0
SET_REG r3, @FILL_NEXT_RANDOM
JUMP r3


:ERROR
SET_REG r2, 5
PLAY_SOUND_BLOCKING r2
PLAY_SOUND_BLOCKING r2
PLAY_SOUND_BLOCKING r2
SET_REG r3, @INIT
JUMP r3


:WIN
SET_REG r2, 6
PLAY_SOUND_BLOCKING r2
PLAY_SOUND_BLOCKING r2
PLAY_SOUND_BLOCKING r2
SET_REG r3, @INIT
JUMP r3

