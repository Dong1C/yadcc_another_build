# Yadcc Repository Structure Analysis

## Project Overview

**Yadcc** (Yet Another Distributed C++ Compiler) is Tencent Advertising's self-developed industrial-grade C++ distributed compilation system. It has been operating stably for 8 months in a 1700+ core cluster, compiling 30,000+ target files daily and producing approximately 3-5TB of output.

## Build System: Blade

Yadcc uses **Blade** as its build tool, which is a high-performance build system developed by Tencent.

### Blade Directory Structure

```
/home/doc/yadcc/
├── blade                     # Blade launcher script
├── BLADE_ROOT               # Blade build configuration
├── blade-bin -> build64_release  # Build output symlink
```

### Key Blade Commands

```bash
# Build all targets
./blade build yadcc/...

# Build specific target
./blade build yadcc/scheduler/...

# Clean build
./blade clean
```

### BLADE_ROOT Configuration

Located at project root, contains:
- Global configuration settings
- C++ compiler flags (`cc_config`)
- Test configuration (`cc_test_config`)
- Target architecture settings (x86_64, ppc64le, aarch64)

## Repository Structure

```
/home/doc/yadcc/
├── blade                     # Blade launcher script
├── BLADE_ROOT               # Blade configuration file
├── README.md                # Main project documentation
│
├── flare -> build/external/flare/flare/  # Symlink to flare framework
│
├── flare/                   # Flare RPC framework (submodule)
│   ├── base/                # Base utilities (buffer, threading, future, etc.)
│   ├── doc/                 # Flare documentation
│   ├── example/             # Example code
│   ├── fiber/               # Fiber-based concurrency
│   ├── io/                  # I/O utilities
│   ├── net/                 # Network protocols (HTTP, Redis, HBase)
│   ├── rpc/                 # RPC implementation
│   ├── testing/             # Testing utilities
│   └── tools/               # Tools
│
├── thirdparty/              # Third-party dependencies
│   ├── blake3/              # BLAKE3 cryptographic hash
│   ├── benchmark/           # Google Benchmark
│   ├── curl/                 # HTTP client library
│   ├── glog/                 # Google logging
│   ├── gflags/               # Google command-line flags
│   ├── googletest/           # Google Test framework
│   ├── jemalloc/             # Memory allocator
│   ├── jsoncpp/              # JSON library
│   ├── lz4/                  # Compression library
│   ├── openssl/              # SSL/TLS library
│   ├── protobuf/             # Protocol Buffers
│   ├── snappy/               # Compression library
│   ├── yaml-cpp/             # YAML library
│   ├── zlib/                 # Compression library
│   ├── zstd/                 # Zstd compression
│   ├── xxhash/               # xxHash fast hash
│   └── blade/                # Bundled blade build tool
│
├── yadcc/                    # Main project source code
│   ├── api/                  # Protocol definitions (protobuf)
│   ├── cache/                # Cache server implementation
│   ├── client/               # Client code
│   │   ├── cxx/              # C++ compiler wrapper
│   │   │   ├── libfakeroot/   # Fake root for path normalization
│   │   │   └── BUILD
│   │   ├── common/           # Common client utilities
│   │   │   ├── compress.cc    # Zstd compression
│   │   │   ├── daemon_call.cc # Daemon communication
│   │   │   ├── io.cc          # I/O utilities
│   │   │   └── logging.cc     # Logging
│   │   └── wrapper/           # Java wrapper (for non-distributed builds)
│   ├── common/               # Shared utilities
│   │   ├── dir.cc
│   │   ├── inspect_auth.cc
│   │   ├── io.cc
│   │   └── token_verifier.cc
│   ├── daemon/               # Daemon implementation
│   │   ├── cloud/            # Cloud/remote execution
│   │   │   ├── compiler_registry.cc
│   │   │   ├── daemon_service_impl.cc
│   │   │   ├── distributed_cache_writer.cc
│   │   │   ├── execution_engine.cc
│   │   │   └── remote_task.cc
│   │   ├── local/             # Local task management
│   │   │   ├── distributed_cache_reader.cc
│   │   │   ├── distributed_task_dispatcher.cc
│   │   │   ├── http_service_impl.cc
│   │   │   └── local_task_monitor.cc
│   │   ├── BUILD
│   │   ├── cache_format.cc    # Cache format handling
│   │   ├── entry.cc           # Daemon entry point
│   │   ├── privilege.cc       # Privilege handling
│   │   └── sysinfo.cc        # System information
│   ├── doc/                  # Project documentation
│   │   ├── README.md         # Documentation index
│   │   ├── usage.md          # Usage guide
│   │   ├── rationale.md      # Design rationale
│   │   ├── benchmark.md      # Performance benchmarks
│   │   ├── cache.md          # Cache server docs
│   │   ├── client.md         # Client docs
│   │   ├── daemon.md         # Daemon docs
│   │   ├── scheduler.md      # Scheduler docs
│   │   ├── debugging.md      # Debugging guide
│   │   ├── faq.md            # FAQ
│   │   ├── security-considerations.md
│   │   └── wrapper.md        # Wrapper docs
│   └── scheduler/           # Scheduler implementation
│       ├── BUILD
│       ├── entry.cc          # Scheduler entry point
│       ├── running_task_bookkeeper.cc
│       ├── scheduler_service_impl.cc
│       └── task_dispatcher.cc
│
├── build/                    # Build directory
└── build64_release/         # Build output (symlink)
```

## Component Architecture

### 1. Scheduler (`yadcc-scheduler`)
**Binary**: `yadcc/scheduler/yadcc-scheduler`

Responsibilities:
- Global task scheduling across the compilation cluster
- Maintains cluster capacity view
- Allocates compilation tasks to appropriate daemon nodes
- Manages servant (compile node) registration via heartbeats

Key Files:
- `task_dispatcher.cc` - Core scheduling algorithm
- `scheduler_service_impl.cc` - RPC service implementation
- `running_task_bookkeeper.cc` - Tracks active tasks

### 2. Cache Server (`yadcc-cache`)
**Binary**: `yadcc/cache/yadcc-cache`

Responsibilities:
- Stores compiled artifacts
- L1 (memory) + L2 (disk) cache architecture
- Bloom filter maintenance for cache hit optimization
- ARC algorithm for L1 cache eviction

### 3. Daemon (`yadcc-daemon`)
**Binary**: `yadcc/daemon/yadcc-daemon`

Dual Role:
- **Client-side**: Accepts local compilation requests, coordinates with scheduler and cache
- **Server-side**: Executes remote compilation tasks from other machines

Key Submodules:
- `local/` - Local task management and distributed task dispatching
- `cloud/` - Remote task execution engine, compiler registry

### 4. C++ Client (`yadcc-cxx`)
**Binary**: `yadcc/client/cxx/yadcc-cxx`

Responsibilities:
- Acts as compiler wrapper (gcc/g++ replacement)
- Preprocesses source code
- Computes BLAKE3 digest for cache key
- Compresses preprocessed output with zstd
- Communicates with local daemon

### 5. Wrapper
For Java compilation (non-distributed), provides quota-based concurrency control.

## Key Technologies

### Flare RPC Framework
Tencent's underlying RPC framework providing:
- Fiber-based concurrency
- Protocol support (HTTP, Redis, Protocol Buffers)
- Service discovery
- Connection management

### Hashing & Compression
| Algorithm | Purpose |
|-----------|---------|
| BLAKE3 | Cryptographic hash for cache keys |
| xxHash | Fast non-cryptographic hash |
| zstd | Compression for network transfer |

### Protocols
- Protocol Buffers for serialization
- Custom protocols for daemon-scheduler and daemon-daemon communication

## Build Configuration (BLADE_ROOT)

```python
cc_config(
    cppflags=['-gdwarf-2', '-mcx16' if arch == 'x86_64' else ''],
    cxxflags=['-std=gnu++2a'],
)

cc_test_config(
    dynamic_link=not build_target.is_debug() and build_target.arch != 'ppc64le',
    gtest_libs=['thirdparty/googletest:gtest', '#pthread'],
)
```

## Build Dependencies

### Required System Tools
- GCC 8+ for building
- Git and git-lfs for submodule management

### Build Command
```bash
git clone https://github.com/Tencent/yadcc --recurse-submodules
./blade build yadcc/...
```

## Documentation Structure

### Project Documentation (`yadcc/doc/`)
| Document | Description |
|----------|-------------|
| README.md | Documentation index |
| usage.md | Getting started and deployment |
| rationale.md | Design philosophy and comparison with similar tools |
| benchmark.md | Performance comparison data |
| scheduler.md | Scheduler design and configuration |
| cache.md | Cache server architecture |
| daemon.md | Daemon design and local task management |
| client.md | Client configuration |
| debugging.md | Debugging techniques |
| faq.md | Frequently asked questions |

### Flare Documentation (`flare/doc/`)
Technical documentation for the Flare RPC framework including:
- Async programming
- Fiber scheduling
- RPC protocols
- Logging and monitoring

## Quick Reference

| Component | Binary Name | Default Port |
|-----------|-------------|--------------|
| Scheduler | yadcc-scheduler | 8336 |
| Cache | yadcc-cache | 8337 |
| Daemon | yadcc-daemon | 8334 |

| Environment Variable | Purpose |
|---------------------|---------|
| YADCC_LOG_LEVEL | Logging level (0-5) |
| YADCC_CACHE_CONTROL | Cache behavior (0=disable, 1=enable, 2=write-only) |
| YADCC_DAEMON_PORT | Local daemon port |
| YADCC_WARN_ON_NONCACHEABLE | Warn on non-cacheable source |