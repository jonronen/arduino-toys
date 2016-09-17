# conventions:
# r1 holds the current color needed to be guessed
# r3 is the next state to jump to
# r7 holds max number of rounds
# r8 holds the current round
# r9 holds the last random (to make sure the next random is different)

#sounds
#lang01 - Yellow     
#lang02 - Red    
#lang03 - Blue   
#lang04 - Green
#
#lang05 - "I said RED, you idiot" --> 6
#lang06 - "Arrrrrrr"      
#lang07 - "Are you color blind or what?"
#lang08 - "You stupid, The other Green!"


:START
SET_REG r7, 8
SET_REG r8, 0
# initialise it to an invalid value
SET_REG r9, 0xff
SET_REG r3, @SAVE_RAND_AND_PLAY
JUMP r3


:SAVE_RAND_AND_PLAY
# check if we are after 8 correct rounds of simple color learning.
# if yes, move to the next game
SET_REG r3, @PLAY_SIMON
JUMP_EQUAL r7, r8, r3

# get a random value between zero and three (only 4 buttons buttons)
SET_REG r0, 4
RANDOM r1, r0

# if the random is equal to the last one, get another random
SET_REG r3, @SAVE_RAND_AND_PLAY
JUMP_EQUAL r1, r9, r3

# else, store the new random in r9
SET_REG r10, 0
ADD_REGS r9, r1, r10

# play target sound
PLAY_SOUND_BLOCKING r1

# wait for the input from player (delay 5 secs)
SET_REG r5, 50
GET_INPUT r2, r5

# timeout or incorrect key?
# r5->Correct color
# r2->user input
BIT r5, r1
SET_REG r3, @ERROR_1
LOG r6, r2
JUMP_GREATER r5, r2, r3
JUMP_GREATER r2, r5, r3

# correct key - play it and move to the next one
LIGHT_ON r1
PLAY_SOUND_BLOCKING r1
LIGHT_OFF r1

# increment the round counter
SET_REG r10, 1
ADD_REGS r7, r7, r10

# end of sequence - wait a bit and get next random
SET_REG r2, 8
DELAY r2
SET_REG r3, @SAVE_RAND_AND_PLAY
JUMP r3

# next stage game - guess series of colors
:PLAY_SIMON


:ERROR_1
SET_REG r2, 4
ADD_REGS r2, r2, r1
PLAY_SOUND_BLOCKING r2 
SET_REG r3, @SAVE_RAND_AND_PLAY
JUMP r3