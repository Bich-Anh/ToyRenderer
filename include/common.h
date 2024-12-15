#ifndef COMMON_H
#define COMMON_H

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>

// C++ STD using

using std::make_shared;
using std::shared_ptr;

using namespace std::chrono;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility functions

inline double degrees_to_radians(double degrees) { return degrees * pi / 180; }

inline double random_double()
{
  // Returns a random real in [0, 1[
  return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
{
  // Returns a random real in [0, 1[
  return min + (max - min) * random_double();
}

inline int random_int(int min, int max)
{
  // Returns a random integer in [min,max].
  return int(random_double(min, max + 1));
}

std::string generate_filename(std::string name_prefix, std::string extension)
{
  // Get the current time as a time_t object
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);

  // Convert the time_t to tm structure (local time)
  std::tm tm = *std::localtime(&time);

  // Use a stringstream to format the filename
  std::stringstream filename;
  filename
      << name_prefix << "_"
      << std::put_time(
             &tm,
             "%Y-%m-%d_%H-%M-%S")  // Format the time as YYYY-MM-DD_HH-MM-SS
      << "." << extension;

  return filename.str();
}

std::string format_elapsed_time(std::chrono::steady_clock::time_point start,
                                std::chrono::steady_clock::time_point end)
{
  // Calculate the elapsed time
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

  // Convert the duration to hours, minutes, and seconds
  long hours = duration.count() / 3600;
  long minutes = (duration.count() % 3600) / 60;
  long seconds = duration.count() % 60;

  // Format the result as a string
  std::stringstream result;
  result << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2)
         << minutes << ":" << std::setw(2) << seconds;

  return result.str();
}

// Common headers
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif