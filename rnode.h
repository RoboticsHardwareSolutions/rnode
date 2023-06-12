#ifndef __NODE_H__
#define __NODE_H__

#include "stdbool.h"
#include "stdio.h"

#define TOTAL_NODE_QUANTITY 100
#define WAIT_MESSAGE_TIMEOUT_MS 2

struct rnode
{
    int sock;
    int eid;
};

bool rnode_create(struct rnode* n, char* url);

bool rnode_receive(struct rnode* n, void* data, size_t len);

bool rnode_send(struct rnode* n, void* data, size_t len);

bool rnode_delete(struct rnode* n);

#endif  //__NODE_H__
