/* 
    File: my_allocator.hpp

    Author: Daniel Frias
            Department of Computer Science and Engineering
            Texas A&M University
    Date  : 2020/08/26

    Modified:

*/

#ifndef _my_allocator_hpp_                   // include file only once
#define _my_allocator_hpp_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cstdlib>
#include <vector>
#include "free_list.hpp"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS   MyAllocator */
/*--------------------------------------------------------------------------*/

class MyAllocator {

 private:

  void* start;
  FreeList free_list;
  size_t _blk_sz;
   
  vector<FreeList> free_lists;


 public:
  MyAllocator(size_t _basic_block_size, size_t _size); 
  /* This function initializes the memory allocator and makes a portion of 
     ’_size’ bytes available. The allocator uses a ’_basic_block_size’ as 
     its minimal unit of allocation. 
     NOTE: In the old days one used 'unsigned int' for the size. The C
     Standards Committee has added the 'size_t' data type to ensure to 
     represent memory sizes. This makes the code more portable. Depending
     on the architecture, 'size_t' may or may not be the same 
     as 'unsigned int'. 
  */ 

  ~MyAllocator(); 
  /* This function returns any allocated memory to the operating system. 
  */ 

  void* Malloc(size_t _length);
  /* Allocate _length number of bytes of free memory and returns the 
     address of the allocated portion. Returns nullptr when out of memory. 
  */ 

  bool Free(void* _a); 
  /* Frees the section of physical memory previously allocated 
     using ’Malloc’. Returns true if everything ok. */ 

  size_t Fibonacci(size_t _min_num, bool _ret_idx);
  /* Returns a fibonacci number greater than the minimum number provided, 
     or the index to a fibbonaci number (for FreeList array) */
};

#endif 
