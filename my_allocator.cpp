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
    start = std::malloc(_size);
    SegmentHeader* init_seg = new (start) SegmentHeader(_size);
    init_seg->CheckValid();
    free_list.Add(init_seg);
}

MyAllocator::~MyAllocator() {
    std::free(start); // Free all the memory from the allocator
}

void* MyAllocator::Malloc(size_t _length) {
    cout << "MyAllocator::Malloc called with length = " << _length << endl;
    size_t len = (_length + sizeof(SegmentHeader)) / _blk_sz;
    len *= _blk_sz;
    if (len < _length + sizeof(SegmentHeader)) {
        len = (_length + sizeof(SegmentHeader)) / _blk_sz;
        len++;
        len *= _blk_sz;
    }
    SegmentHeader* seg = free_list.Head();
    seg->CheckValid();

    while (seg != nullptr && seg->Length() < len) {
        seg = seg->Next();
        if(seg)
            seg->CheckValid();
    }

    if (seg == nullptr)
        return NULL;

    free_list.Remove(seg);

    if (seg->Length() > len && len + sizeof(SegmentHeader) < seg->Length()) {
        SegmentHeader* seg2 = seg->Split(len);
        seg2->CheckValid();
        free_list.Add(seg2);
    }
    free_list.pretty_print();
    void* ptr = (void *) ((char *)seg + sizeof(SegmentHeader));
    return ptr;
}

bool MyAllocator::Free(void* _a) {
    cout << "MyAllocator::Free called" << endl;
    SegmentHeader* seg = (SegmentHeader*) ((char*)_a - sizeof(SegmentHeader));
    seg->CheckValid();
    free_list.Add(seg);
    free_list.pretty_print();
    return true;
}

