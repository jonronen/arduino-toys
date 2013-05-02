#!/usr/bin/python


#
# Arduino Toys
# Copyright 2012-2013 Jon Ronen-Drori
#
# An Arduino-based platform for children's toys
# with LEDs, push-buttons, and sounds
#
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#


MAX_REG = 15
BLOCK_SIZE = 512


g_ops = {
  # NAME                    OPCODE  PARAM_TYPES
  "SET_REG":                ("1",   ("REG", "BYTE_OR_STATE")),
  "ADD_REGS":               ("001", ("REG", "REG", "REG")),
  "SUB_REGS":               ("002", ("REG", "REG", "REG")),
  "MUL_REGS":               ("003", ("REG", "REG", "REG")),
  "DIV_REGS":               ("004", ("REG", "REG", "REG")),
  "AND_REGS":               ("005", ("REG", "REG", "REG")),
  "OR_REGS":                ("006", ("REG", "REG", "REG")),
  "XOR_REGS":               ("007", ("REG", "REG", "REG")),
  "LIGHT_ON":               ("2",   ("REG",)),
  "LIGHT_OFF":              ("3",   ("REG",)),
  "PLAY_SOUND":             ("4",   ("REG",)),
  "PLAY_SOUND_BLOCKING":    ("5",   ("REG",)),
  "GET_INPUT":              ("01",  ("REG", "REG")),
  "DELAY":                  ("6",   ("REG",)),
  "LOAD":                   ("02",  ("REG", "REG")),
  "STORE":                  ("03",  ("REG", "REG")),
  "JUMP":                   ("7",   ("REG",)),
  "JUMP_EQUAL":             ("8",   ("REG", "REG", "REG")),
  "JUMP_GREATER":           ("9",   ("REG", "REG", "REG")),
  "RANDOM":                 ("04",  ("REG", "REG")),
  "LOG":                    ("05",  ("REG", "REG")),
  "BIT":                    ("06",  ("REG", "REG")),
}


def assemble_arg(arg, argtype, state_names):
  arg = arg.strip().lower()

  if argtype == "REG":
    if not arg.startswith("r"):
      raise ValueError("expected 'r' in the beginning of a register")
    val = int(arg[1:])
    if val<0 or val>MAX_REG:
      raise ValueError("register value %d is out of bounds" % val)
    return "%x" % val

  elif argtype == "BYTE_OR_STATE":
    if arg.startswith("@") and arg[1:] in state_names:
      val = state_names.index(arg[1:])
    else:
      val = int(arg, 0)
    if val<0 or val>0xff:
      raise ValueError("byte value %d is out of bounds" % val)
    return "%02x" % val

def assemble_op(line, state_names):
  tokens = line.strip().split(None, 1)
  if len(tokens) != 2:
    raise ValueError("expected opcode and arguments")
  
  op, args = tokens
  if op not in g_ops.keys():
    raise ValueError("unknown op %s" % op)
  
  args = args.split(",")
  if len(args) != len(g_ops[op][1]):
    raise ValueError(
      "wrong number of args for %s: received %d, expected %d" % \
      (op, len(args), len(g_ops[op][1]))
    )
  
  asm_args = ""
  try:
    for i in range(len(args)):
      asm_args += assemble_arg(args[i], g_ops[op][1][i], state_names)
  except ValueError, e:
    raise ValueError("error in args of %s: %s" % (op, e.message))
  
  result = g_ops[op][0] + "".join(asm_args)
  result = result.decode('hex')
  
  return result

def assemble_file(f):
  result = ""
  line_num = 0
  state_names = []
  current_state_block = ""

  # first pass: build the state names
  for line in f:
    if line.strip().startswith(":"):
      # add the state name or report an error if already exists
      state_name = line.strip()[1:].split()[0].lower()
      if state_name[0].isdigit():
        raise ValueError(
          "line %d: state %s begins with a number" % (line_num, state_name)
        )
      if state_name in state_names:
        raise ValueError(
          "line %d: state %s already exists" % (line_num, state_name)
        )
      state_names.append(state_name)
      if len(state_names) > 0x100:
        raise ValueError(
          "too many states (state %s is already the 256th state)" % \
          state_name
        )

  # second pass: assemble the states and concatenate them
  f.seek(0)
  for line in f:
    line_num += 1

    # discard empty lines and comments
    if len(line.strip()) == 0: continue
    if line.strip().startswith("#"): continue
    if line.strip().startswith(";"): continue

    # handle a new state
    if line.strip().startswith(":"):
      if current_state_block:
        current_state_block += "\x00" * \
            ((BLOCK_SIZE-len(current_state_block)) % BLOCK_SIZE)
      result += current_state_block
      current_state_block = ""
      continue

    try:
      current_state_block += assemble_op(line, state_names)
    except ValueError, e:
      raise ValueError(
        "Error in line %d: %s" % (line_num, e.message)
      )

  # when we're done, the last block is probably still not written
  if current_state_block:
    current_state_block += "\x00" * \
        ((BLOCK_SIZE-len(current_state_block)) % BLOCK_SIZE)
    result += current_state_block

  return result, line_num


if __name__ == "__main__":
  import sys

  if len(sys.argv) == 1:
    print "Usage: %s FILE" % sys.argv[0]
    sys.exit()

  try:
    f = open(sys.argv[1], "rt")
  except IOError, e:
    print "error opening %s: %s" % (sys.argv[1], e.message)
    sys.exit()

  try:
    assembled_code, line_num = assemble_file(f)
  except ValueError, e:
    print "error assembling %s: %s" % (sys.argv[1], e.message)
    sys.exit()

  f.close()

  out_file = open(sys.argv[1] + ".bin", "wb")
  out_file.write(assembled_code)
  out_file.close()

  print "%d lines assembled into %s" % (line_num, sys.argv[1] + ".bin")

