#pragma once

/**
 * @brief Start timer.
 * 
 * @note Thread-safe.
 */
void timer_start(void);

/**
 * @brief Stop timer and prints elapsed time.
 * 
 * @note Thread-safe.
 */
void timer_stop_and_print(const char *message);