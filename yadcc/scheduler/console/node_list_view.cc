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
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <string>

#include "yadcc/scheduler/console/node_list_view.h"
#include "yadcc/scheduler/console/render.h"

namespace yadcc::scheduler {

NodeListView::NodeListView() = default;

void NodeListView::SetNodes(const std::vector<NodeInfo>& nodes) {
  nodes_ = nodes;
  if (selected_index_ >= static_cast<int>(nodes_.size())) {
    selected_index_ = nodes_.empty() ? 0 : nodes_.size() - 1;
  }
}

void NodeListView::Select(int index) {
  if (index < 0) {
    selected_index_ = 0;
  } else if (index >= static_cast<int>(nodes_.size())) {
    selected_index_ = nodes_.empty() ? 0 : nodes_.size() - 1;
  } else {
    selected_index_ = index;
  }
}

void NodeListView::Scroll(int delta) {
  int new_offset = scroll_offset_ + delta;
  new_offset = std::max(0, new_offset);
  int max_offset = std::max(0, static_cast<int>(nodes_.size()) - visible_count_);
  scroll_offset_ = std::min(new_offset, max_offset);

  int new_selected = selected_index_ + delta;
  if (new_selected < scroll_offset_) {
    selected_index_ = scroll_offset_;
  } else if (new_selected >= scroll_offset_ + visible_count_) {
    selected_index_ = scroll_offset_ + visible_count_ - 1;
  } else {
    selected_index_ = new_selected;
  }
  selected_index_ = std::max(0, std::min(selected_index_, static_cast<int>(nodes_.size()) - 1));
}

const NodeInfo* NodeListView::GetSelectedNode() const {
  if (selected_index_ >= 0 && selected_index_ < static_cast<int>(nodes_.size())) {
    return &nodes_[selected_index_];
  }
  return nullptr;
}

std::string NodeListView::Render() const {
  std::ostringstream oss;

  oss << "+----+---------------------+--------+---------+--------+--------+\n";
  oss << "| Id | Location            | Tasks  | Load    | Memory | Status |\n";
  oss << "+----+---------------------+--------+---------+--------+--------+\n";

  int end = std::min(scroll_offset_ + visible_count_, static_cast<int>(nodes_.size()));
  for (int i = scroll_offset_; i < end; ++i) {
    const auto& node = nodes_[i];

    std::string marker = (i == selected_index_) ? ">" : " ";
    std::string location = node.observed_location;
    if (location.size() > 19) {
      location = location.substr(0, 16) + "...";
    }

    std::string tasks = std::to_string(node.running_tasks) + "/" +
                        std::to_string(node.max_tasks);

    std::string load_bar = RenderLoadBar(node.current_load, node.num_processors);
    std::string memory = FormatMemory(node.memory_available_in_bytes);

    std::string status;
    if (node.max_tasks == 0) {
      status = "DISABLED";
    } else if (node.running_tasks >= node.max_tasks) {
      status = "BUSY";
    } else {
      status = "OK";
    }

    oss << "|" << marker << std::setw(3) << (i + 1) << " "
        << "|" << std::setw(19) << location << " "
        << "|" << std::setw(7) << tasks << " "
        << "|" << load_bar << " "
        << "|" << std::setw(7) << memory << " "
        << "|" << std::setw(6) << status << " "
        << "|\n";
  }

  oss << "+----+---------------------+--------+---------+--------+--------+\n";
  oss << "Total: " << nodes_.size() << " nodes, showing " << (end - scroll_offset_)
      << " nodes from " << (scroll_offset_ + 1) << " to " << end << "\n";

  return oss.str();
}

}  // namespace yadcc::scheduler