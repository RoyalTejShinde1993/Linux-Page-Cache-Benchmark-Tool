# Linux Page Cache Benchmark Tool

A Linux page-cache benchmark tool written in C++.

## Features

- Buffered IO benchmark
- Direct IO benchmark (O_DIRECT)
- mmap benchmark
- Sequential and random access
- Throughput + latency metrics

## Build

```bash
mkdir build
cd build
cmake ..
make
```

## Create Test File

```bash
dd if=/dev/zero of=test.bin bs=1M count=1024
```

## Run

Buffered IO:
```bash
./pagebench buffered ../test.bin
```

Direct IO:
```bash
./pagebench direct ../test.bin
```

mmap:
```bash
./pagebench mmap ../test.bin
```

## Example Benchmark Results

The following are example runs performed on a 100 MB test file (`testfile.bin`):

```bash
cd /workspaces/Linux-Page-Cache-Benchmark-Tool && fallocate -l 100M testfile.bin && ./build/pagebench buffered testfile.bin

=== Buffered IO ===
Processed: 100 MB
Time: 0.06718 sec
Throughput: 1488.54 MB/s

cd /workspaces/Linux-Page-Cache-Benchmark-Tool && ./build/pagebench mmap testfile.bin

=== mmap IO ===
Processed: 100 MB
Time: 0.00730904 sec
Throughput: 13681.7 MB/s

cd /workspaces/Linux-Page-Cache-Benchmark-Tool && ./build/pagebench direct testfile.bin

=== Direct IO ===
Processed: 100 MB
Time: 0.0466004 sec
Throughput: 2145.91 MB/s
```

---

# Distributed Storage Architecture

This architecture shows a clustered storage system with a coordinator and replicated storage nodes.

```
                 +----------------+
                 |    Client      |
                 +--------+-------+
                          |
                    HTTP/TCP API
                          |
                +---------+----------+
                |      Coordinator   |
                +---------+----------+
                          |
          +---------------+----------------+
          |                                |
     Replicate                        Replicate
          |                                |
+---------+--------+             +---------+--------+
|   Storage Node   |             |   Storage Node   |
+------------------+             +------------------+
| local disk       |             | local disk       |
| metadata index   |             | metadata index   |
| WAL              |             | WAL              |
+------------------+             +------------------+
```

Key components:

- **Client**: issues benchmark or storage requests over HTTP/TCP.
- **Coordinator**: routes requests, manages metadata, and coordinates replication.
- **Storage Nodes**: persist data locally, maintain metadata indexes, and use a write-ahead log (WAL) for durability.
- **Replication**: ensures data resilience and availability across nodes.

This final architecture represents a more production-ready distributed storage design compared to a single-node benchmark runner.
# What This Project Actually Does

Your benchmark tool studies how the Linux kernel handles file IO through the **page cache**.

The Linux page cache is RAM used by the kernel to speed up disk access.

Instead of reading from disk every time:

```text
Application -> Kernel Page Cache -> Disk
```

Linux tries to:

* keep frequently used data in memory
* reduce disk reads
* optimize sequential access
* batch writes

Your project measures this behavior.

---

# Core Linux Concept: Page Cache

Suppose you read a file:

```cpp
read(fd, buffer, 4096);
```

Linux:

1. checks page cache
2. if data exists → fast memory read
3. otherwise → disk read + cache data

So:

```text
First read  -> slow
Second read -> very fast
```

This is the key idea your benchmark demonstrates.

---

# What Your Tool Benchmarks

---

# 1. Buffered IO

File:

```cpp
src/buffered_io.cpp
```

Uses:

```cpp
open()
read()
```

This is NORMAL Linux file IO.

Data goes through page cache.

---

## Flow

```text
Application
		|
read()
		|
Linux Page Cache
		|
Disk
```

---

## What Happens

### First Read

Kernel loads data from disk.

### Second Read

Kernel serves data from RAM.

Huge speed increase.

---

## Why This Matters

You can measure:

* warm cache vs cold cache
* cache hit performance
* Linux readahead behavior

This is fundamental storage engineering knowledge.

---

# 2. Direct IO

File:

```cpp
src/direct_io.cpp
```

Uses:

```cpp
O_DIRECT
```

Example:

```cpp
open(path, O_RDONLY | O_DIRECT);
```

This bypasses page cache.

---

## Flow

```text
Application
		|
Direct Disk Access
```

No kernel caching.

---

## Why Companies Use O_DIRECT

Databases often avoid page cache because:

* they manage memory themselves
* they want predictable latency
* double caching wastes RAM

Examples:

* MySQL
* PostgreSQL
* RocksDB

---

## What You Learn

You compare:

```text
Buffered IO:
	faster warm reads
	less predictable

Direct IO:
	more stable
	bypasses cache
```

This is REAL systems engineering.

---

# 3. mmap Benchmark

File:

```cpp
src/mmap_io.cpp
```

Uses:

```cpp
mmap()
```

Instead of:

```cpp
read()
```

the file becomes memory.

---

## Flow

```text
File -> Virtual Memory Mapping
```

Access:

```cpp
data[i]
```

Linux loads pages lazily.

---

# Major Concept: Page Faults

When accessing mmap pages:

* page not loaded → page fault
* kernel fetches from disk

Your benchmark can measure:

* major page faults
* minor page faults
* memory locality

---

# Sequential vs Random Reads

This is one of the MOST important storage concepts.

---

# Sequential Access

Example:

```text
0MB -> 1MB -> 2MB -> 3MB
```

Linux detects the pattern and performs:

## Readahead

Kernel preloads future pages automatically.

Result:

```text
Very high throughput
```

---

# Random Access

Example:

```text
100MB -> 2MB -> 700MB
```

Linux cannot predict future reads.

Result:

* more seeks
* worse locality
* more cache misses
* lower throughput

---

# Why Recruiters Like This

Because this is how REAL systems behave.

Storage systems care deeply about:

* access patterns
* page cache efficiency
* memory locality
* disk latency

---

# The Most Important Benchmark

## Cold Cache vs Warm Cache

---

# Cold Cache

Before benchmark:

```bash
echo 3 > /proc/sys/vm/drop_caches
```

This clears page cache.

Now reads come from disk.

Slow.

---

# Warm Cache

Run benchmark again.

Now data already exists in RAM.

Fast.

---

# Example Result

```text
Cold cache:
	250 MB/s

Warm cache:
	4.8 GB/s
```

This demonstrates Linux page cache power beautifully.

---

# Why O_DIRECT Requires Alignment

Direct IO bypasses kernel buffering.

So:

* memory alignment matters
* page boundaries matter

That’s why your code uses:

```cpp
posix_memalign()
```

Example:

```cpp
posix_memalign(&buffer, 4096, BUFFER_SIZE);
```

This is real low-level systems programming.

---

# mmap Optimization

Your mmap benchmark:

```cpp
for (size_t i = 0; i < filesize; i += 4096)
```

touches one byte per page.

This intentionally triggers:

* page loading

* virtual memory activity

Excellent for studying:

* page faults
* memory mappings
* kernel VM behavior

---

# Throughput Calculation

Your code computes:

```text
MB/s = total_MB / seconds
```

This is the most common storage metric.

Example:

```text
Processed: 1024 MB
Time: 2 sec

Throughput:
512 MB/s
```

---

# Why This Project Is Strong

Most candidates:

* never touch Linux internals
* never benchmark anything
* never study IO behavior

This project demonstrates:

* systems thinking
* kernel awareness
* performance engineering
* storage fundamentals

That combination is rare.

---

# Advanced Improvements You Can Add

---

# 1. Random IO Benchmark

Add:

```cpp
pread()
```

with random offsets.

Measure:

* random read latency
* cache miss rates

Very valuable.

---

# 2. Multi-threaded Benchmark

Use:

```cpp
std::thread
```

Measure:

* contention
* parallel throughput
* scalability

---

# 3. /proc Metrics

Read:

```text
/proc/meminfo
/proc/vmstat
```

Track:

* page faults
* dirty pages
* cache size

Now your tool becomes MUCH more professional.

---

# 4. perf Integration

Run:

```bash
perf stat ./pagebench buffered test.bin
```

Measure:

* CPU cycles
* cache misses
* instructions
* page faults

Huge Linux engineering signal.

---

# 5. eBPF Tracing

Track:

* block IO latency
* syscall latency
* scheduler delays

Inspired by:

* bpftrace
* bcc

This becomes extremely impressive.

---

# High-Level Architecture

```
+----------------------+                     
| Benchmark Runner     |                     
+----------+-----------+                     
		   |                                 
		   +--> buffered IO test            
		   +--> mmap test                   
		   +--> O_DIRECT test               
		   +--> random access test          
		   +--> sequential test             
		   |                                 
		   v                                 
+----------------------+                     
| Metrics Collector    |                     
+----------------------+                     
		   |                                 
		   +--> latency                      
		   +--> throughput                   
		   +--> page faults                  
		   +--> cache hit ratio              
		   +--> CPU usage                    
```



