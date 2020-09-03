/* 
    File: free_list.hpp

    Author: Daniel Frias
            Department of Computer Science and Engineering
            Texas A&M University
    Date  : 2020/08/26

    Modified:

*/

#ifndef _free_list_hpp_                   // include file only once
#define _free_list_hpp_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cstdlib>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

class SegmentHeader {

private:

  static const unsigned COOKIE_VALUE = 0xBADB00;
  unsigned int cookie; /* To check whether this is a genuine header! */
  size_t length;
  bool is_free;
  
  SegmentHeader* next;
  SegmentHeader* prev;

  // You will need additional data here!
  
public:

  SegmentHeader(size_t _length, bool _is_free = true);
  
  ~SegmentHeader();
  /* We probably won't need the destructor. */

  void CheckValid();
  /* Check if the cookie is valid. */

  const size_t Length();
  /* Return the length of the segment */

  SegmentHeader* Next();
  /* Return the pointer to the next segment. */

  SegmentHeader* Prev();
  /* Return the pointer to the previous segment. */

  SegmentHeader* Split(size_t _length);
  /* Return a pointer to a segment split at length. */

  void SetNext(SegmentHeader* _segment);
  /* Sets the next pointer. */

  void SetPrev(SegmentHeader* _segment);
  /* Sets the prev pointer. */
};

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CLASS  FreeList */
/*--------------------------------------------------------------------------*/

class FreeList {

 private:

  /* Here you add whatever private members you need...*/
  SegmentHeader* head;

public:

  FreeList(); 
  /* This function initializes a new free-list. */

  ~FreeList(); 
  /* We probably don't need a destructor. */ 

  bool Remove(SegmentHeader * _segment); 
  /* Remove the given segment from the given free list. 
     Returns true if the function succeeds.
  */ 

  bool Add(SegmentHeader * _segment); 
  /* Add the segment to the given free list. */
  
  SegmentHeader* Head();
  /* Return a pointer to the head of the free list. */

};

#endif 
