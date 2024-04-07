# Measures lookup times times for several perfect hashing libs
#
# Copyright 2024 Joaquin M Lopez Munoz.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# See https://github.com/joaquintides/usingstdcpp2024
# for project home page.

import os
import sys

def remove_file(filename):
  try:
    os.remove(filename)
  except FileNotFoundError:
    pass
  
def compile_and_run_lookup(lookup_header,input_data):
  remove_file("a.out")
  os.system(
    "g++ -std=c++20 -O3 -fconstexpr-ops-limit=1000000000 -DNDEBUG "
    "-I../frozen/include -I../mph -I../hd -I{} "
    "-DLOOKUP_HEADER=\\\"{}\\\" -DINPUT_DATA=\\\"{}\\\" "
    "lookup.cpp -o a.out".format(
      os.environ["BOOST_ROOT"],lookup_header,input_data))
  os.system("a.out")

matrix=[
  ["gperf",       ["lookup_gperf_100.hpp","input_data_100.txt"],
                  ["lookup_gperf_1000.hpp","input_data_1000.txt"]],
  ["frozen",      ["lookup_frozen.hpp","input_data_100.txt"],
                  ["lookup_frozen.hpp","input_data_1000.txt"]],
  ["frozen_rt",   ["lookup_frozen_rt.hpp","input_data_100.txt"],
                  ["lookup_frozen_rt.hpp","input_data_1000.txt"]],
  ["mph",         ["lookup_mph_60.hpp","mph_input_data_60.txt"]],
  ["hd",          ["lookup_hd.hpp","input_data_100.txt"],
                  ["lookup_hd.hpp","input_data_1000.txt"]],
  ["hd_constexpr",["lookup_hd_constexpr.hpp","input_data_100.txt"],
                  ["lookup_hd_constexpr.hpp","input_data_1000.txt"]]]

sys.stdout.write("container;successful;50/50;unsuccessful\n")
for row in matrix:
  sys.stdout.write("{};".format(row[0]))
  sys.stdout.flush()
  for args in row[1:]:
    compile_and_run_lookup(*args)
  sys.stdout.write("\n")

