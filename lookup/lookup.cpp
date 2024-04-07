/* Measures lookup times times for several perfect hashing libs
 *
 * Copyright 2024 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See https://github.com/joaquintides/usingstdcpp2024
 * for project home page.
 */

#include <algorithm>
#include <array>
#include <chrono>
#include <numeric>

std::chrono::high_resolution_clock::time_point measure_start,measure_pause;

template<typename F>
double measure(F f)
{
  using namespace std::chrono;

  static const int              num_trials=10;
  static const milliseconds     min_time_per_trial(200);
  std::array<double,num_trials> trials;

  for(int i=0;i<num_trials;++i){
    int                               runs=0;
    high_resolution_clock::time_point t2;
    volatile decltype(f())            res; /* to avoid optimizing f() away */

    measure_start=high_resolution_clock::now();
    do{
      res=f();
      ++runs;
      t2=high_resolution_clock::now();
    }while(t2-measure_start<min_time_per_trial);
    trials[i]=duration_cast<duration<double>>(t2-measure_start).count()/runs;
  }

  std::sort(trials.begin(),trials.end());
  return std::accumulate(
    trials.begin()+2,trials.end()-2,0.0)/(trials.size()-4);
}

void pause_timing()
{
  measure_pause=std::chrono::high_resolution_clock::now();
}

void resume_timing()
{
  measure_start+=std::chrono::high_resolution_clock::now()-measure_pause;
}

#include <iostream>
#include <random>
#include <vector>

#include LOOKUP_HEADER

static const char* data[]=
{
#include INPUT_DATA
};
static std::size_t data_size=sizeof(data)/sizeof(data[0]);

int main()
{
  std::vector<std::string> input;
  for(int i=0;i<10;++i){
    input.insert(input.end(),&data[0],&data[data_size]);
  }
  std::mt19937 rng(2321);
  std::shuffle(input.begin(),input.end(),rng);

  auto lookup=[&]{
    std::size_t res=0;
    for(const auto& x:input){
      if(found(x))++res;
    }
    return res;
  };

  std::cout<<measure(lookup)*1.E9/input.size()<<";";

  std::discrete_distribution<> d({50,50});
  for(auto& x:input){
    if(d(rng)) x[x.size()/2]='*';
  }
  std::cout<<measure(lookup)*1.E9/input.size()<<";";

  for(auto& x:input){
    x[x.size()/2]='*';
  }
  std::cout<<measure(lookup)*1.E9/input.size()<<";";
}