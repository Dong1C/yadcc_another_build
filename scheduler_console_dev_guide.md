# Yadcc Scheduler 控制面板开发指南

## 1. 现有可直接使用的指标分析

### 1.1 ServantPersonality 结构体指标

位于 [task_dispatcher.h#L64-84](file:///home/doc/yadcc/yadcc/scheduler/task_dispatcher.h#L64-84):

| 指标名称 | 类型 | 说明 |
|---------|------|------|
| `version` | int | Daemon版本 |
| `observed_location` | string | 服务节点IP:端口（调度器观察到的） |
| `reported_location` | string | 服务节点上报的IP:端口（用于NAT检测） |
| `environments` | vector<EnvironmentDesc> | 支持的编译器环境列表 |
| `num_processors` | size_t | CPU核心数 |
| `current_load` | size_t | 当前负载均值 |
| `total_memory_in_bytes` | size_t | 总内存 |
| `memory_available_in_bytes` | size_t | 可用内存 |
| `max_tasks` | size_t | 最大并发任务数 |
| `priority` | ServantPriority | 优先级（DEDICATED=1, USER=2） |
| `not_accepting_task_reason` | NotAcceptingTaskReason | 拒绝任务原因 |

### 1.2 ServantDesc 内部指标

位于 [task_dispatcher.h#L186-191](file:///home/doc/yadcc/yadcc/scheduler/task_dispatcher.h#L186-191):

| 指标名称 | 类型 | 说明 |
|---------|------|------|
| `running_tasks` | size_t | 当前运行任务数 |
| `ever_assigned_tasks` | size_t | 累计分配任务数 |
| `discovered_at` | time_point | 节点发现时间 |
| `expires_at` | time_point | 心跳过期时间 |

### 1.3 TaskRegistry 任务指标

| 指标名称 | 类型 | 说明 |
|---------|------|------|
| `next_task_id` | atomic<uint64_t> | 下一个任务ID |
| `tasks` | unordered_map | 所有任务描述 |

### 1.4 TaskDesc 任务详情

| 指标名称 | 类型 | 说明 |
|---------|------|------|
| `task_id` | uint64_t | 任务ID |
| `requestor_ip` | string | 请求者IP |
| `started_at` | time_point | 任务开始时间 |
| `expires_at` | time_point | 任务过期时间 |
| `is_prefetch` | bool | 是否为预取任务 |
| `zombie` | bool | 是否为僵尸任务 |

### 1.5 集群级别统计（来自 DumpInternals）

位于 [task_dispatcher.cc#L520-540](file:///home/doc/yadcc/yadcc/scheduler/task_dispatcher.cc#L520-540):

```cpp
jsv["servants_up"]        // 在线节点数
jsv["running_tasks"]      // 集群运行任务总数
jsv["capacity"]           // 集群总容量
jsv["capacity_available"] // 可用容量
jsv["capacity_unavailable"] // 不可用容量
```

### 1.6 RunningTaskBookkeeper 分布式任务追踪

位于 [running_task_bookkeeper.h](file:///home/doc/yadcc/yadcc/scheduler/running_task_bookkeeper.h):

```cpp
void SetServantRunningTasks(const string& servant_location, vector<RunningTask> tasks);
void DropServant(const string& servant_location);
vector<RunningTask> GetRunningTasks() const;
```

---

## 2. 控制面板功能分析

### 2.1 必须具备的功能

#### 2.1.1 节点状态显示
- [ ] **节点列表展示**：分页/滚动显示所有在线节点
- [ ] **节点数量统计**：在线节点数、离线节点数
- [ ] **负载情况可视化**：使用进度条或颜色标识负载等级
  - 负载 < 30%: 绿色
  - 负载 30-70%: 黄色
  - 负载 > 70%: 红色

#### 2.1.2 节点详细信息
- [ ] **IP:端口显示**：observed_location 和 reported_location
- [ ] **任务数量**：running_tasks / max_tasks
- [ ] **资源使用**：CPU核心数、当前负载、内存使用率
- [ ] **优先级标识**：DEDICATED / USER
- [ ] **编译器环境**：支持的 compiler_digest 列表
- [ ] **运行时长**：discovered_at 至今

#### 2.1.3 节点控制
- [ ] **启用/禁用节点连接**：通过修改节点的 `max_tasks` 或 `not_accepting_task_reason`
- [ ] **强制下线节点**：触发节点过期
- [ ] **设置节点权重**：影响任务分配优先级

#### 2.1.4 任务分发控制
- [ ] **手动设置任务分发量**：调整集群总体分发策略
- [ ] **设置黑白名单**：IP或编译器类型过滤
- [ ] **调整任务超时时间**：expires_in 参数

### 2.2 建议具备的功能

- [ ] **实时告警**：节点离线、负载过高、内存不足
- [ ] **历史趋势图**：任务数量、资源使用变化
- [ ] **日志查看**：快速查看调度日志
- [ ] **快捷键支持**：键盘导航操作

---

## 3. 控制面板技术选型

### 3.1 推荐方案：ncurses + 自定义TUI框架

**优点**：
- 轻量级，无需额外图形依赖
- 跨平台支持好（Linux/Unix）
- 与 flare fiber 线程模型兼容
- 可实现高性能实时刷新

**缺点**：
- 开发效率较低，需要手动处理渲染
- 颜色和布局支持有限

### 3.2 备选方案对比

| 库 | 优点 | 缺点 | 推荐度 |
|----|------|------|--------|
| **ncurses** | 轻量、成熟、跨平台 | API原始 | ⭐⭐⭐⭐⭐ |
| **PDCurses** | ncurses兼容 Windows支持 | 社区较小 | ⭐⭐⭐ |
| **boost::ui** | 现代C++风格 | 依赖复杂 | ⭐⭐ |
| **Qt Widgets** | 功能强大 | 过重、不适合终端 | ⭐ |

### 3.3 架构建议

```
┌─────────────────────────────────────────┐
│           Scheduler Control Panel       │
├─────────────────────────────────────────┤
│  ┌─────────────┐  ┌──────────────────┐  │
│  │  节点列表    │  │   节点详情面板    │  │
│  │  (左侧菜单)  │  │   (右侧主区域)    │  │
│  │             │  │                   │  │
│  │  - node1   │  │  IP: 192.168.1.1  │  │
│  │  - node2   │  │  Port: 3030      │  │
│  │  - node3   │  │  Load: 45%       │  │
│  │             │  │  Tasks: 3/8      │  │
│  └─────────────┘  └──────────────────┘  │
├─────────────────────────────────────────┤
│  [F1帮助] [F2过滤] [F3控制] [Q退出]      │
└─────────────────────────────────────────┘
```

---

## 4. 开发指南（细分任务）

### 阶段一：基础设施搭建

#### 任务 1.1：创建控制面板模块结构
```
yadcc/
└── scheduler/
    └── console/
        ├── BUILD
        ├── console.h           # 主控制面板类
        ├── console.cc
        ├── render.h            # 渲染相关
        ├── render.cc
        ├── node_view.h         # 节点视图
        ├── node_view.cc
        ├── task_view.h         # 任务视图
        └── task_view.cc
```

#### 任务 1.2：集成 ncurses 库
- 在 `BLADE_ROOT` 或 `BUILD` 文件中添加 ncurses 依赖
- 创建基础的屏幕初始化和清理类

#### 任务 1.3：创建 SchedulerConsole 主类
```cpp
class SchedulerConsole {
public:
  SchedulerConsole(TaskDispatcher& dispatcher);
  ~SchedulerConsole();
  void Run();  // 主事件循环
  void Stop();
  
private:
  void Render();
  void HandleInput(int key);
  // ...
};
```

---

### 阶段二：节点信息展示

#### 任务 2.1：实现节点列表组件
```cpp
class NodeListView {
public:
  void SetServants(const vector<ServantDesc*>& servants);
  void Select(int index);
  int GetSelectedIndex() const;
  void Scroll(int delta);  // 滚动支持
  
private:
  vector<ServantDesc*> servants_;
  int selected_index_ = 0;
  int scroll_offset_ = 0;
};
```

#### 任务 2.2：实现节点详情面板
```cpp
class NodeDetailPanel {
public:
  void SetServant(const ServantDesc* servant);
  void Clear();
  
private:
  void RenderBasicInfo();
  void RenderResourceUsage();
  void RenderTaskInfo();
};
```

#### 任务 2.3：实现负载可视化
```cpp
std::string RenderLoadBar(std::size_t current_load, std::size_t max_load) {
  int percentage = (current_load * 100) / max_load;
  std::string bar(50, ' ');
  // 根据负载设置颜色和填充
  return bar;
}
```

---

### 阶段三：节点控制功能

#### 任务 3.1：实现节点启用/禁用
```cpp
// 在 TaskDispatcher 中添加控制接口
void SetServantEnabled(const std::string& location, bool enabled);
// 内部实现：修改 max_tasks 或添加特殊标记
```

#### 任务 3.2：实现节点强制下线
```cpp
void ForceOfflineServant(const std::string& location);
// 内部实现：设置 expires_at 为当前时间，触发 OnExpirationTimer 清理
```

#### 任务 3.3：实现任务分发参数调整
```cpp
// 添加全局分发策略控制
void SetDispatchWeight(const std::string& location, double weight);
void SetGlobalThrottle(double factor);  // 0.0-1.0
```

---

### 阶段四：交互优化

#### 任务 4.1：添加键盘快捷键
| 按键 | 功能 |
|------|------|
| `↑/↓` | 选择节点 |
| `Enter` | 查看详情 |
| `d` | 禁用节点 |
| `e` | 启用节点 |
| `k` | 强制下线 |
| `f` | 过滤 |
| `q` | 退出 |
| `F1` | 帮助 |

#### 任务 4.2：添加实时刷新
```cpp
// 使用 fiber 定时器实现每秒刷新
flare::fiber::SetTimer(1s, [&] { Render(); });
```

#### 任务 4.3：添加日志面板
```cpp
class LogPanel {
public:
  void Append(const std::string& message);
  void Render();
  
private:
  std::deque<std::string> logs_;
  int max_lines_ = 10;
};
```

---

### 阶段五：集成与测试

#### 任务 5.1：集成到 scheduler 主程序
```cpp
// 在 scheduler_main.cc 中添加 --console 选项
DEFINE_bool(console, false, "启动控制面板模式");
```

#### 任务 5.2：添加单元测试
```cpp
TEST(ConsoleTest, NodeListScrolling) { ... }
TEST(ConsoleTest, NodeSelection) { ... }
TEST(ConsoleTest, LoadBarRendering) { ... }
```

#### 任务 5.3：端到端测试
- 启动 scheduler + 控制面板
- 模拟 daemon 心跳
- 验证控制和显示功能

---

## 5. 关键代码位置参考

| 功能 | 文件位置 |
|------|----------|
| 节点信息结构体 | [task_dispatcher.h#L64-84](file:///home/doc/yadcc/yadcc/scheduler/task_dispatcher.h#L64-84) |
| 容量计算 | [task_dispatcher.cc#L283-312](file:///home/doc/yadcc/yadcc/scheduler/task_dispatcher.cc#L283-312) |
| 任务分配 | [task_dispatcher.cc#L83-145](file:///home/doc/yadcc/yadcc/scheduler/task_dispatcher.cc#L83-145) |
| 内部状态导出 | [task_dispatcher.cc#L520-580](file:///home/doc/yadcc/yadcc/scheduler/task_dispatcher.cc#L520-580) |
| Servant注册 | [task_dispatcher.cc#L193-230](file:///home/doc/yadcc/yadcc/scheduler/task_dispatcher.cc#L193-230) |
| 过期处理 | [task_dispatcher.cc#L470-510](file:///home/doc/yadcc/yadcc/scheduler/task_dispatcher.cc#L470-510) |

---

## 6. 开发优先级建议

| 优先级 | 任务 | 预计工作量 |
|--------|------|-----------|
| P0 | 节点列表 + 详情展示 | 中 |
| P0 | 基本键盘导航 | 小 |
| P1 | 节点启用/禁用 | 中 |
| P1 | 负载可视化 | 小 |
| P2 | 强制下线 | 中 |
| P2 | 实时刷新 | 小 |
| P3 | 日志面板 | 中 |
| P3 | 历史趋势 | 大 |

---

## 7. 注意事项

1. **线程安全**：TaskDispatcher 使用 `allocation_lock_` 保护，所有外部访问需要加锁
2. **flare兼容**：使用 flare::fiber::Mutex 而非 std::mutex
3. **刷新频率**：建议 1-2 秒刷新一次，避免过于频繁影响性能
4. **颜色支持**：检测终端是否支持颜色，自动调整输出
5. **窗口大小**：处理 SIGWINCH 信号，支持窗口 resize