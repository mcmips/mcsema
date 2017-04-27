# quick script to check how many variables from the cfg are lifted into bc

import sys
import os

from llvmcpy.llvm import *

import mcsema_disass.ida.CFG_pb2 as CFG_pb2

# take global name, ir module
# return dict:
#   "global" : g_ir
#   "uses" : []
def get_lifted_global_in_bc(g_cfg, ir):
  g_uses = {} 
  for g_ir in ir.iter_globals():
    if g_ir.get_name() == g_cfg.var.name:
      g_uses["global"] = g_ir
      g_uses["uses"] = []
      for i in g_ir.iter_uses():
        g_uses["uses"].append(i)
  return g_uses

def check_lifted_globals(cfg, ir):
  g_in_cfg = []
  g_in_ir = []

  for g in cfg.global_vars:
    g_in_cfg.append(g)
    g_uses = get_lifted_global_in_bc(g, ir)
    if g_uses:
      g_in_ir.append(g_uses)

  print "Globals in CFG:"
  print [str(g.var.name) for g in g_in_cfg]
  
  print "Globals in IR:"
  print [str(g["global"].get_name()) for g in g_in_ir]
 
  print "Global Uses in IR:"
  for g in g_in_ir:
    print g["global"].get_name() + ":\t" + str(g["uses"])

  print ""
  print "Summary:"
  print "recovered %d global variables; lifted %d to bitcode" % (len(g_in_cfg), len(g_in_ir))

  print "of %d variables lifted to bitcode, %d have uses" % (len(g_in_ir), len(filter(lambda x : len(x["uses"]), g_in_ir)))
  print ""

  return

# TODO
def check_lifted_stackvars(cfg, ir):
  #for f in cfg.internal_funcs:
  #  for v in f.stackvars:
  #    print v.var.name

  #for f in ir.iter_functions():
  #  for bb in f.iter_basic_blocks():
  #    for i in bb.iter_instructions():
  #      #i.dump()
  #      print dir(i)
  return

def check_lifted_vars(cfg, ir):
  check_lifted_globals(cfg, ir)
  check_lifted_stackvars(cfg, ir)
  return

if __name__ == "__main__":
  if len(sys.argv) != 3 or not os.path.exists(sys.argv[1]) or not os.path.exists(sys.argv[2]):
    print "Usage: %s <cfg_filename> <bc_filename>"
    sys.exit()
  
  # parse cfg
  cfg = CFG_pb2.Module()
  cfg.ParseFromString(open(sys.argv[1],'rb').read())

  ir = []
  buffer = create_memory_buffer_with_contents_of_file(sys.argv[2])
  context = get_global_context()
# for function in module.iter_functions():
#     for bb in function.iter_basic_blocks():
#             for instruction in bb.iter_instructions():
#                         instruction.dump()
  
  # read in bitcode module
  if sys.argv[2][-2:] == "bc":
    ir = context.parse_ir(buffer)

  if ir:
    check_lifted_vars(cfg, ir)