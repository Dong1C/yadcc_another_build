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

#ifndef YADCC_SCHEDULER_CONSOLE_RENDER_H_
#define YADCC_SCHEDULER_CONSOLE_RENDER_H_

#include <string>
#include <cstddef>
#include <chrono>

namespace yadcc::scheduler {

std::string RenderLoadBar(std::size_t current_load, std::size_t max_load);

std::string RenderPercentageBar(double percentage);

std::string FormatMemory(std::size_t bytes);

std::string FormatDuration(int seconds);

}  // namespace yadcc::scheduler

#endif  // YADCC_SCHEDULER_CONSOLE_RENDER_H_