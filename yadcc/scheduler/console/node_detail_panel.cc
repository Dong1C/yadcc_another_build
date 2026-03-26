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
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>

#include "yadcc/scheduler/console/node_detail_panel.h"
#include "yadcc/scheduler/console/render.h"
#include "yadcc/api/scheduler.pb.h"

namespace yadcc::scheduler {

NodeDetailPanel::NodeDetailPanel() = default;

void NodeDetailPanel::SetNode(const NodeInfo* node) {
  node_ = node;
}

void NodeDetailPanel::Clear() {
  node_ = nullptr;
}

std::string NodeDetailPanel::Render() const {
  std::ostringstream oss;

  if (!node_) {
    oss << "No node selected.\n";
    return oss.str();
  }

  const auto& node = *node_;

  oss << "=== Node Detail ===\n\n";

  oss << "Location:\n";
  oss << "  Observed: " << node.observed_location << "\n";
  oss << "  Reported: " << node.reported_location << "\n";

  if (node.is_nat) {
    oss << "  [WARNING: Behind NAT?]\n";
  }

  oss << "\nVersion: " << node.version << "\n";

  oss << "\nResources:\n";
  oss << "  Processors: " << node.num_processors << "\n";
  oss << "  Current Load: " << node.current_load << "\n";
  oss << "  Load Bar: " << RenderLoadBar(node.current_load, node.num_processors) << "\n";

  oss << "\nMemory:\n";
  oss << "  Total: " << FormatMemory(node.total_memory_in_bytes) << "\n";
  oss << "  Available: " << FormatMemory(node.memory_available_in_bytes) << "\n";
  double mem_pct = 0.0;
  if (node.total_memory_in_bytes > 0) {
    mem_pct = 100.0 * (1.0 - static_cast<double>(node.memory_available_in_bytes) /
                       static_cast<double>(node.total_memory_in_bytes));
  }
  oss << "  Usage: " << RenderPercentageBar(mem_pct) << "\n";

  oss << "\nTasks:\n";
  oss << "  Running: " << node.running_tasks << "\n";
  oss << "  Max Allowed: " << node.max_tasks << "\n";
  oss << "  Ever Assigned: " << node.ever_assigned_tasks << "\n";

  double task_pct = 0.0;
  if (node.max_tasks > 0) {
    task_pct = 100.0 * node.running_tasks / node.max_tasks;
  }
  oss << "  Utilization: " << RenderPercentageBar(task_pct) << "\n";

  oss << "\nPriority: ";
  switch (node.priority) {
    case SERVANT_PRIORITY_DEDICATED:
      oss << "DEDICATED\n";
      break;
    case SERVANT_PRIORITY_USER:
      oss << "USER\n";
      break;
    default:
      oss << "UNKNOWN\n";
  }

  if (node.max_tasks == 0) {
    oss << "Not Accepting Reason: ";
    switch (node.not_accepting_task_reason) {
      case NOT_ACCEPTING_TASK_REASON_USER_INSTRUCTED:
        oss << "USER_INSTRUCTED\n";
        break;
      case NOT_ACCEPTING_TASK_REASON_POOR_MACHINE:
        oss << "POOR_MACHINE\n";
        break;
      case NOT_ACCEPTING_TASK_REASON_CGROUPS_PRESENT:
        oss << "CGROUPS_PRESENT\n";
        break;
      case NOT_ACCEPTING_TASK_REASON_BEHIND_NAT:
        oss << "BEHIND_NAT\n";
        break;
      default:
        oss << "UNKNOWN\n";
    }
  }

  oss << "\nEnvironments (" << node.environments.size() << "):\n";
  for (const auto& env : node.environments) {
    oss << "  - " << env << "\n";
  }

  return oss.str();
}

}  // namespace yadcc::scheduler