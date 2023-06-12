#include "test_node.h"
#include "../rnode.h"
#include "../runit/src/runit.h"
#include "test_defs.h"
#include "../rtimeout/rtimeout.h"
#include "unistd.h"
#include "pthread.h"
#include <stdio.h>
#include "help.h"

#include "urls.h"
#include <sys/wait.h>
#include "test_proc.h"

#define MAX_NODE (TOTAL_NODE_QUANTITY - 1)

struct thread_data
{
    struct rnode nd;
    bool         done;
    pthread_t    thread;
    radio_packet rp;
    rt           timeout;
};

struct thread_data th_data[MAX_NODE];

radio_packet sp = {.preamble       = PREAMBLE_SET_ID,
                   .id             = 0x01,
                   .payload_length = 3,
                   .payload        = {0x01, 0x02, 0x03},
                   .crc            = 0x3456};

void* thread_loop_sender(void* arg)
{
    struct thread_data* td = (struct thread_data*) arg;
    if (!rnode_create(&td->nd, URL_DATA_EXCHANGE_IN_PROC))
    {
        printf("cannot create node sender = %p\n", (void*) &td->nd);
    }
    usleep(10000);
    if (rnode_send(&td->nd, &sp, sizeof(radio_packet)))
    {
        td->done = true;
    }
    else
    {
        td->done = false;
        printf("cannot send  = %p\n", (void*) &td->nd);
    }
    sleep(2);
    if (!rnode_delete(&td->nd))
    {
        printf("cannot delete node sender = %p\n", (void*) &td->nd);
    }
    pthread_exit(0);
}

void* thread_loop_receivers(void* arg)
{
    struct thread_data* td = (struct thread_data*) arg;
    rt                  timeout;
    if (!rnode_create(&td->nd, URL_DATA_EXCHANGE_IN_PROC))
    {
        printf("cannot create node receiver = %p\n", (void*) &td->nd);
    }
    usleep(1000);
    rt_set(&timeout, 2000000);
    while (!(rt_timed_out(&timeout) > 0))
    {
        if (rnode_receive(&td->nd, &td->rp, sizeof(radio_packet)))
        {
            if (packet_cmp(&sp, &td->rp))
            {
                td->done = true;
                if (!rnode_delete(&td->nd))
                {
                    printf("error delete node = %p\n", (void*) &td->nd);
                }
                pthread_exit(0);
            }
            printf("error cmp packet\n");
        }
        else
        {
            usleep(10);
        }
    }
    printf("error receive packet node = %p sock = %d eid = %d\n", (void*) &td->nd, td->nd.sock, td->nd.eid);
    td->done = false;
    if (!rnode_delete(&td->nd))
    {
        printf("error delete node = %p\n", (void*) &td->nd);
    }
    pthread_exit(0);
}

bool start_tread_sender(void)
{
    if (pthread_create(&th_data[0].thread, NULL, &thread_loop_sender, &th_data[0]) != 0)
    {
        printf("cannot start thread sender \n ");
        return false;
    }
    return true;
}

void stop_thread_sender(void)
{
    pthread_join(th_data[0].thread, NULL);
}

bool start_thread_receivers(void)
{
    for (int i = 1; i < MAX_NODE; i++)
    {
        if (pthread_create(&th_data[i].thread, NULL, &thread_loop_receivers, &th_data[i]) != 0)
        {
            printf("cannot start thread sender \n ");
            return false;
        }
    }
    return true;
}

void stop_thread_receivers(void)
{
    for (int i = 1; i < MAX_NODE; i++)
    {
        pthread_join(th_data[i].thread, NULL);
    }
}

void test_node_inproc(void)
{
    runit_true(start_thread_receivers());
    runit_true(start_tread_sender());

    stop_thread_receivers();
    stop_thread_sender();
    for (int i = 0; i < MAX_NODE; i++)
    {
        runit_true(th_data[i].done);
    }
}

void test_node_inter_proc(void)
{
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        perror("fork");
        runit_true(false);
    case 0:
        test_receive_increment_and_send_back();
        break;
    default:
        test_send_packet_and_wait_incremented_echo();
        break;
    }
}

void test_node_group_inproc3(void)
{
    struct rnode node0 = {0};
    struct rnode node1 = {0};
    struct rnode node2 = {0};

    radio_packet spinpros = {.preamble       = PREAMBLE_SET_ID,
                             .id             = 0x01,
                             .payload_length = 3,
                             .payload        = {0x01, 0x02, 0x03},
                             .crc            = 0x3456};
    radio_packet rp1      = {0};
    radio_packet rp2      = {0};

    runit_true(rnode_create(&node0, URL_DATA_EXCHANGE_IN_PROC));
    runit_true(rnode_create(&node1, URL_DATA_EXCHANGE_IN_PROC));
    runit_true(rnode_create(&node2, URL_DATA_EXCHANGE_IN_PROC));
    usleep(1000);

    runit_true(rnode_send(&node0, &spinpros, sizeof(radio_packet)));

    runit_false(rnode_receive(&node0, &rp1, sizeof(radio_packet))); /** Node do not must receive self message*/

    runit_true(rnode_receive(&node1, &rp1, sizeof(radio_packet)));
    runit_true(packet_cmp(&spinpros, &rp1));

    runit_true(rnode_receive(&node2, &rp2, sizeof(radio_packet)));
    runit_true(packet_cmp(&spinpros, &rp2));

    runit_false(rnode_receive(&node0, &rp1, sizeof(radio_packet)));
    runit_false(rnode_receive(&node1, &rp1, sizeof(radio_packet)));
    runit_false(rnode_receive(&node2, &rp1, sizeof(radio_packet)));

    usleep(100);

    runit_true(rnode_delete(&node1));
    runit_true(rnode_delete(&node2));
    runit_true(rnode_delete(&node0));

    usleep(100);
}

void test_node_group_inproc8(void)
{
    struct rnode node0 = {0};
    struct rnode node1 = {0};
    struct rnode node2 = {0};
    struct rnode node3 = {0};
    struct rnode node4 = {0};
    struct rnode node5 = {0};
    struct rnode node6 = {0};
    struct rnode node7 = {0};

    radio_packet sp1 = {.preamble       = 0xFFFF,
                        .id             = 0x08,
                        .payload_length = MAX_PROTO_MESSAGE_LENGTH,
                        .payload        = {0},
                        .crc            = 0xBBB8};

    radio_packet sp2 = {.preamble       = 0xFFFF,
                        .id             = 0x07,
                        .payload_length = MAX_PROTO_MESSAGE_LENGTH,
                        .payload        = {0},
                        .crc            = 0xBBB7};

    radio_packet sp3 = {.preamble       = 0xFFFF,
                        .id             = 0x06,
                        .payload_length = MAX_PROTO_MESSAGE_LENGTH,
                        .payload        = {0},
                        .crc            = 0xBBB6};

    radio_packet rp1 = {0};
    radio_packet rp2 = {0};
    radio_packet rp3 = {0};
    radio_packet rp4 = {0};
    radio_packet rp5 = {0};
    radio_packet rp6 = {0};
    radio_packet rp7 = {0};
    radio_packet rp8 = {0};

    runit_true(rnode_create(&node0, URL_DATA_EXCHANGE_IN_PROC));
    runit_true(rnode_create(&node1, URL_DATA_EXCHANGE_IN_PROC));
    runit_true(rnode_create(&node2, URL_DATA_EXCHANGE_IN_PROC));
    runit_true(rnode_create(&node3, URL_DATA_EXCHANGE_IN_PROC));
    runit_true(rnode_create(&node4, URL_DATA_EXCHANGE_IN_PROC));
    runit_true(rnode_create(&node5, URL_DATA_EXCHANGE_IN_PROC));
    runit_true(rnode_create(&node6, URL_DATA_EXCHANGE_IN_PROC));
    runit_true(rnode_create(&node7, URL_DATA_EXCHANGE_IN_PROC));
    usleep(100);

    /** first message */
    runit_true(rnode_send(&node7, &sp1, sizeof(radio_packet)));

    runit_true(rnode_receive(&node0, &rp1, sizeof(radio_packet)));
    runit_true(rnode_receive(&node1, &rp2, sizeof(radio_packet)));
    runit_true(rnode_receive(&node2, &rp3, sizeof(radio_packet)));
    runit_true(rnode_receive(&node3, &rp4, sizeof(radio_packet)));
    runit_true(rnode_receive(&node4, &rp5, sizeof(radio_packet)));
    runit_true(rnode_receive(&node5, &rp6, sizeof(radio_packet)));
    runit_true(rnode_receive(&node6, &rp7, sizeof(radio_packet)));
    runit_false(rnode_receive(&node7, &rp8, sizeof(radio_packet)));

    runit_true(packet_cmp(&sp1, &rp1));
    runit_true(packet_cmp(&sp1, &rp2));
    runit_true(packet_cmp(&sp1, &rp3));
    runit_true(packet_cmp(&sp1, &rp4));
    runit_true(packet_cmp(&sp1, &rp5));
    runit_true(packet_cmp(&sp1, &rp6));
    runit_true(packet_cmp(&sp1, &rp7));

    /** second message */
    runit_true(rnode_send(&node6, &sp2, sizeof(radio_packet)));
    usleep(1000);
    runit_true(rnode_receive(&node0, &rp1, sizeof(radio_packet)));
    runit_true(rnode_receive(&node1, &rp2, sizeof(radio_packet)));
    runit_true(rnode_receive(&node2, &rp3, sizeof(radio_packet)));
    runit_true(rnode_receive(&node3, &rp4, sizeof(radio_packet)));
    runit_true(rnode_receive(&node4, &rp5, sizeof(radio_packet)));
    runit_true(rnode_receive(&node5, &rp6, sizeof(radio_packet)));
    runit_false(rnode_receive(&node6, &rp7, sizeof(radio_packet)));
    runit_true(rnode_receive(&node7, &rp8, sizeof(radio_packet)));

    runit_true(packet_cmp(&sp2, &rp1));
    runit_true(packet_cmp(&sp2, &rp2));
    runit_true(packet_cmp(&sp2, &rp3));
    runit_true(packet_cmp(&sp2, &rp4));
    runit_true(packet_cmp(&sp2, &rp5));
    runit_true(packet_cmp(&sp2, &rp6));
    runit_true(packet_cmp(&sp2, &rp8));

    /** third message */
    runit_true(rnode_send(&node5, &sp3, sizeof(radio_packet)));
    usleep(1000);
    runit_true(rnode_receive(&node0, &rp1, sizeof(radio_packet)));
    runit_true(rnode_receive(&node1, &rp2, sizeof(radio_packet)));
    runit_true(rnode_receive(&node2, &rp3, sizeof(radio_packet)));
    runit_true(rnode_receive(&node3, &rp4, sizeof(radio_packet)));
    runit_true(rnode_receive(&node4, &rp5, sizeof(radio_packet)));
    runit_false(rnode_receive(&node5, &rp6, sizeof(radio_packet)));
    runit_true(rnode_receive(&node6, &rp7, sizeof(radio_packet)));
    runit_true(rnode_receive(&node7, &rp8, sizeof(radio_packet)));

    runit_true(packet_cmp(&sp3, &rp1));
    runit_true(packet_cmp(&sp3, &rp2));
    runit_true(packet_cmp(&sp3, &rp3));
    runit_true(packet_cmp(&sp3, &rp4));
    runit_true(packet_cmp(&sp3, &rp5));
    runit_true(packet_cmp(&sp3, &rp7));
    runit_true(packet_cmp(&sp3, &rp8));

    runit_false(rnode_receive(&node0, &rp1, sizeof(radio_packet))); /** Node do not must receive self message*/
    runit_false(rnode_receive(&node1, &rp1, sizeof(radio_packet)));
    runit_false(rnode_receive(&node2, &rp1, sizeof(radio_packet)));

    runit_true(rnode_delete(&node0));
    runit_true(rnode_delete(&node1));
    runit_true(rnode_delete(&node2));
    runit_true(rnode_delete(&node3));
    runit_true(rnode_delete(&node4));
    runit_true(rnode_delete(&node5));
    runit_true(rnode_delete(&node6));
    runit_true(rnode_delete(&node7));
}
