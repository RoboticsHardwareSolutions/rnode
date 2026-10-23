#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "rlog.h"

static sem_t*         sem;
static pthread_once_t sem_once    = PTHREAD_ONCE_INIT;
static bool           sem_open_ok = false;

static void sem_open_once(void)
{
#if __APPLE__
    sem = sem_open("/tmp/rbus_sem", O_CREAT, S_IRUSR | S_IWUSR, 1);
#else
    sem = sem_open("/rbus_sem", O_CREAT, S_IRUSR | S_IWUSR, 1);
#endif
    sem_open_ok = (sem != SEM_FAILED);
}

bool node_sem_init(void)
{
    // pthread_once() guarantees sem_open_once() runs exactly once even if several
    // threads call node_sem_init() (via rnode_create()) at the same time.
    pthread_once(&sem_once, sem_open_once);
    if (!sem_open_ok)
    {
        RLOG_ERROR("rnode: sem_open failed: %s", strerror(errno));
    }
    return sem_open_ok;
}

bool node_sem_enter_critical(void)
{
    if (sem_wait(sem) != 0)
    {
        RLOG_ERROR("rnode: sem_wait failed: %s", strerror(errno));
        return false;
    }
    return true;
}

bool node_sem_leave_critical(void)
{
    if (sem_post(sem) != 0)
    {
        RLOG_ERROR("rnode: sem_post failed: %s", strerror(errno));
        return false;
    }
    return true;
}
