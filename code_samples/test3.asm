:STATE_ZERO
SET_REG r0, 0
LIGHT_ON r0
SET_REG r1, 30
GET_INPUT r3, r1
SET_REG r2, @STATE_ONE
JUMP r2

:STATE_ONE
LIGHT_OFF r0
GET_INPUT r3, r1
SET_REG r2, @STATE_ZERO
JUMP r2

