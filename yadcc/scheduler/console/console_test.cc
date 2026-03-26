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

#include <sstream>
#include <string>

#include "gtest/gtest.h"

#include "yadcc/scheduler/console/node_list_view.h"
#include "yadcc/scheduler/console/node_detail_panel.h"
#include "yadcc/scheduler/console/node_info.h"
#include "yadcc/scheduler/console/render.h"
#include "yadcc/api/scheduler.pb.h"

using namespace std::literals;
using namespace yadcc::scheduler;

namespace {

NodeInfo CreateMockNodeInfo(
    const std::string& location,
    int num_processors,
    int current_load,
    int max_tasks,
    int running_tasks) {
  NodeInfo info;
  info.observed_location = location;
  info.reported_location = location;
  info.num_processors = num_processors;
  info.current_load = current_load;
  info.max_tasks = max_tasks;
  info.running_tasks = running_tasks;
  info.priority = SERVANT_PRIORITY_USER;
  info.version = 8;
  info.memory_available_in_bytes = 16ULL * 1024 * 1024 * 1024;
  info.total_memory_in_bytes = 32ULL * 1024 * 1024 * 1024;
  info.environments.push_back("test-digest");
  return info;
}

}  // namespace

TEST(RenderTest, LoadBarRendering) {
  auto output0 = RenderLoadBar(0, 10);
  auto output3 = RenderLoadBar(3, 10);
  auto output5 = RenderLoadBar(5, 10);
  auto output7 = RenderLoadBar(7, 10);
  auto output8 = RenderLoadBar(8, 10);
  auto output10 = RenderLoadBar(10, 10);

  EXPECT_TRUE(output0.find("0%") != std::string::npos);
  EXPECT_TRUE(output3.find("30%") != std::string::npos);
  EXPECT_TRUE(output5.find("50%") != std::string::npos);
  EXPECT_TRUE(output7.find("70%") != std::string::npos);
  EXPECT_TRUE(output8.find("80%") != std::string::npos);
  EXPECT_TRUE(output10.find("100%") != std::string::npos);
}

TEST(RenderTest, MemoryFormatting) {
  EXPECT_EQ(FormatMemory(1024), "1.00 KB");
  EXPECT_EQ(FormatMemory(1024 * 1024), "1.00 MB");
  EXPECT_EQ(FormatMemory(1024 * 1024 * 1024), "1.00 GB");
}

TEST(RenderTest, DurationFormatting) {
  EXPECT_EQ(FormatDuration(30), "30s");
  EXPECT_EQ(FormatDuration(90), "1m 30s");
  EXPECT_EQ(FormatDuration(3665), "1h 1m");
}

TEST(NodeListViewTest, EmptyList) {
  NodeListView view;
  std::vector<NodeInfo> empty;
  view.SetNodes(empty);

  EXPECT_EQ(view.GetSelectedIndex(), 0);
  std::string output = view.Render();
  EXPECT_TRUE(output.find("Total: 0 nodes") != std::string::npos);
}

TEST(NodeListViewTest, SingleNode) {
  NodeListView view;
  auto node = CreateMockNodeInfo("127.0.0.1:1234", 8, 2, 10, 3);

  std::vector<NodeInfo> nodes;
  nodes.push_back(node);
  view.SetNodes(nodes);

  EXPECT_EQ(view.GetSelectedIndex(), 0);
  std::string output = view.Render();
  EXPECT_TRUE(output.find("127.0.0.1:1234") != std::string::npos);
  EXPECT_TRUE(output.find("3/10") != std::string::npos);
}

TEST(NodeListViewTest, MultiNode) {
  NodeListView view;

  auto node1 = CreateMockNodeInfo("192.168.1.1:1234", 8, 2, 10, 3);
  auto node2 = CreateMockNodeInfo("192.168.1.2:1234", 16, 8, 16, 10);
  auto node3 = CreateMockNodeInfo("192.168.1.3:1234", 4, 1, 8, 2);

  std::vector<NodeInfo> nodes;
  nodes.push_back(node1);
  nodes.push_back(node2);
  nodes.push_back(node3);
  view.SetNodes(nodes);

  EXPECT_EQ(view.GetSelectedIndex(), 0);
  std::string output = view.Render();
  EXPECT_TRUE(output.find("192.168.1.1:1234") != std::string::npos);
  EXPECT_TRUE(output.find("192.168.1.2:1234") != std::string::npos);
  EXPECT_TRUE(output.find("192.168.1.3:1234") != std::string::npos);
  EXPECT_TRUE(output.find("Total: 3 nodes") != std::string::npos);
}

TEST(NodeListViewTest, Selection) {
  NodeListView view;

  auto node1 = CreateMockNodeInfo("node1:1234", 8, 2, 10, 3);
  auto node2 = CreateMockNodeInfo("node2:1234", 8, 2, 10, 3);
  auto node3 = CreateMockNodeInfo("node3:1234", 8, 2, 10, 3);

  std::vector<NodeInfo> nodes;
  nodes.push_back(node1);
  nodes.push_back(node2);
  nodes.push_back(node3);
  view.SetNodes(nodes);

  view.Select(1);
  EXPECT_EQ(view.GetSelectedIndex(), 1);

  view.Select(2);
  EXPECT_EQ(view.GetSelectedIndex(), 2);

  view.Select(99);
  EXPECT_EQ(view.GetSelectedIndex(), 2);

  view.Select(-1);
  EXPECT_EQ(view.GetSelectedIndex(), 0);
}

TEST(NodeListViewTest, Scroll) {
  NodeListView view;
  view.SetVisibleCount(2);

  std::vector<NodeInfo> nodes;
  for (int i = 0; i < 5; ++i) {
    nodes.push_back(CreateMockNodeInfo("node" + std::to_string(i) + ":1234", 8, 2, 10, 3));
  }
  view.SetNodes(nodes);

  std::string output1 = view.Render();
  EXPECT_TRUE(output1.find("node0:1234") != std::string::npos);
  EXPECT_TRUE(output1.find("node1:1234") != std::string::npos);
  EXPECT_TRUE(output1.find("showing 2 nodes") != std::string::npos);

  view.Scroll(1);
  std::string output2 = view.Render();
  EXPECT_TRUE(output2.find("node1:1234") != std::string::npos);
  EXPECT_TRUE(output2.find("node2:1234") != std::string::npos);
}

TEST(NodeDetailPanelTest, NoSelection) {
  NodeDetailPanel panel;
  std::string output = panel.Render();
  EXPECT_TRUE(output.find("No node selected") != std::string::npos);
}

TEST(NodeDetailPanelTest, BasicInfo) {
  auto node = CreateMockNodeInfo("192.168.1.100:8080", 16, 8, 32, 15);
  node.priority = SERVANT_PRIORITY_DEDICATED;
  node.version = 10;

  NodeDetailPanel panel;
  panel.SetNode(&node);

  std::string output = panel.Render();
  EXPECT_TRUE(output.find("192.168.1.100:8080") != std::string::npos);
  EXPECT_TRUE(output.find("Processors: 16") != std::string::npos);
  EXPECT_TRUE(output.find("Current Load: 8") != std::string::npos);
  EXPECT_TRUE(output.find("DEDICATED") != std::string::npos);
  EXPECT_TRUE(output.find("test-digest") != std::string::npos);
}

TEST(NodeDetailPanelTest, BehindNat) {
  NodeInfo node;
  node.observed_location = "10.0.0.1:1234";
  node.reported_location = "192.168.1.1:1234";
  node.is_nat = true;
  node.num_processors = 8;
  node.max_tasks = 10;
  node.running_tasks = 5;
  node.version = 8;

  NodeDetailPanel panel;
  panel.SetNode(&node);

  std::string output = panel.Render();
  EXPECT_TRUE(output.find("WARNING: Behind NAT") != std::string::npos);
}

TEST(IntegrationTest, ConsoleDisplay) {
  std::vector<NodeInfo> nodes;

  NodeInfo node1;
  node1.observed_location = "10.0.0.1:1234";
  node1.reported_location = "10.0.0.1:1234";
  node1.num_processors = 8;
  node1.current_load = 3;
  node1.max_tasks = 16;
  node1.running_tasks = 5;
  node1.priority = SERVANT_PRIORITY_DEDICATED;
  node1.version = 8;
  node1.memory_available_in_bytes = 16ULL * 1024 * 1024 * 1024;
  node1.total_memory_in_bytes = 32ULL * 1024 * 1024 * 1024;
  node1.environments.push_back("gcc-9.0");
  nodes.push_back(node1);

  NodeInfo node2;
  node2.observed_location = "192.168.1.100:5678";
  node2.reported_location = "192.168.1.100:5678";
  node2.num_processors = 16;
  node2.current_load = 12;
  node2.max_tasks = 32;
  node2.running_tasks = 28;
  node2.priority = SERVANT_PRIORITY_USER;
  node2.version = 8;
  node2.memory_available_in_bytes = 8ULL * 1024 * 1024 * 1024;
  node2.total_memory_in_bytes = 64ULL * 1024 * 1024 * 1024;
  node2.environments.push_back("clang-11.0");
  node2.environments.push_back("gcc-10.0");
  nodes.push_back(node2);

  NodeListView list_view;
  list_view.SetNodes(nodes);

  NodeDetailPanel detail_panel;
  auto* selected = list_view.GetSelectedNode();
  if (selected) {
    detail_panel.SetNode(selected);
  }

  std::string list_output = list_view.Render();
  std::string detail_output = detail_panel.Render();

  std::cout << "\n===== Node List =====\n" << list_output << "\n";
  std::cout << "\n===== Node Detail =====\n" << detail_output << "\n";

  EXPECT_TRUE(list_output.find("10.0.0.1:1234") != std::string::npos);
  EXPECT_TRUE(list_output.find("192.168.1.100:5678") != std::string::npos);
  EXPECT_TRUE(list_output.find("Total: 2 nodes") != std::string::npos);
  EXPECT_TRUE(detail_output.find("10.0.0.1:1234") != std::string::npos);
  EXPECT_TRUE(detail_output.find("DEDICATED") != std::string::npos);
}