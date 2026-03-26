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

#ifndef YADCC_SCHEDULER_CONSOLE_NODE_INFO_H_
#define YADCC_SCHEDULER_CONSOLE_NODE_INFO_H_

#include <string>
#include <vector>
#include <cstddef>

#include <jsoncpp/value.h>

namespace yadcc::scheduler {

struct NodeInfo {
  std::string observed_location;
  std::string reported_location;
  int version = 0;
  size_t num_processors = 0;
  size_t current_load = 0;
  size_t total_memory_in_bytes = 0;
  size_t memory_available_in_bytes = 0;
  size_t max_tasks = 0;
  size_t running_tasks = 0;
  size_t ever_assigned_tasks = 0;
  int priority = 0;
  int not_accepting_task_reason = 0;
  std::vector<std::string> environments;
  bool is_nat = false;
};

std::vector<NodeInfo> CreateNodeInfosFromJson(const Json::Value& json);

std::vector<NodeInfo> FetchAllNodeInfos();

}  // namespace yadcc::scheduler

#endif  // YADCC_SCHEDULER_CONSOLE_NODE_INFO_H_