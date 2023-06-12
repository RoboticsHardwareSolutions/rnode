#ifndef __NODE_SEM_H__
#define __NODE_SEM_H__

#include "stdbool.h"

bool node_sem_init(void);
void node_sem_enter_critical(void);
void node_sem_leave_critical(void);
bool node_sem_delete(void);

#endif
