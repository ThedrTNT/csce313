/* 
    File: free_list.cpp

    Author: <your name>
            Department of Computer Science
            Texas A&M University
    Date  : <date>

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
#include <assert.h>
#include "free_list.hpp"

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

SegmentHeader::SegmentHeader(size_t _length, bool _is_free, SegmentHeader * _next, SegmentHeader * _prev, int buddy) {
  length = _length;
  is_free = _is_free;
  next = _next;
  prev = _prev;
  cookie = COOKIE_VALUE;
  buddyType = 0;
  INH = -1;
  index = 0;
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

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR CLASS FreeList */
/*--------------------------------------------------------------------------*/

FreeList::FreeList() {
  // You will need to add code here
  head = nullptr;
  tail = nullptr;
}

FreeList::~FreeList() {
  // You may need to add code here.
}

bool FreeList::Add(SegmentHeader * _segment) {
  //assert(false); // This implementation does nothing, other than abort.
  //cout << "seg add: " << _segment->length << endl;
  if(head == nullptr) {
    head = _segment;
    tail = _segment;
    return true;
  }
  if(head == tail) {
    tail = _segment;
    tail->prev = head;
    head->next = tail;
    return true;
  }

  _segment->prev = tail;
  tail->next = _segment;
  tail = _segment;


  //printList();
  return false;
}

bool FreeList::Remove(SegmentHeader * _segment) {
  //assert(false); // This implementation does nothing, other than abort.

  //cout << "Segment being removed: " << _segment->length << endl;

  /*if(head == nullptr || _segment == nullptr) {
    return false;
  }

  if(head == _segment) {
    head = _segment->next;
    head->prev = nullptr;
    return true;
  }

  if(_segment->next != nullptr) {
    _segment->next->prev = _segment->prev;
  }

  if(_segment->prev != nullptr) {
    _segment->prev->next = _segment->next;
  }

  return true;*/

  
  SegmentHeader * curr = head;
  while(curr != _segment && curr != nullptr) {
    curr = curr->next;
  }
  if(curr == nullptr) {
    //cout << "ruh roh" << endl;
    return false;
  }

  SegmentHeader * prevN = curr->prev;
  SegmentHeader * nextN = curr->next;
  
  //if curr is only node in the list
  if(prevN == nullptr && nextN == nullptr) {
    head = nullptr;
    tail = nullptr;
    return true;
  }

  if(prevN == nextN) {
    head = nullptr;
    tail = nullptr;
    return true;
  }

  //if curr is head
  if(prevN == nullptr) {
    head = nextN;
    head->prev = nullptr;
    return true;
  }
  //if curr is tail
  if(nextN == nullptr) {
    tail = prevN;
    tail->next = nullptr;
    return true;
  }

  //cout << "::remove op::" << endl;
  prevN->next = nextN;
  nextN->prev = prevN;

  //delete curr;
  curr = nullptr;
  

  return false;
}

SegmentHeader * FreeList::first() {
  return head;
}

void FreeList::printList() {
  //cout << "test" << endl;
  SegmentHeader * curr = head;
  cout << endl;
  cout << "Printing freelist: " << endl;
  while(curr != nullptr) {
    //cout << "\ttest" << endl;
    cout << "\tCurr length: " << curr->length << endl;
    curr->CheckValid();

    curr = curr->next;
  }
  cout << endl;
}

SegmentHeader * SegmentHeader::split(size_t _length) {
  SegmentHeader * newSegFree = new ((void*)((char*)this + _length)) SegmentHeader(this->length - _length, true, nullptr, nullptr, 0);
  this->length = _length;
  this->is_free = false;
  this->INH = this->buddyType;
  this->buddyType = 1;
  newSegFree->INH = this->INH;
  return newSegFree;
}

bool FreeList::empty() {
  return head==nullptr;
}
