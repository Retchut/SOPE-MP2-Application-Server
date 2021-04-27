#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_

/**
 * @brief Records the time the program started in in a global variable
 * @param duration - Duration of the program run
 * @details The variable holds the time in the number of seconds since the epoch
 */
void setTimer(int duration);

/**
 * @brief Calculates and returns the number of elapsed seconds since the start of the program
 * @return Number of seconds since the start of the program's execution
 */
long int getElapsed();

/**
 * @brief Returns the current time
 * @details The time comes in seconds since the epoch
 * @return The current time
 */
long int getTime();


/**
 * @brief Returns the time reemaining in this program run
 * @details The time comes in seconds
 * @return The current time remaining
 */
long int getRemaining();

#endif /* SRC_TIMER_H_ */