/**
 * @file:   MemoryTests.h
 * @brief:  Unit tests for memory request and release.
 */
 
#ifndef _MEMORY_TESTS_
#define _MEMORY_TESTS_

/*
 * Sets test processes to memory tests.
 */
void set_test_procs(void);

/*
 * Memory requester process.
 */
void request(void);

/*
 * Memory releaser process.
 */
void release(void);

/*
 * Process to check and print test results.
 */
void results(void);

/*
 * Filler process 1 (system must have 6 test processes).
 */
void dummy1(void);

/*
 * Filler process 2 (system must have 6 test processes).
 */
void dummy2(void);

/*
 * Filler process 3 (system must have 6 test processes).
 */
void dummy3(void);

#endif /* _MEMORY_TESTS_ */
