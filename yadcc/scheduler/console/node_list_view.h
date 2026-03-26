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

#ifndef YADCC_SCHEDULER_CONSOLE_NODE_LIST_VIEW_H_
#define YADCC_SCHEDULER_CONSOLE_NODE_LIST_VIEW_H_

#include <string>
#include <vector>

#include "yadcc/scheduler/console/node_info.h"

namespace yadcc::scheduler {

class NodeListView {
 public:
  NodeListView();

  void SetNodes(const std::vector<NodeInfo>& nodes);

  void Select(int index);
  int GetSelectedIndex() const { return selected_index_; }

  int GetVisibleCount() const { return visible_count_; }
  void SetVisibleCount(int count) { visible_count_ = count; }

  void Scroll(int delta);

  std::string Render() const;

  const NodeInfo* GetSelectedNode() const;

 private:
  std::vector<NodeInfo> nodes_;
  int selected_index_ = 0;
  int scroll_offset_ = 0;
  int visible_count_ = 10;
};

}  // namespace yadcc::scheduler

#endif  // YADCC_SCHEDULER_CONSOLE_NODE_LIST_VIEW_H_