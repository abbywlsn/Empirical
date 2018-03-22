//  This file is part of Empirical, https://github.com/devosoft/Empirical
//  Copyright (C) Michigan State University, 2018.
//  Released under the MIT Software license; see doc/LICENSE
//
//
//  Comparing the ordered versus unordered versions of IndexMap

#include <array>
#include <ctime>         // For std::clock

#include "tools/IndexMap.h"
#include "tools/UnorderedIndexMap.h"
#include "tools/Random.h"

constexpr size_t NUM_WEIGHTS = 1000;
constexpr size_t NUM_CALLS = 10000;
constexpr double MAX_WEIGHT = 100.0;

int main()
{
  // Prepare some data.
  emp::Random random;
  std::array<double, NUM_WEIGHTS> weights;
  std::array<double, NUM_CALLS> calls;

  size_t total_weight = 0.0;
  for (double & x : weights) {
    x = random.GetDouble(MAX_WEIGHT);
    total_weight += x;
  }
  for (double & x : calls) {
    x = random.GetDouble(total_weight);
  }

  std::clock_t start_time1 = std::clock();

  // Setup this index_map.
  emp::IndexMap index_map(NUM_WEIGHTS);
  for (size_t id = 0; id < NUM_WEIGHTS; id++) index_map[id] = weights[id];

  // Run the tests.

  std::clock_t tot_time1 = std::clock() - start_time1;
  std::cout << "base sum = " << 1
            << ";  time = " << 1000.0 * ((double) tot_time1) / (double) CLOCKS_PER_SEC
            << " ms." << std::endl;




  std::clock_t start_time2 = std::clock();

  // ...

  std::clock_t tot_time2 = std::clock() - start_time2;
  std::cout << "std sum = " << 2
            << ";  time = " << 1000.0 * ((double) tot_time2) / (double) CLOCKS_PER_SEC
            << " ms." << std::endl;


}
