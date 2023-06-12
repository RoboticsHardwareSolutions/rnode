#include "test_proc.h"
#include "stdio.h"
#include "../rnode.h"
#include "../rtimeout/rtimeout.h"
#include "unistd.h"
#include "stdlib.h"
#include "urls.h"
#include "test_defs.h"
#include "../runit/src/runit.h"
#include "help.h"

void test_send_packet_and_wait_incremented_echo(void)
{
    printf("PARENT start - test_send_packet_and_wait_incremented_echo\n");
    radio_packet pack           = {.preamble       = PREAMBLE_SET_ID,
                                   .id             = 0x01,
                                   .payload_length = 3,
                                   .payload        = {0x01, 0x02, 0x03},
                                   .crc            = 0x3456};
    struct rnode parent_node    = {0};
    radio_packet receive_packet = {0};
    rt           parent_timeout = {0};
    bool         parent_result  = false;
    int          timed_out      = 0;
    int          count          = 0;
    usleep(100);
    if (!rnode_create(&parent_node, URL_DATA_EXCHANGE_INTER_PROC))
    {
        printf(" PARENT:  error create !\n");
    }
    printf(" PARENT: node created  !\n");
    usleep(1000);
    if (!rnode_send(&parent_node, &pack, sizeof(radio_packet)))
    {
        printf(" PARENT:   error send !\n");
        exit(0);
    }
    printf(" PARENT: send  !\n");
    rt_set(&parent_timeout, 300000);
    do
    {
        count++;
        if (rnode_receive(&parent_node, &receive_packet, sizeof(radio_packet)))
        {
            printf("parent receive \n");
            pack.id++;
            parent_result = packet_cmp(&pack, &receive_packet);
            break;
        }
        else
        {
            usleep(10);
        }
        timed_out = rt_timed_out(&parent_timeout);
        if (timed_out == 1 || timed_out == -1)
        {
            break;
        }
    } while (1);
    int wstatus = 0;
    wait(&wstatus);
    sleep(1);
    if (!rnode_delete(&parent_node))
    {
        printf(" PARENT:   error delete !\n");
    }
    printf(" PARENT:   stoped !\n");
    runit_true(parent_result);
}

void test_receive_increment_and_send_back(void)
{
    printf("CHILD start - test_receive_increment_and_send_back\n");
    rt           timeout_child        = {0};
    radio_packet child_receive_packet = {0};
    struct rnode child_node           = {0};
    int          count                = 0;
    int          timed_out            = 0;
    if (!rnode_create(&child_node, URL_DATA_EXCHANGE_INTER_PROC))
    {
        printf(" CHILD: exit error create !\n");
        exit(0);
    }
    printf(" CHILD: node created  !\n");
    usleep(100);
    printf(" CHILD: start listen !\n");
    rt_set(&timeout_child, 3000000);
    do
    {
        count++;
        if (rnode_receive(&child_node, &child_receive_packet, sizeof(radio_packet)))
        {
            printf("child  receive \n");
            child_receive_packet.id++;
            if (!rnode_send(&child_node, &child_receive_packet, sizeof(radio_packet)))
            {
                printf(" CHILD: exit  error send !\n");
            }
            sleep(1);
            if (!rnode_delete(&child_node))
            {
                printf(" CHILD: exit  error delete !\n");
            }
            exit(0);
        }
        else
        {
            usleep(10);
        }
        timed_out = rt_timed_out(&timeout_child);
        if (timed_out == 1 || timed_out == -1)
        {
            break;
        }
    } while (1);
    printf("count %d\n", count);
    printf(" CHILD: message was not receive !\n");
    sleep(1);
    if (!rnode_delete(&child_node))
    {
        printf(" CHILD: exit  error delete !\n");
    }
    printf(" CHILD:   stoped !\n");
    exit(0);
}
