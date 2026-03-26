// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this
// file except in compliance with the License. You may obtain a copy of the
// License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#include "yadcc/scheduler/console/render.h"

namespace yadcc::scheduler {

std::string RenderLoadBar(std::size_t current_load, std::size_t max_load) {
  if (max_load == 0) {
    return "[N/A]";
  }
  int percentage = static_cast<int>((current_load * 100) / max_load);
  percentage = std::min(100, std::max(0, percentage));

  int bar_width = 20;
  int filled = (bar_width * percentage) / 100;

  std::string color;
  if (percentage < 30) {
    color = "\033[32m";
  } else if (percentage < 70) {
    color = "\033[33m";
  } else {
    color = "\033[31m";
  }
  std::string reset = "\033[0m";

  std::ostringstream oss;
  oss << color << "[" << std::string(filled, '=') << std::string(bar_width - filled, ' ')
      << "]" << reset << " " << percentage << "%";
  return oss.str();
}

std::string RenderPercentageBar(double percentage) {
  percentage = std::min(100.0, std::max(0.0, percentage));
  int bar_width = 20;
  int filled = (bar_width * static_cast<int>(percentage)) / 100;

  std::string color;
  if (percentage < 30) {
    color = "\033[32m";
  } else if (percentage < 70) {
    color = "\033[33m";
  } else {
    color = "\033[31m";
  }
  std::string reset = "\033[0m";

  std::ostringstream oss;
  oss << color << "[" << std::string(filled, '=') << std::string(bar_width - filled, ' ')
      << "]" << reset;
  return oss.str();
}

std::string FormatMemory(std::size_t bytes) {
  const char* units[] = {"B", "KB", "MB", "GB", "TB"};
  int unit_index = 0;
  double size = static_cast<double>(bytes);

  while (size >= 1024.0 && unit_index < 4) {
    size /= 1024.0;
    ++unit_index;
  }

  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << size << " " << units[unit_index];
  return oss.str();
}

std::string FormatDuration(int seconds) {
  if (seconds < 60) {
    return std::to_string(seconds) + "s";
  } else if (seconds < 3600) {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
  } else {
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
  }
}

}  // namespace yadcc::scheduler