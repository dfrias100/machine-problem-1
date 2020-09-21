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

    // If the total memory pool divided by block size is not a fibonacci number, we need to make it one
    size_t _num_of_blocks = Fibonacci(ceil(_size / (double) _blk_sz), 0);
    cout << "Blocks requested: " << _num_of_blocks << endl;

    // Computing new allocation size
    size_t _allocation_size = _blk_sz * _num_of_blocks;
    cout << "Total memory pool size: " << _allocation_size << "B" << endl;

    // Calculating the FreeList array size
    _list_sz = Fibonacci(_num_of_blocks, 1) + 1;
    cout << "Required FreeList array size: " << _list_sz << " FreeLists" << endl;

    // Creating the vector of FreeLists
    free_lists = vector<FreeList>(_list_sz);
    cout << "Vector size: " << free_lists.size() << endl;

    // Initializing everything
    start = std::malloc(_allocation_size);
    SegmentHeader* init_seg = new (start) SegmentHeader(_allocation_size);
    init_seg->CheckValid();

    free_lists[_list_sz - 1].Add(init_seg);
}

MyAllocator::~MyAllocator() {
    std::free(start); // Free all the memory from the allocator
}

void* MyAllocator::Malloc(size_t _length) {
    void* ptr;

    cout << "MyAllocator::Malloc called with length = " << _length << endl;
    size_t len = ceil((_length + sizeof(SegmentHeader)) / (double) _blk_sz);
    cout << "Minimum length in blocks: " << len << " blocks" << endl;
    // Need size in blocks to find appropriate FreeList
    size_t _len_blks = Fibonacci(len, 0);
    cout << "Length needed in blocks: " << _len_blks << " blocks" << endl;

    // Finding a suitable FreeList
    size_t idx = 0;
    while (idx < _list_sz && (!free_lists[idx].Head() || Fibonacci(idx + 1) < _len_blks))
        idx++;
    if (idx == _list_sz) {
        return NULL;
    }

    // Removing segment to check it
    SegmentHeader* seg = free_lists[idx].Head();
    free_lists[idx].Remove(seg);

    if ((seg->Length() / _blk_sz) == _len_blks || idx <= 1) {
        /* Base case: if we have a segment exactly big enough or 
           if the segment is 1 or 2 blocks long (Splitting is complicated in this case) */
        seg->SetUsed();
        ptr = (void *) ((char *)seg + sizeof(SegmentHeader));
        return ptr;
    } else { 
        // Need to split at the correct length
        size_t _split_at = _blk_sz * Fibonacci(idx);
        cout << "Splitting at length: " << _split_at << "B" << endl;
        SegmentHeader* seg2 = seg->Split(_split_at);
        seg2->CheckValid();
        // Adding to appropriate freelists, we don't know if we are going to use these yet
        free_lists[idx - 1].Add(seg);
        free_lists[idx - 2].Add(seg2);
        // Recursive call
        ptr = Malloc(len * _blk_sz);
        return ptr;
    }
}

bool MyAllocator::Free(void* _a) {
    /* The majority of the cout statements were just for debugging purposes */
    cout << "MyAllocator::Free called" << endl;
    // Obtaining the segment header
    SegmentHeader* seg = (SegmentHeader *) ((char*)_a - sizeof(SegmentHeader));
    cout << "Obtained seg, checking validity..." << endl;
    seg->CheckValid();

    // Declaring variables if we need them
    SegmentHeader* seg2;
    size_t len;
    size_t idx_seg2;

    // Calculating the inverse fibonacci value to get the index
    cout << "seg length (in blocks): " << seg->Length() / _blk_sz << endl;
    size_t idx = Fibonacci(seg->Length() / _blk_sz, 1);
    cout << "seg idx in FreeList array: " << idx << endl;

    cout << "Buddy type of seg: " << (int) seg->GetBuddyType() << endl;

    if (idx == _list_sz - 1)  {
        /* If the index is the end of the list, then we have the original segment and 
           all the blocks were freed */
        cout << "All heap blocks have been freed." << endl;
        seg->SetFree();
        free_lists[idx].Add(seg);
        return true;
    } else if (seg->GetBuddyType() == BT::LEFT_BUDDY) {
        len = seg->Length();
        cout << "Length of seg2 needs to be: " << Fibonacci(idx) << endl;

        // Obtaining seg2 by shifting the pointer over by the length of seg
        seg2 = (SegmentHeader *) (len + (char*)_a - sizeof(SegmentHeader));
        cout << "Obtained seg2, checking validity..." << endl;
        seg2->CheckValid();

        // Getting what the index of seg2 is
        cout << "Length of seg2 (in blocks): " << seg2->Length() / _blk_sz << endl;
        idx_seg2 = Fibonacci(seg2->Length() / _blk_sz, 1);
        cout << "Index of seg2: " << idx_seg2 << endl;

        cout << "Is seg2 free? " << seg2->IsFree() << endl; 

        // If the segment does not qualify, we just add back to the FreeList
        if (idx_seg2 != idx - 1 || !seg2->IsFree()) {
            cout << "seg2 did not qualify to coalesce, adding seg to FreeList..." << endl;
            seg->SetFree();
            free_lists[idx].Add(seg);
            return true;
        }
        // If the segment DOES qualify, we have to remove it from the FreeList lest we overwrite memory by not removing it
        free_lists[idx - 1].Remove(seg2);
    } else if (seg->GetBuddyType() == BT::RIGHT_BUDDY) {
        // Computing the length of what seg2 SHOULD be
        len = _blk_sz * Fibonacci(idx + 2);
        cout << "Length of seg2 needs to be: " << len  / _blk_sz << endl;
        
        // Obtaining a seg2 to then check whether or not it can be coalesced
        seg2 = (SegmentHeader *) ((char*)_a - sizeof(SegmentHeader) - len);
        cout << "Obtained seg2, checking validity..." << endl;
        seg2->CheckValid();

        // Just computing the index of seg2
        cout << "Length of seg2 (in blocks): " << seg2->Length() / _blk_sz << endl;
        idx_seg2 = Fibonacci(seg2->Length() / _blk_sz, 1);
        cout << "Index of seg2: " << idx_seg2 << endl;

        cout << "Is seg2 free? " << seg2->IsFree() << endl; 

        // See last if branch
        if (idx_seg2 != idx + 1 || !seg2->IsFree()) {
            cout << "seg2 did not qualify to coalesce, adding seg to FreeList..." << endl;
            seg->SetFree();
            free_lists[idx].Add(seg);
            return true;
        }
        free_lists[idx + 1].Remove(seg2);
    }

    // Declaring necessary SH pointers
    SegmentHeader* bbseg;
    SegmentHeader* sbseg;
    SegmentHeader* mseg;

    // Finding the maximum of the two segments
    if (seg->Length() > seg2->Length()) {
        bbseg = seg;
        sbseg = seg2;
    } else {
        bbseg = seg2;
        sbseg = seg;
    }

    // Beginning merge
    mseg = bbseg;

    // Add the two lengths together in mseg
    mseg->Coalesce(sbseg);

    // Restore the buddy information
    mseg->SetBuddyType(sbseg->GetInheritance());
    mseg->SetInheritance(bbseg->GetInheritance());

    // Preparing the pointer to recurse
    void* ptr = (void *) ((char *)mseg + sizeof(SegmentHeader));

    // Start over from step 1; can we coalesce more?
    Free(ptr);
    return true;
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
    // Computes the fibonacci sequence; the function begins at 1. i.e. F*(1) = 1, F*(2) = 2,... 
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