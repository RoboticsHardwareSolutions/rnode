#include "rnode.h"
#include "nn.h"
#include "bus.h"
#include <stdio.h>
#include <string.h>
#include "rnode_sem.h"
#include "rlog.h"

static int node_bind(struct rnode* n, const char* url)
{
    char ipc[30];
    for (int i = 0; i < TOTAL_NODE_QUANTITY; i++)
    {
        int written = snprintf(ipc, sizeof(ipc), "%s%d", url, i);
        if (written < 0 || (size_t) written >= sizeof(ipc))
        {
            RLOG_ERROR("rnode: url '%s' too long for the ipc buffer", url);
            return -1;
        }
        int res = nn_bind(n->sock, ipc);
        if (res >= 0)
        {
            return i;
        }
    }
    return -1;
}

static bool node_connect(struct rnode* n, int num, const char* url)
{
    char ipc[30];
    // connects to every slot below the one just bound (0 .. num-1), building a full mesh.
    // n->eid ends up holding only the id of the last (slot 0) connection: there is no
    // per-endpoint disconnect API, so the intermediate ids are intentionally not kept.
    for (int index = num - 1; index >= 0; index--)
    {
        int written = snprintf(ipc, sizeof(ipc), "%s%d", url, index);
        if (written < 0 || (size_t) written >= sizeof(ipc))
        {
            RLOG_ERROR("rnode: url '%s' too long for the ipc buffer", url);
            return false;
        }
        n->eid = nn_connect(n->sock, ipc);
        if (n->eid < 0)
        {
            return false;
        }
    }
    return true;
}

bool rnode_create(struct rnode* n, const char* url)
{
    if (!node_sem_init())
    {
        return false;
    }
    if (!node_sem_enter_critical())
    {
        return false;
    }
    n->sock = nn_socket(AF_SP, NN_BUS);
    if (n->sock == -1)
    {
        node_sem_leave_critical();
        return false;
    }
    int num = node_bind(n, url);
    if (num < 0)
    {
        nn_close(n->sock);
        node_sem_leave_critical();
        return false;
    }
    if (!node_connect(n, num, url))
    {
        nn_close(n->sock);
        node_sem_leave_critical();
        return false;
    }
    int to = WAIT_MESSAGE_TIMEOUT_MS;
    if (nn_setsockopt(n->sock, NN_SOL_SOCKET, NN_RCVTIMEO, &to, sizeof(to)) < 0)
    {
        nn_close(n->sock);
        node_sem_leave_critical();
        return false;
    }
    node_sem_leave_critical();
    return true;
}

bool rnode_receive(struct rnode* n, void* data, size_t len)
{
    int rc = nn_recv(n->sock, data, len, 0);
    if (rc < 0)
    {
        return false;
    }
    if (rc != (int) len)
    {
        return false;
    }
    return true;
}

bool rnode_send(struct rnode* n, const void* data, size_t len)
{
    int rc = nn_send(n->sock, data, len, 0);
    if (rc < 0)
    {
        return false;
    }
    if (rc != (int) len)
    {
        return false;
    }
    return true;
}

bool rnode_delete(struct rnode* n)
{
    int rc = nn_close(n->sock);
    if ((rc != 0) && (errno != EBADF && errno != ETERM))
    {
        return false;
    }
    return true;
}
