/* 
 File: memtest.c
 
 Author: R. Bettati
 Department of Computer Science and Engineering
 Texas A&M University
 Date  : 2019/09/02
 
 Test program for the memory allocator in MP1 of CSCE 313.
 This program reads and processes the command-line arguments and then
 exercises the memory allocator by allocating and de-allocating memory
 as part of the recursive invocations in the function 'ackerman', which
 is famous for being very highly recursive.
 */
/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include<iostream>
#include<string.h>
#include<sys/time.h>
#include<assert.h>
#include<stdio.h>
#include<unistd.h>

#include <new>

#include "my_allocator.hpp"

/*--------------------------------------------------------------------------*/
/* NAME SPACES */ 
/*--------------------------------------------------------------------------*/

using namespace std;
// I know, it's a bad habit, but this is a tiny program anyway...

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* GLOBAL DATA */
/*--------------------------------------------------------------------------*/

MyAllocator * ptr_global_allocator = nullptr;
//This is a global pointer to the allocator defined in the main function.
//We use this pointer in the overloaded 'new' and 'delete' operators.

unsigned long num_allocations;
// Global counter to keep track of the number of allocations exercised
// during one test run.


/*--------------------------------------------------------------------------*/
/* HERE WE OVERLOAD THE NEW AND DELETE OPERATORS */
/*--------------------------------------------------------------------------*/

// When in the test program we call new, we use the following code to allocate
// the memory.


//old new operator that fails if allocator doesn't exist
/*void* operator new(std::size_t sz) throw(std::bad_alloc) {
    cout << "global op new called, size = " << sz << endl;
    assert(ptr_global_allocator != nullptr);
    void *ptr = ptr_global_allocator->Malloc(sz);
    //cout << "::new op::" << endl;
    if (ptr)
        return ptr;
    else {
        std::bad_alloc exception;
        throw exception;
    }
}*/

//new new operator that calls std::malloc instead of failing if allocator doesn't exist
void* operator new(std::size_t sz) throw(std::bad_alloc) {
    cout << "global op new called, size = " << sz << endl;
    void * ptr;
    if (ptr_global_allocator == nullptr) {
        ptr = std::malloc(sz);
    } else {
        ptr = ptr_global_allocator->Malloc(sz);
    }
    if (ptr)
        return ptr;
    else {
        std::bad_alloc exception;
        throw exception;
    }
}

void operator delete(void* ptr) throw()
// The use of throw() is deprecated, but clang throws a hissi-fit wihouth it.
{
    cout << "global op delete called" << endl;
    if (ptr_global_allocator == nullptr) {
        std::free(ptr);
    } else {
        ptr_global_allocator->Free(ptr);
    }
};

/*--------------------------------------------------------------------------*/
/* IMPLEMENTATION OF ACKERMAN FUNCTION */
/*--------------------------------------------------------------------------*/

int ackerman(int a, int b) {
    
    //This is the implementation of the Ackerman function.
    // The function itself is very simple (just two recursive calls).
    // We use it to exercise the memory allocator
    // (see "MyAllocator::Malloc" and "MyAllocator::Free").
    // We use two additional calls to "gettimeofday" to measure the elapsed time.
    
    
    //Choose randomly the size of the memory to allocate:
    int to_alloc =  ((2 << (rand() % 19)) * (rand() % 100)) / 100;
    if  (to_alloc < 4) to_alloc = 4;
    
    int result = 0;
    
    // Here we ALLOCATE the memory using our own allocator.
    int * test_array = new int[to_alloc];
    
    num_allocations++;
    
    // Fill the allocated memory with a value.
    // This same value will be used later for testing.
    int val = rand() % 128;
    
    for(int i = 0; i < to_alloc; i++) {
        test_array[i] = val;
    }
    
    // Here we do the Ackerman recursion thing.
    if (a == 0)
        result = b + 1;
    else if (b == 0)
        result = ackerman(a - 1, 1);
    else
        result = ackerman(a - 1, ackerman(a, b - 1));
    
    // We returned from recursion.
    // Let's check if the memory allocated above still has the same value.
    for (int i = 0; i < to_alloc; i++) {
        //cout << "test_arr: " << test_array[i] << " val: " << val << endl;
        if (test_array[i] != val) {
            // The value does not match.
            // It must have been overwritten somewhere. This is very bad.
            cout << "Memory checking error!" << endl;
            assert(false);
        }
    }
    
    // Now we FREE the memory allocated above.
    // Remember: we use our own allocator for that.
    delete[] test_array;
    
    return result;
    // Generally the Ackerman function returns a value.
    // We don't really care what the value is.
}

/*--------------------------------------------------------------------------*/
/* UTILITY FUNCTIONS */
/*--------------------------------------------------------------------------*/

void print_time_diff(const string _label,
                     const struct timeval & _tp1,
                     const struct timeval & _tp2) {
    // Prints to stdout the difference, in seconds and museconds, between
    // two timevals.
    
    cout << _label;
    long sec = _tp2.tv_sec - _tp1.tv_sec;
    long musec = _tp2.tv_usec - _tp1.tv_usec;
    if (musec < 0) {
        musec += 1000000;
        sec--;
    }
    cout << " [sec = " << sec << ", musec = " << musec << "]" << endl;
    
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    
    // This program repeatedly asks for two parameters, 'a' and 'b'.
    // It then constructs an allocatore of class MyAllocator.
    // Once 'a' and 'b', and the allocator are defined, it computes
    // the result of the (highly recursive!) Ackerman function with these two
    // parameters 'a' and 'b'.
    //
    // During every recursion step, it allocates and de-allocates a portion
    // of memory with the use of the memory allocator defined in module
    // "my_allocator.H".
    //
    // Before and after the invocation of the ackerman function, the
    // value of the wallclock is taken, and the elapsed time for the
    // computation of the ackerman function is displayed.
    
    
    // In your version, these parameters are passed as command-line arguments!
    
    size_t block_size = 128; /* 128 bytes -- CHANGE THIS! */
    size_t mem_size = 4000 * block_size; /* 512KB -- CHANGE THIS! */

    int cmd;
    while((cmd = getopt(argc, argv, "b:s:")) != -1) {
        switch(cmd) {
            case 'b':
                block_size = atoi(optarg);
                break;
            case 's':
                mem_size = atoi(optarg);
                break;
        }
    }

    cout << "Block size = " << block_size << endl;
    cout << "Mem size = " << mem_size << endl;

    for (;;) { // Loop forewer, or until we break.
        
        // Create new allocator (gets destroyed automatically at end of iteration)
        //cout << "test" << endl;
        MyAllocator my_allocator(block_size, mem_size);
        
        // Point the global allocator pointer (used in overloaded new and delete)
        // to my_allocator;
        ptr_global_allocator = &my_allocator;
        
        num_allocations = 0;
        
        cout << endl;
        cout << "Please enter parameters 'a' and 'b' to ackerman function." << endl;
        cout << "Note that this function takes a long time to compute" << endl;
        cout << "even for small values. Keep 'a' at or below 3, and 'b'" << endl;
        cout << "at or below 8." << endl;
        cout << "Otherwise, the function takes seemingly forever." << endl;
        cout << "Enter 0 for either 'a' or 'b' in order to exit" << endl << endl;
        
        int a, b; // Parameters for the invocation of the Ackerman function.
        
        cout << "  a = "; cin >> a;
        if (a <= 0) break;
        cout << "  b = "; cin >> b;
        if (b <= 0) break;
        
        cout << "FOO      a = " << a << ", b = " << b << endl;
        
        struct timeval tp_start; // Used to compute elapsed time.
        struct timeval tp_end;   // same here.
        
        assert(gettimeofday(&tp_start, 0) == 0);
        // Assert aborts if there is a problem with gettimeofday.
        // We rather die of a horrible death rather than returning
        // invalid timing information!
        
        int result = ackerman(a, b);
        // We really don't care about the result. It's like Zen:
        // It's the way to there that counts; not the result that we find there.
        
        assert(gettimeofday(&tp_end, 0) == 0);
        // (see above)
        
        cout << "Result of ackerman(" << a << ", " << b << "):" << result << endl;
        
        print_time_diff("Time taken for computation: ", tp_start, tp_end);
        
        cout << "Number of allocate/free cycles: " << num_allocations << endl;
        cout << endl << endl;

        // Inform new/delete that global MyAllocator is or is getting destroyed.
        ptr_global_allocator = nullptr;        
    }
    
    cout << "Reached end of Ackerman program. Thank you for using it" << endl;
    
}


