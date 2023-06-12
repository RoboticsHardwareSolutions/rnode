#include "rnode.h"
#include <nng/compat/nanomsg/nn.h>
#include <nng/compat/nanomsg/bus.h>
#include <stdio.h>
#include <string.h>
#include "rnode_sem.h"

int node_bind(struct rnode* n, char* url)
{
    char ipc[30];
    int  i = 0;
    for (; i < TOTAL_NODE_QUANTITY; i++)
    {
        sprintf(ipc, "%s%d", url, i);
        int res = nn_bind(n->sock, ipc);
        if (res >= 0)
        {
            return i;
        }
    }
    return -1;
}

bool node_connect(struct rnode* n, int num, char* url)
{
    char ipc[30];
    int  index = num - 1;
    while (!(index < 0))
    {
        sprintf(ipc, "%s%d", url, index);
        n->eid = nn_connect(n->sock, ipc);
        if (n->eid < 0)
        {
            return false;
        }
        index--;
    }
    return true;
}

bool rnode_create(struct rnode* n, char* url)
{
    if (!node_sem_init())
    {
        return false;
    }
    node_sem_enter_critical();
    n->sock = nn_socket(AF_SP, NN_BUS);
    if (n->sock == -1)
    {
        node_sem_leave_critical();
        return false;
    }
    int num = node_bind(n, url);
    if (num < 0)
    {
        node_sem_leave_critical();
        return false;
    }
    if (!node_connect(n, num, url))
    {
        node_sem_leave_critical();
        return false;
    }
    int to = WAIT_MESSAGE_TIMEOUT_MS;
    if (nn_setsockopt(n->sock, NN_SOL_SOCKET, NN_RCVTIMEO, &to, sizeof(to)) < 0)
    {
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

bool rnode_send(struct rnode* n, void* data, size_t len)
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
