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

MyAllocator::MyAllocator(size_t _basic_block_size, size_t _size) {
    start = std::malloc(_size);
    current_start = (char *)start; // start is a void pointer, needs to be cast to a char pointer
    remaining_memory = _size;
}

MyAllocator::~MyAllocator() {
    std::free(start); // Free all the memory from the salami allocator
}

void* MyAllocator::Malloc(size_t _length) {
    cout << "MyAllocator::Malloc called with length = " << _length << endl;
    if (_length > remaining_memory)
        return NULL;
    void* res = (void *)current_start; // current_start is a char pointer, needs to be cast to a void pointer
    current_start += _length;
    return res; // Return the pointer to the memory
    // return std::malloc(_length);
}

bool MyAllocator::Free(void* _a) {
    cout << "MyAllocator::Free called" << endl;
    //std::free(_a); Salami allocator doesn't free anything
    return true;
}

