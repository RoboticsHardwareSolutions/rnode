#ifndef __NODE_H__
#define __NODE_H__

#include "stdbool.h"
#include "stdio.h"

/** maximum number of nodes that can share the same url (slots 0 .. TOTAL_NODE_QUANTITY-1) */
#define TOTAL_NODE_QUANTITY 100
/** receive timeout (ms) applied to every node's socket, see rnode_receive() */
#define WAIT_MESSAGE_TIMEOUT_MS 2

struct rnode
{
    int sock;
    int eid;
};

/**
 * @brief Create a node and join the full-mesh bus at the given url.
 * Internally picks the first free slot N by binding "<url>0", "<url>1", ... and then
 * connects to every lower-numbered slot "<url>0" .. "<url>(N-1)" already bound by other nodes.
 * @param url base address (e.g. "inproc://node" or "ipc:///tmp/node"); a slot number is appended to it,
 * so it must be short enough to leave room for the digits within the internal address buffer.
 * @return false if all slots are taken, the url is too long, or the underlying socket could not be set up.
 */
bool rnode_create(struct rnode* n, const char* url);

/**
 * @brief Receive a message of exactly len bytes (non-blocking beyond WAIT_MESSAGE_TIMEOUT_MS).
 * @return false if no message was available within the timeout, or its size didn't match len.
 */
bool rnode_receive(struct rnode* n, void* data, size_t len);

/**
 * @brief Broadcast a message of len bytes to every other node connected to the bus.
 * @return false if the message could not be fully sent.
 */
bool rnode_send(struct rnode* n, const void* data, size_t len);

/**
 * @brief Close the node's socket, leaving the bus.
 */
bool rnode_delete(struct rnode* n);

#endif  //__NODE_H__
