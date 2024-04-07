# Measures table creation times for several perfect hashing libs
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
import time

def remove_file(filename):
  try:
    os.remove(filename)
  except FileNotFoundError:
    pass
  
gperf_template="""\
%%
{}
%%
int main()
{{
  Perfect_Hash ph;
  if(ph.in_word_set("AElig",5)) return TOTAL_KEYWORDS;
  else return -1;
}}
"""

def time_gperf(n):
  remove_file("a.out")
  with open("a.gperf","wb") as fout:
    fout.write(
      bytes(
        gperf_template.format("\n".join(html_entities[0:n])),"ascii"))
  t0=time.time()
  os.system("gperf -I -L C++ a.gperf >a.cpp")
  os.system("g++ -std=c++20 -O3 -DNDEBUG a.cpp -o a.out")
  if os.system("a.out")!=n: # compile- or run-time problem
    return ""
  t1=time.time()
  return t1-t0

def time_compile_cpp(template,elements):
  remove_file("a.out")
  with open("a.cpp","w") as fout:
    fout.write(template.format(",\n".join(elements)))
  t0=time.time()
  os.system(
    "g++ -std=c++20 -O3 -fconstexpr-ops-limit=1000000000 -DNDEBUG "
    "-I../frozen/include -I../mph -I../hd -I{} "
    "a.cpp -o a.out".format(os.environ["BOOST_ROOT"]))
  if os.system("a.out")!=len(elements): # compile- or run-time problem
    return ""
  t1=time.time()
  return t1-t0

with open("html_entities.txt","r") as fin:
  html_entities=list(map(str.rstrip,fin.readlines()))

frozen_template="""\
#include <frozen/string.h>
#include <frozen/unordered_set.h>

static constexpr frozen::string entities[]=
{{
{}
}};

static constexpr auto c=frozen::make_unordered_set(entities);

int main()
{{
  if(c.find("AElig")!=c.end()) return c.size();
  else return -1;
}}
"""

def time_frozen(n):
  return time_compile_cpp(
    frozen_template,
    ["\"{}\"".format(e) for e in html_entities[:n]])

frozen_rt_template="""\
#include <frozen/string.h>
#include <frozen/unordered_set.h>

static frozen::string entities[]=
{{
{}
}};
std::size_t entities_size=sizeof(entities)/sizeof(entities[0]);

int main()
{{
  auto c=frozen::make_unordered_set(entities);
  if(c.find("AElig")!=c.end()) return c.size();
  else return -1;
}}
"""

def time_frozen_rt(n):
  return time_compile_cpp(
    frozen_rt_template,
    ["\"{}\"".format(e) for e in html_entities[:n]])

mph_entities=[
  "mph::pair{{\"{}\",{}}}".format(str(i),i)
  for i in range(2125)]

mph_template="""\
#include <mph>

int main()
{{
  constexpr auto entities=std::array
  {{
  {}
  }};

  if(*mph::hash<entities>(std::string_view("0"))==0) return entities.size();
  else return -1;
}}
"""

def time_mph(n):
  return time_compile_cpp(mph_template,mph_entities[:n])

hd_template="""\
#include <string_view>
#include "hd_perfect_set.hpp"

static constexpr std::string_view entities[]=
{{
{}
}};
std::size_t entities_size=sizeof(entities)/sizeof(entities[0]);

int main()
{{
  hd::perfect_set<std::string_view,hd::mulxp3_string_hash> c(
    &entities[0],&entities[entities_size]);
  if(c.find(std::string_view("AElig"))!=c.end()) return entities_size;
  else return -1;    
}}
"""

def time_hd(n):
  return time_compile_cpp(
    hd_template,
    ["\"{}\"".format(e) for e in html_entities[:n]])

hd_constexpr_template="""\
#include <string_view>
#include "hd_constexpr_perfect_set.hpp"

constexpr auto entities=std::array
{{
{}
}};

int main()
{{
  constexpr hd::constexpr_perfect_set<
    std::string_view,entities.size(),hd::mulxp3_string_hash
  > c(entities);
  
  if(c.find(std::string_view("AElig"))!=c.end()) return entities.size();
  else return -1;    
}}
"""

def time_hd_constexpr(n):
  return time_compile_cpp(
    hd_constexpr_template,
    ["std::string_view(\"{}\")".format(e) for e in html_entities[:n]])

sys.stdout.write("n;gperf;frozen;frozen_rt;mph;hd;hd_constexpr\n")
for n in range(25,2125+25,25):
  sys.stdout.write("{};{};{};{};{};{};{}\n".format(
    n,
    time_gperf(n),
    time_frozen(n),
    time_frozen_rt(n) if n<=1000 else "", # run-time crash
    time_mph(n) if n<=100 else "",        # compile-time crash
    time_hd(n),
    time_hd_constexpr(n)))
