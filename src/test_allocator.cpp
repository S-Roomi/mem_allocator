// Tests for Allocator. Run these before the benchmark.

#include <iostream>
#include <set>

#include "allocator.hpp"

struct Widget {
    int a, b, c;
};

// We should be able to allocate every block, and each one should be different.
bool test_allocate_all_blocks() {
    Allocator<sizeof(Widget), 100> pool;
    std::set<void*> seen;

    for (int i = 0; i < 100; i++) {
        void* p = pool.allocate();
        if (p == nullptr) {
            std::cout << "FAIL: allocate() returned nullptr after " << i << " blocks\n";
            return false;
        }
        if (seen.count(p) > 0) {
            std::cout << "FAIL: got the same block twice\n";
            return false;
        }
        seen.insert(p);
    }

    std::cout << "PASS: allocated 100/100 distinct blocks\n";
    return true;
}

// Once every block is used, allocate() should return nullptr.
bool test_full_allocator_returns_nullptr() {
    Allocator<sizeof(Widget), 4> pool;
    for (int i = 0; i < 4; i++) {
        pool.allocate();
    }

    if (pool.allocate() != nullptr) {
        std::cout << "FAIL: expected nullptr on a full pool, got a pointer\n";
        return false;
    }

    std::cout << "PASS: allocate() returns nullptr when pool is exhausted\n";
    return true;
}

// A block we give back should be handed out again.
bool test_freed_block_is_reused() {
    Allocator<sizeof(Widget), 4> pool;
    void* p1 = pool.allocate();
    pool.deallocate(p1);
    void* p2 = pool.allocate();

    if (p1 != p2) {
        std::cout << "FAIL: expected freed block to be reused (p1=" << p1 << " p2=" << p2 << ")\n";
        return false;
    }

    std::cout << "PASS: freed block was reused\n";
    return true;
}

// We should be able to store an object in a block and read it back.
bool test_store_and_read_object() {
    Allocator<sizeof(Widget), 4> pool;
    void* memory = pool.allocate();
    if (memory == nullptr) {
        std::cout << "FAIL: allocate() returned nullptr unexpectedly\n";
        return false;
    }

    // Build a Widget inside the memory the pool gave us.
    Widget* w = new (memory) Widget{1, 2, 3};
    bool ok = (w->a == 1 && w->b == 2 && w->c == 3);
    w->~Widget();
    pool.deallocate(memory);

    if (!ok) {
        std::cout << "FAIL: placement-new object readback\n";
        return false;
    }

    std::cout << "PASS: placement-new object readback\n";
    return true;
}

int main() {
    bool all_passed = true;

    if (!test_allocate_all_blocks()) all_passed = false;
    if (!test_full_allocator_returns_nullptr()) all_passed = false;
    if (!test_freed_block_is_reused()) all_passed = false;
    if (!test_store_and_read_object()) all_passed = false;

    if (all_passed) {
        std::cout << "\nALL TESTS PASSED\n";
        return 0;
    } else {
        std::cout << "\nSOME TESTS FAILED\n";
        return 1;
    }
}
