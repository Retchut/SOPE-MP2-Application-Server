// COPYRIGHT 2021 Flávio Lobo Vaz, José Costa, Mário Travassos, Tomás Fidalgo

#include "../src/timer.h"

#include <stdint.h>
#include <time.h>

int64_t start;
int64_t end;

/**
 * \brief Get current time
 * \param 
 * \return current time
 */
int64_t getTime() { return time(NULL); }

/**
 * \brief set variable end with new time
 * \param 
 * \return 
 */
void setTimer(int duration) {
  start = time(NULL);
  end = start + duration;
}
/**
 * \brief Get passed time of our program
 * \param 
 * \return passed time
 */
int64_t getElapsed() { return getTime() - start; }

/**
 * \brief Get remaining time of our program
 * \param 
 * \return remaining time of our program
 */
int64_t getRemaining() { return end - getTime(); }
