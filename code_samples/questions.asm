# conventions:
# r0 is always zero
# r1 is always one
# r3 is the next state to jump to
# r10 is the index of the current answer
# r11 is the maximal number of answers
# r12 is the index of the current question
# r13 is the maximal number of questions (per answer)
# r14 is the number of the current attempt
# r15 is the maximal number of attempts

# sounds:
# 0: success (right!)
# 1: no, let's try again (one more attempt)
# 2-6: failure with correct answer (no... it's the cow!)
# 7+5i - 11+5i: questions (i between zero and r13)

:INIT
SET_REG r0, 0
SET_REG r1, 1
SET_REG r11, 5
SET_REG r13, 5
SET_REG r14, 0
SET_REG r15, 2
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
SET_REG r3, @NEW_QUESTION
JUMP_EQUAL r4, r5, r3

# wait some more with the lights off
SET_REG r5, 5
GET_INPUT r4, r5
SET_REG r5, 0x03
AND_REGS r4, r4, r5
SET_REG r3, @NEW_QUESTION
JUMP_EQUAL r4, r5, r3

# wait some more...
SET_REG r3, @WAIT_STATE
JUMP r3


:NEW_QUESTION

# a new question means the number of attempts is cleared
SET_REG r14, 0

# choose an answer and a question at random
RANDOM r10, r11
RANDOM r12, r13

SET_REG r3, @ASK_THE_QUESTION
JUMP r3


:ASK_THE_QUESTION

# small delay before we ask
SET_REG r2, 10
DELAY r2

# the sound index is 2 + r10 + (r12+1)*r11
ADD_REGS r2, r1, r12
MUL_REGS r2, r2, r11
ADD_REGS r2, r2, r1
ADD_REGS r2, r2, r1
ADD_REGS r2, r2, r10
PLAY_SOUND_BLOCKING r2

# wait for an answer (timeout is 5 seconds)
SET_REG r4, 50
GET_INPUT r2, r4

# correct input?
BIT r5, r10
SET_REG r3, @RIGHT_ANSWER
JUMP_EQUAL r5, r2, r3
SET_REG r3, @ONE_MORE_TRY
JUMP r3


:ONE_MORE_TRY

# should we try again or have we had enough?
ADD_REGS r14, r14, r1
SET_REG r3, @REVEAL_THE_ANSWER
JUMP_EQUAL r14, r15, r3

# play the try again sound and ask again
PLAY_SOUND_BLOCKING r1
SET_REG r3, @ASK_THE_QUESTION
JUMP r3


:REVEAL_THE_ANSWER

# flicker the LED
LIGHT_ON r10
SET_REG r2, 2
DELAY r2
LIGHT_OFF r10
DELAY r2
LIGHT_ON r10
DELAY r2
LIGHT_OFF r10
DELAY r2
LIGHT_ON r10

# play the sound that tells the right answer
ADD_REGS r2, r10, r1
ADD_REGS r2, r2, r1
PLAY_SOUND_BLOCKING r2

# turn off the LED
LIGHT_OFF r10

# and ask a new question
SET_REG r3, @NEW_QUESTION
JUMP r3


:RIGHT_ANSWER

# right!
PLAY_SOUND_BLOCKING r0

# new question please!
SET_REG r3, @NEW_QUESTION
JUMP r3

