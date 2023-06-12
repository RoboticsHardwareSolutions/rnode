#ifndef __NODE_H__
#define __NODE_H__

#include "stdbool.h"
#include "stdio.h"

#define TOTAL_NODE_QUANTITY 100
#define WAIT_MESSAGE_TIMEOUT_MS 4

struct node
{
    int sock;
    int eid;
};

bool node_create(struct node* n, char* url);

bool node_receive(struct node* n, void* data, size_t len);

bool node_send(struct node* n, void* data, size_t len);

bool node_delete(struct node* n);

#endif  //__NODE_H__
