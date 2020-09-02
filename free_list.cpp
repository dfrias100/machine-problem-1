/* 
    File: free_list.cpp

    Author: Daniel Frias
            Department of Computer Science
            Texas A&M University
    Date  : 2020/08/26

    Modified: 

    This file contains the implementation of the class FreeList.

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <iostream>
#include "free_list.hpp"
#include <cassert>

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
/* FUNCTIONS FOR CLASS SegmentHeader */
/*--------------------------------------------------------------------------*/

SegmentHeader::SegmentHeader(size_t _length, bool _is_free) {
  length = _length;
  is_free = _is_free;
  cookie = COOKIE_VALUE;
  // You may need to initialize more members here!
}

SegmentHeader::~SegmentHeader() {
  // You may need to add code here.
}

void SegmentHeader::CheckValid() {
  if (cookie != COOKIE_VALUE) {
    cout << "INVALID SEGMENT HEADER!!" << endl;
    assert(false);
    // You will need to check with the debugger to see how we got into this
    // predicament.
  }
}

const SegmentHeader* SegmentHeader::Next() {
  return this->next;
}


const SegmentHeader* SegmentHeader::Prev() {
  return this->prev;
}

const size_t SegmentHeader::Length() {
  return this->length;
}

SegmentHeader* SegmentHeader::Split(size_t _length) {
  SegmentHeader* seg_new = new ((void *)((char *)this + _length)) SegmentHeader(this->length - _length, 1);
  this->length = _length;
  return seg_new;
}
 
/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR CLASS FreeList */
/*--------------------------------------------------------------------------*/

FreeList::FreeList() {
  // You will need to add code here!
}

FreeList::~FreeList() {
  // You may need to add code here.
}

bool FreeList::Add(SegmentHeader * _segment) {
  assert(false); // This implementation does nothing, other than abort.
}

bool FreeList::Remove(SegmentHeader * _segment) {
  assert(false); // This implementation does nothing, other than abort.
}

SegmentHeader* FreeList::Head() {
  return head;
}