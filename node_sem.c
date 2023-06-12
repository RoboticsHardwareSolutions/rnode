#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include "rlog/rlog.h"

sem_t* sem;

bool node_sem_init(void)
{
#if __APPLE__
    sem = sem_open("/tmp/rbus_sem", O_CREAT, S_IRUSR | S_IWUSR, 1);
#else
    sem = sem_open("/rbus_sem", O_CREAT, S_IRUSR | S_IWUSR, 1);
#endif
    if (sem == SEM_FAILED)
    {
        return false;
    }
    return true;
}

void node_sem_enter_critical(void)
{
    if (sem_wait(sem) != 0)
    {
        return;
    }
}

void node_sem_leave_critical(void)
{
    if (sem_post(sem) != 0)
    {
        return;
    }
}

bool node_sem_delete(void)
{
    if (sem_close(sem) != 0)
    {
        return false;
    }
    return true;
}
