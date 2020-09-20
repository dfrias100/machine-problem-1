/* 
 File: my_allocator.cpp
 
 Author: Daniel Frias
 Department of Computer Science
 Texas A&M University
 Date  : 2020/08/26
 
 Modified:
 
 This file contains the implementation of the class MyAllocator.
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cstdlib>
#include "my_allocator.hpp"
#include <cmath>
#include <assert.h>
#include <iostream>

/*--------------------------------------------------------------------------*/
/* NAME SPACES */ 
/*--------------------------------------------------------------------------*/

using namespace std;
/* I know, it's a bad habit, but this is a tiny program anyway... */

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR CLASS MyAllocator */
/*--------------------------------------------------------------------------*/

MyAllocator::MyAllocator(size_t _basic_block_size, size_t _size) : _blk_sz(_basic_block_size) {
    cout << "Constructing allocator..." << endl;

    size_t _num_of_blocks = Fibonacci(ceil(_size / (double) _blk_sz), 0);
    cout << "Blocks requested: " << _num_of_blocks << endl;

    size_t _allocation_size = _blk_sz * _num_of_blocks;
    cout << "Total memory pool size: " << _allocation_size << "B" << endl;

    _list_sz = Fibonacci(_num_of_blocks, 1) + 1;
    cout << "Required FreeList array size: " << _list_sz << " FreeLists" << endl;

    free_lists = vector<FreeList>(_list_sz);
    cout << "Vector size: " << free_lists.size() << endl;

    start = std::malloc(_allocation_size);
    SegmentHeader* init_seg = new (start) SegmentHeader(_allocation_size);
    init_seg->CheckValid();

    free_lists[_list_sz - 1].Add(init_seg);
 
    // free_list.Add(init_seg);
}

MyAllocator::~MyAllocator() {
    std::free(start); // Free all the memory from the allocator
}

/* void* MyAllocator::Malloc(size_t _length) {
    cout << "MyAllocator::Malloc called with length = " << _length << endl; */
    /* Rounding up the length in terms of block sizes */
    /* size_t len = (_length + sizeof(SegmentHeader)) / _blk_sz;
    len *= _blk_sz;
    if (len < _length + sizeof(SegmentHeader)) { */
        /* The requested length may not be divisible by the block size, so the integer division performed will 
        make the length too short once it is multiplied by the block size. */
        /* len = (_length + sizeof(SegmentHeader)) / _blk_sz;
        len++;
        len *= _blk_sz;
    }
    size_t len_blks = Fibonacci(len / _blk_sz, 1);
    //SegmentHeader* seg = free_lists[len_blks].Head();
    SegmentHeader* seg = free_list.Head();
    seg->CheckValid();

    while (seg != nullptr && seg->Length() < len) {
        seg = seg->Next();
        if(seg) // Preventing segfaults if we cannot find a long enough segment; don't bother checking the header of a nullptr
            seg->CheckValid();
    }

    if (seg == nullptr)
        return NULL;

    //free_lists[len_blks].Remove(seg);
    free_list.Remove(seg);

    if (seg->Length() > len) {
        // TODO: Find way to split
        SegmentHeader* seg2 = seg->Split(len);
        seg2->CheckValid();
        free_list.Add(seg2);
    }

    //free_list.pretty_print();
    void* ptr = (void *) ((char *)seg + sizeof(SegmentHeader));
    return ptr;
} */

void* MyAllocator::Malloc(size_t _length) {
    void* ptr;

    cout << "MyAllocator::Malloc called with length = " << _length << endl;
    size_t len = ceil((_length + sizeof(SegmentHeader)) / (double) _blk_sz);
    cout << "Minimum length in blocks: " << len << " blocks" << endl;
    size_t _len_blks = Fibonacci(len, 0);
    cout << "Length needed in blocks: " << _len_blks << " blocks" << endl;

    size_t idx = 0;
    while (idx < _list_sz && (!free_lists[idx].Head() || Fibonacci(idx + 1) < _len_blks))
        idx++;
    if (idx == _list_sz) {
        return NULL;
    }

    SegmentHeader* seg = free_lists[idx].Head();
    free_lists[idx].Remove(seg);

    if ((seg->Length() / _blk_sz) == _len_blks || idx <= 1) {
        seg->SetUsed();
        ptr = (void *) ((char *)seg + sizeof(SegmentHeader));
        return ptr;
    } else { 
        size_t _split_at = _blk_sz * Fibonacci(idx);
        cout << "Splitting at length: " << _split_at << "B" << endl;
        SegmentHeader* seg2 = seg->Split(_split_at);
        free_lists[idx - 1].Add(seg);
        free_lists[idx - 2].Add(seg2);
        ptr = Malloc(len * _blk_sz);
        return ptr;
    }
}

/*bool MyAllocator::Free(void* _a) {
    cout << "MyAllocator::Free called" << endl;
    SegmentHeader* seg = (SegmentHeader*) ((char*)_a - sizeof(SegmentHeader));
    seg->CheckValid();
    size_t idx = Fibonacci(seg->Length() / _blk_sz, 1);
    free_lists[idx].Add(seg);
    //free_list.pretty_print();
    return true;
}*/

bool MyAllocator::Free(void* _a) {
    // TODO: test this garbage code
    cout << "MyAllocator::Free called" << endl;
    SegmentHeader* seg = (SegmentHeader *) ((char*)_a - sizeof(SegmentHeader));
    SegmentHeader* seg2;
    size_t len;
    size_t idx_seg2;

    size_t idx = Fibonacci(seg->Length() / _blk_sz, 1);
    if (idx == _list_sz - 1)  {
        seg->SetFree();
        free_lists[idx].Add(seg);
        return true;
    } else if (seg->GetBuddyType() == BT::LEFT_BUDDY) {
        len = _blk_sz * Fibonacci(idx + 1);
        seg2 = (SegmentHeader *) (len + (char*)_a - sizeof(SegmentHeader));

        idx_seg2 = Fibonacci(seg2->Length() / _blk_sz, 1);

        if (idx_seg2 != idx - 1 || !seg2->IsFree()) {
            seg->SetFree();
            free_lists[idx].Add(seg);
            return true;
        }
    } else if (seg->GetBuddyType() == BT::RIGHT_BUDDY) {
        len = _blk_sz * Fibonacci(idx + 2);
        seg2 = (SegmentHeader *) ((char*)_a - sizeof(SegmentHeader) - len);

        idx_seg2 = Fibonacci(seg2->Length() / _blk_sz, 1);

        if (idx_seg2 != idx + 1 || !seg2->IsFree()) {
            seg->SetFree();
            free_lists[idx].Add(seg);
            return true;
        }
    }

    SegmentHeader* bbseg;
    SegmentHeader* sbseg;
    SegmentHeader* mseg;

    if (seg->Length() > seg2->Length()) {
        bbseg = seg;
        sbseg = seg2;
    } else {
        bbseg = seg2;
        sbseg = seg;
    }

    mseg = bbseg;

    mseg->Coalesce(sbseg);

    mseg->SetBuddyType(sbseg->GetInheritance());
    mseg->SetInheritance(bbseg->GetInheritance());

    void* ptr = (void *) ((char *)mseg + sizeof(SegmentHeader));

    Free(ptr);
}

size_t MyAllocator::Fibonacci(size_t _min_num, bool _ret_idx) {
    size_t f1 = 1;
    size_t f2 = 2;
    size_t fn = 0;
    size_t idx = 1;
    
    // Finding the correct fibonacci number for the requested input
    while (fn < _min_num && _min_num != 1 && _min_num != 2) {
        fn = f1 + f2;
        
        f1 = f2;
        f2 = fn;
        idx++;
    }

    // Returns the index for the intention of accessing a free list or a fibonacci number for memory allocation
    if (_ret_idx) {
        if (_min_num == 1)
            return 0;
        else if (_min_num == 2)
            return 1;
        else
            return idx;
    } else {
        if (_min_num == 1)
            return f1;
        else if (_min_num == 2)
            return f2;
        else
            return fn;
    }
}

size_t MyAllocator::Fibonacci(size_t _input) {
    size_t f1 = 1;
    size_t f2 = 2;
    size_t fn = 0;

    if (_input == 1)
        return f1;
    else if (_input == 2)
        return f2;
    else {
        for (size_t i = 2; i < _input; i++) {
            fn = f1 + f2;

            f1 = f2;
            f2 = fn;
        }
        return fn;
    }
}