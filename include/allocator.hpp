#pragma once

#include <cstddef>
#include <new>

// A pool of BlockCount blocks, each BlockSize bytes.
//
// Free blocks are kept in a linked list. A free block isn't storing
// anything, so we use its first bytes to hold the pointer to the next
// free block. That way the list needs no extra memory.
template <size_t BlockSize, size_t BlockCount>
class Allocator {
public:
    Allocator() {
        storage_ = new char[BlockSize * BlockCount];

        // Link every block to the one after it. The last block points to nullptr.
        for (size_t i = 0; i < BlockCount; i++) {
            FreeNode* block = get_block(i);
            bool is_last_block = (i == BlockCount - 1);

            if (is_last_block) {
                block->next = nullptr;
            } else {
                block->next = get_block(i + 1);
            }
        }

        free_list_ = get_block(0);
    }

    ~Allocator() {
        delete[] storage_;
    }

    Allocator(const Allocator&) = delete;
    Allocator& operator=(const Allocator&) = delete;

    // Returns nullptr when the pool is full.
    void* allocate() {
        if (free_list_ == nullptr) {
            return nullptr;
        }

        FreeNode* block = free_list_;
        free_list_ = block->next;
        return block;
    }

    // ptr must have come from allocate() on this same pool.
    void deallocate(void* ptr) {
        FreeNode* block = static_cast<FreeNode*>(ptr);
        block->next = free_list_;
        free_list_ = block;
    }

private:
    struct FreeNode {
        FreeNode* next;
    };

    static_assert(BlockSize >= sizeof(FreeNode),
                  "BlockSize must be big enough to hold a pointer");

    FreeNode* get_block(size_t index) {
        char* address = storage_ + index * BlockSize;
        return reinterpret_cast<FreeNode*>(address);
    }

    char* storage_ = nullptr;
    FreeNode* free_list_ = nullptr;
};
