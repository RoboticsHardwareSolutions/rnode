#ifndef __NODE_SEM_H__
#define __NODE_SEM_H__

#include "stdbool.h"

/**
 * @brief Lazily open the named semaphore used to serialize rnode_create() across threads
 * and processes. Safe to call from multiple threads concurrently: the actual sem_open()
 * happens exactly once per process. The semaphore is a process-wide resource shared by
 * every rnode and is intentionally never closed; it stays open for the life of the
 * process and the OS reclaims it on exit.
 */
bool node_sem_init(void);

/** @brief Enter the critical section guarding bind+connect. Returns false on failure. */
bool node_sem_enter_critical(void);

/** @brief Leave the critical section guarding bind+connect. Returns false on failure. */
bool node_sem_leave_critical(void);

#endif
