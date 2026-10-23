# rnode

Full-mesh bus for IPC and inter-process communication, built on nanomsg's `NN_BUS` protocol.
Each `rnode_create()` picks a free numbered slot at the given url and connects to every
other node already using that url, so every node can broadcast to all the others.

Example:
```c
#include "rnode.h"

struct rnode node = {0};

typedef struct
{
    uint8_t id;
    uint8_t payload[8];
} message;

int main(void)
{
    if (!rnode_create(&node, "ipc:///tmp/my_bus"))
        return 1;

    message msg = {.id = 1, .payload = {0}};
    rnode_send(&node, &msg, sizeof(msg));

    message received;
    if (rnode_receive(&node, &received, sizeof(received)))
    {
        // handle the message
    }

    rnode_delete(&node);
    return 0;
}
```

Notes:
- `rnode_receive()` only returns true when exactly `len` bytes were read within
  `WAIT_MESSAGE_TIMEOUT_MS`; poll it in a loop.
- up to `TOTAL_NODE_QUANTITY` nodes can share the same url.
