/**
 * @file:   StressTests.h
 * @brief:  Unit tests that test the system's performance on limited resources.
 */
 
#ifndef _STRESS_TESTS_
#define _STRESS_TESTS_

/*
 * Sets system stress test processes.
 */
void setStressTestProcesses(void);

/*
 * Requests all the memory in the system upon triggering the stress tests.
 */
void stressTestA(void);

/*
 * Forwards messages from stressTestA to stressTestC.
 */
void stressTestB(void);

/*
 * Receives messages and hibernates before releasing memory at regular
 * intervals.
 */
void stressTestC(void);

#endif /* _STRESS_TESTS_ */
