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

g_ops = {
  # NAME                    OPCODE  PARAM_TYPES
  "SET_REG":                ("1",   ("REG", "BYTE")),
  "ADD_REGS":               ("2",   ("REG", "REG", "REG")),
  "SUB_REGS":               ("3",   ("REG", "REG", "REG")),
  "MUL_REGS":               ("4",   ("REG", "REG", "REG")),
  "DIV_REGS":               ("5",   ("REG", "REG", "REG")),
  "LIGHT_ON":               ("6",   ("REG")),
  "LIGHT_OFF":              ("7",   ("REG")),
  "PLAY_SOUND":             ("8",   ("REG")),
  "PLAY_SOUND_BLOCKING":    ("9",   ("REG")),
  "GET_INPUT":              ("01",  ("REG", "REG")),
  "DELAY":                  ("A",   ("REG")),
  "LOAD":                   ("02",  ("REG", "REG")),
  "STORE":                  ("03",  ("REG", "REG")),
  "JUMP":                   ("B",   ("REG")),
  "JUMP_EQUAL":             ("C",   ("REG", "REG", "REG")),
  "JUMP_GREATER":           ("D",   ("REG", "REG", "REG")),
  "RANDOM":                 ("04",  ("REG", "REG")),
}


def assemble_arg(arg, argtype):
  arg = arg.strip().lower()

  if argtype == "REG":
    if not arg.startswith("r"):
      raise ValueError("expected 'r' in the beginning of a register")
    val = int(arg[1:])
    if val<0 or val>MAX_REG:
      raise ValueError("register value %d is out of bounds" % val)
    return "%x" % val

  elif argtype == "BYTE":
    val = int(arg, 0)
    if val<0 or val>0xff:
      raise ValueError("byte value %d is out of bounds" % val)
    return "%02x" % val

def assemble_op(line):
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
      asm_args += assemble_arg(args[i], g_ops[op][1][i])
  except ValueError, e:
    raise ValueError("error in args of %s: %s" % (op, e.message))
  
  result = g_ops[op][0] + "".join(asm_args)
  result = result.decode('hex')
  
  return result

