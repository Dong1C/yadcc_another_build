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

#include <string>
#include <vector>

#include "flare/base/logging.h"

#include "yadcc/scheduler/console/node_info.h"
#include "yadcc/scheduler/task_dispatcher.h"

namespace yadcc::scheduler {

std::vector<NodeInfo> FetchAllNodeInfos() {
  std::vector<NodeInfo> result;

  auto* dispatcher = TaskDispatcher::Instance();
  FLARE_CHECK(dispatcher);

  auto internals = dispatcher->GetClusterStats();
  auto& servants = internals["servants"];

  for (const auto& servant : servants) {
    NodeInfo info;
    info.observed_location = servant["location"].asString();
    if (servant.isMember("observed_location")) {
      info.observed_location = servant["observed_location"].asString();
      info.reported_location = servant["reported_location"].asString();
      info.is_nat = true;
    } else {
      info.observed_location = servant["location"].asString();
      info.reported_location = info.observed_location;
    }
    info.version = servant["version"].asInt();
    info.num_processors = servant["num_processors"].asUInt64();
    info.current_load = servant["current_load"].asUInt64();
    info.total_memory_in_bytes = servant["total_memory_mb"].asUInt64() * 1024 * 1024;
    info.memory_available_in_bytes = servant["memory_available_mb"].asUInt64() * 1024 * 1024;
    info.max_tasks = servant["max_tasks"].asUInt64();
    info.running_tasks = servant["running_tasks"].asUInt64();
    info.ever_assigned_tasks = servant["ever_assigned_tasks"].asUInt64();

    if (servant.isMember("priority")) {
      std::string priority_str = servant["priority"].asString();
      if (priority_str == "SERVANT_PRIORITY_DEDICATED") {
        info.priority = SERVANT_PRIORITY_DEDICATED;
      } else if (priority_str == "SERVANT_PRIORITY_USER") {
        info.priority = SERVANT_PRIORITY_USER;
      }
    }

    if (servant.isMember("not_accepting_task_reason")) {
      std::string reason_str = servant["not_accepting_task_reason"].asString();
      if (reason_str == "NOT_ACCEPTING_TASK_REASON_USER_INSTRUCTED") {
        info.not_accepting_task_reason = NOT_ACCEPTING_TASK_REASON_USER_INSTRUCTED;
      } else if (reason_str == "NOT_ACCEPTING_TASK_REASON_POOR_MACHINE") {
        info.not_accepting_task_reason = NOT_ACCEPTING_TASK_REASON_POOR_MACHINE;
      } else if (reason_str == "NOT_ACCEPTING_TASK_REASON_CGROUPS_PRESENT") {
        info.not_accepting_task_reason = NOT_ACCEPTING_TASK_REASON_CGROUPS_PRESENT;
      } else if (reason_str == "NOT_ACCEPTING_TASK_REASON_BEHIND_NAT") {
        info.not_accepting_task_reason = NOT_ACCEPTING_TASK_REASON_BEHIND_NAT;
      }
    }

    for (const auto& env : servant["environments"]) {
      info.environments.push_back(env.asString());
    }

    result.push_back(info);
  }

  return result;
}

std::vector<NodeInfo> CreateNodeInfosFromJson(const Json::Value& json) {
  std::vector<NodeInfo> result;
  auto& servants = json["servants"];

  for (const auto& servant : servants) {
    NodeInfo info;
    if (servant.isMember("location")) {
      info.observed_location = servant["location"].asString();
      info.reported_location = info.observed_location;
    }
    if (servant.isMember("observed_location")) {
      info.observed_location = servant["observed_location"].asString();
      info.reported_location = servant["reported_location"].asString();
      info.is_nat = true;
    }
    info.version = servant["version"].asInt();
    info.num_processors = servant["num_processors"].asUInt64();
    info.current_load = servant["current_load"].asUInt64();
    info.total_memory_in_bytes = servant["total_memory_mb"].asUInt64() * 1024 * 1024;
    info.memory_available_in_bytes = servant["memory_available_mb"].asUInt64() * 1024 * 1024;
    info.max_tasks = servant["max_tasks"].asUInt64();
    info.running_tasks = servant["running_tasks"].asUInt64();
    info.ever_assigned_tasks = servant["ever_assigned_tasks"].asUInt64();
    result.push_back(info);
  }

  return result;
}

}  // namespace yadcc::scheduler