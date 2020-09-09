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
  
  next = nullptr;
  prev = nullptr;
  // You may need to initialize more members here!
}

SegmentHeader::~SegmentHeader() {
  // You may need to add code here.
}

void SegmentHeader::CheckValid() {
  //cout << "Node ptr: " << this << endl;
  if (cookie != COOKIE_VALUE) {
    cout << "INVALID SEGMENT HEADER!!" << endl;
    assert(false);
    // You will need to check with the debugger to see how we got into this
    // predicament.
  }
}

SegmentHeader* SegmentHeader::Next() {
  return this->next;
}


SegmentHeader* SegmentHeader::Prev() {
  return this->prev;
}

void SegmentHeader::SetNext(SegmentHeader* _segment) {
  this->next = _segment;
}


void SegmentHeader::SetPrev(SegmentHeader* _segment) {
  this->prev = _segment;
}


const size_t SegmentHeader::Length() {
  return length;
}

SegmentHeader* SegmentHeader::Split(size_t _length) {
  SegmentHeader* seg_new = new ((void *)((char *)this + _length)) SegmentHeader(this->length - _length);
  this->length = _length;
  return seg_new;
}
 
/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR CLASS FreeList */
/*--------------------------------------------------------------------------*/

FreeList::FreeList() {
  head = nullptr;
}

FreeList::~FreeList() {
  // You may need to add code here.
}

bool FreeList::Add(SegmentHeader * _segment) {
  SegmentHeader* old_head = head;
  head = _segment;
  head->SetNext(old_head);
  head->SetPrev(nullptr);
  if (old_head)
    old_head->SetPrev(head);
  return true;
}

bool FreeList::Remove(SegmentHeader * _segment) {
  SegmentHeader* prev_node = _segment->Prev();
  SegmentHeader* next_node = _segment->Next();

  _segment->SetNext(nullptr);
  _segment->SetPrev(nullptr);

  if (_segment == head && next_node) {
    head = next_node;
    head->SetPrev(nullptr);
  } else if (_segment == head && !next_node) {
    head = nullptr;
  } else if (!next_node) {
    prev_node->SetNext(nullptr);
  } else {
    prev_node->SetNext(next_node);
    next_node->SetPrev(prev_node);
  }

  return true;
}

SegmentHeader* FreeList::Head() {
  return head;
}

void FreeList::pretty_print() {
  cout << "Prev     |   Address   |   Length   |     Next" << endl;
  if (!head) {
    cout << "Segment list is empty." << endl;
  } else if (head && !head->Next()) {
    cout << head->Prev() << " | " << head << " | " << head->Length() << " | " << head->Next() << endl;
    return;
  } else {
    SegmentHeader* current = head;
    while(current != nullptr) {
      cout << current->Prev() << " | " << current << " | " << current->Length() << " | " << current->Next() << endl;
      current = current->Next();
    }
    return;
  }
}