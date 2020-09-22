/* 
 File: my_allocator.cpp
 
 Author: <your name>
 Department of Computer Science
 Texas A&M University
 Date  : <date>
 
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
#include "free_list.hpp"
#include <assert.h>
#include <iostream>
#include <math.h>
#include <vector>

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
    // We don't do anything yet...
    blockSize = _basic_block_size;
    remainingMem = _size;
    size_t totalSize = _basic_block_size;
    while(totalSize < _size) {
    	totalSize += _basic_block_size;
    }
    start = malloc(totalSize);

    //cout << totalSize << endl;


    //cout << "test" << endl;

    /*freeList = FreeList();
    freeList.Add(head);*/

	int fib1 = 1;
	int fib2 = 1;

	while(fib1 < totalSize) {
		freeList.push_back(FreeList());
		int temp = fib1;
		fib1 += fib2;
		fib2 = temp;
	}
    SegmentHeader * head = new (start) SegmentHeader(totalSize, false, nullptr, nullptr, -1);
	freeList.back().Add(head);

	//freeList.printList();
}

MyAllocator::~MyAllocator() {
    // Same here...
}

void* MyAllocator::Malloc(size_t _length) {
    cout << "MyAllocator::Malloc called with length = " << _length << endl;
    // get the size of the object based on blocksize
    double tempLen = _length + sizeof(SegmentHeader);
    size_t len = blockSize;
    while(len < tempLen) {
    	len += blockSize;
    }

    // find the freelist the data can fit inside of
    int fib1 = 1;
	int fib2 = 1;

	while(fib1 < len) {
		int temp = fib1;
		fib1 += fib2;
		fib2 = temp;
	}
	len = fib1;

	//find open freeList that is big enough
	int index = 0;
	while(index < freeList.size() && (freeList.at(index).empty() || Fib(index) < fib1)) {
		index++;
	}
	//fail if outside of vector;
	if(index >= freeList.size()) {
		//fail
		cout << "Out of memory" << endl;
		return nullptr;
	}

	SegmentHeader * seg = freeList.at(index).first();

	if(seg == nullptr) {
		//fail
		cout << "Out of memory" << endl;
		return nullptr;
	}

	freeList.at(index).Remove(seg);
	if(seg->length != len) {
		 //segment is too long
		SegmentHeader * newFreeSeg = seg->split(len);
		freeList.at(index - 1).Add(seg);
		seg->index = index - 1;
		freeList.at(index - 2).Add(newFreeSeg);
		newFreeSeg->index = index - 2;
	}



	/*SegmentHeader * segFree = freeList.first();

	while(segFree != nullptr && segFree->length < len) {
		segFree = segFree->next;
	}

	if(segFree == nullptr) {
		//fail
		return nullptr;
	}

	freeList.Remove(segFree);

	if(segFree->length > len) {
		SegmentHeader * newSegFree = segFree->split(len);
		freeList.Add(newSegFree);
	}
*/

	void * ptr = (void *)((char *)seg + sizeof(SegmentHeader));
	//cout << "Pointer: " << ptr << endl;
	//cout << "ptr index: " << seg->index << endl;
    return ptr;
    //return retStart;
    // This empty implementation just uses C standard library malloc
    //return std::malloc(_length);
}

bool MyAllocator::Free(void* _a) {
    cout << "MyAllocator::Free called" << endl;
	
	//free releases memory back to index but doesn't merge memory
	SegmentHeader * segFree = (SegmentHeader *) ((char *)_a - sizeof(SegmentHeader));
	int index = segFree->index;
	segFree->is_free = true;
	freeList.at(index).Add(segFree);
    // This empty implementation just uses C standard library free
    //std::free(_a);
    return true;
    //return Coalesce(segFree);
}

int MyAllocator::Fib(int idx) {
	int fib0 = 1;
	int fib1 = 2;
	if(idx == 0) {
		return 1;
	}
	if(idx == 1) {
		return 2;
	}
	int index = 1;
	while(index < idx) {
		//cout << "Index: " << index << " Fib: " << fib0 << endl;
		int temp = fib0;
		fib0 += fib1;
		fib1 = temp;
		index++;
	} 
	return fib1;
}


//for some reason I could mot find the header for the buddy segment no matter what I tried
bool MyAllocator::Coalesce(SegmentHeader * seg) {
	SegmentHeader * seg2;
	cout << "seg pointer: " << seg << endl;
	//check which buddy to look for
	if(seg->index == freeList.size() - 2) {
		cout << "no buddy" << endl;
		freeList.at(seg->index + 1).Add(seg);
	}
	else if(seg->buddyType == 0) {
		cout << "left buddy" << endl;
		seg2 = seg + Fib(seg->index);
	}
	else if(seg->buddyType == 1) {
		cout << "right buddy" << endl;
		cout << "right index: " << seg->index << endl;
		//cout << "right fib: " << Fib(seg->index + 1) << endl;
		cout << "seg2 pointer: " << seg + Fib(seg->index + 1) << endl;
		seg2 = seg + Fib(seg->index + 1);
	}
	//cout << seg2->index << endl;
	if(seg2->index - seg->index == 1 && seg2->is_free) {
		cout << "merging" << endl;
		SegmentHeader * bigBuddy = min(seg, seg2);
		SegmentHeader * smallBuddy = max(seg, seg2);
		SegmentHeader * mergedBuddy = bigBuddy;
		mergedBuddy->index = bigBuddy->index + 1;
		mergedBuddy->buddyType = smallBuddy->INH;
		mergedBuddy->INH = bigBuddy->INH;
		cout << "merged" << endl;
		return Coalesce(mergedBuddy);
	}
	else {
		cout << "no free space" << endl;
		freeList.at(seg->index).Add(seg);
		return false;
	}
}
