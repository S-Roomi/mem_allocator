# mem-allocator

A fixed-size memory pool allocator in C++17, benchmarked against `new`/`delete`.

## How it works

The allocator reserves one big buffer up front and splits it into equal-sized blocks.
Free blocks are kept in a linked list. A free block isn't holding any data, so its
first bytes store the pointer to the next free block, and the list needs no extra memory.

- `allocate()` takes the first block off the list, in O(1).
- `deallocate()` puts a block back on the front of the list, in O(1).

```cpp
Allocator<sizeof(Widget), 100> pool;   // 100 blocks, each big enough for a Widget

void* memory = pool.allocate();          // nullptr if the pool is full
Widget* w = new (memory) Widget{1, 2, 3};

w->~Widget();
pool.deallocate(memory);
```

## Files

- `include/allocator.hpp`: the allocator
- `src/test_allocator.cpp`: correctness tests
- `src/benchmark.cpp`: speed comparison with `new`/`delete`

## Build and run

```sh
make test    # run the tests
make bench   # run the benchmark (100,000 allocations)
```

In local runs the allocator is about 6 to 10 times faster than `new`/`delete`.

## To do

- [ ] **Alignment:** round the block size up to a multiple of the pointer alignment.
- [ ] **Reject empty pools:**
- [ ] **Thread safety:** guard `allocate()` and `deallocate()` with a `std::mutex`
- [ ] **STL support:**
