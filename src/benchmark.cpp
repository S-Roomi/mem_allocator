// Compares Allocator against new/delete.
// Usage: ./benchmark <num_allocations>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "allocator.hpp"

struct Node {
    int value;
    Node* next;
};

const size_t POOL_SIZE = 100000;

double time_new_delete(size_t count) {
    auto start = std::chrono::steady_clock::now();

    std::vector<Node*> nodes;
    nodes.reserve(count);

    for (size_t i = 0; i < count; i++) {
        nodes.push_back(new Node{(int)i, nullptr});
    }
    for (Node* node : nodes) {
        delete node;
    }

    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

// Returns -1 if the pool runs out of blocks.
double time_allocator(size_t count) {
    auto start = std::chrono::steady_clock::now();

    Allocator<sizeof(Node), POOL_SIZE> pool;
    std::vector<Node*> nodes;
    nodes.reserve(count);

    for (size_t i = 0; i < count; i++) {
        void* memory = pool.allocate();
        if (memory == nullptr) {
            return -1;
        }
        // Build a Node inside the memory the pool gave us.
        Node* node = new (memory) Node{(int)i, nullptr};
        nodes.push_back(node);
    }
    for (Node* node : nodes) {
        node->~Node();
        pool.deallocate(node);
    }

    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

int main(int argc, char** argv) {
    size_t count = POOL_SIZE;
    if (argc > 1) {
        count = std::atol(argv[1]);
    }
    if (count > POOL_SIZE) {
        count = POOL_SIZE;
    }

    double new_delete_ms = time_new_delete(count);
    double pool_ms = time_allocator(count);

    if (pool_ms < 0) {
        std::cout << "Pool ran out of blocks early. allocate() or deallocate() may have a bug.\n";
        return 1;
    }

    std::cout << "=== Allocator Benchmark ===\n";
    std::cout << "Allocations:      " << count << "\n";
    std::cout << "new/delete:       " << new_delete_ms << " ms\n";
    std::cout << "Allocator:        " << pool_ms << " ms\n";
    std::cout << "Speedup:          " << (new_delete_ms / pool_ms) << "x\n";

    return 0;
}
