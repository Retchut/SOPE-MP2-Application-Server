#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

/**
 * @brief Records the time the program started in in a global variable
 * @details The variable holds the time in the number of seconds since the epoch
 */
void set_start();

/**
 * @brief Calculates and returns the number of elapsed seconds since the start of the program
 * @return Number of seconds since the start of the program's execution
 */
long int get_elapsed();

/**
 * @brief Returns the current time
 * @details The time comes in seconds since the epoch
 * @return The current time
 */
long int get_time();

#endif /* SRC_UTILS_H_ */