#include "help.h"
#include "../rlog/rlog.h"
#include "stdio.h"
#include "string.h"

void print_rp(radio_packet* pack)
{
    RLOG_DEBUG("pream  = %d; id = %d; len = %d; crc = %d;", pack->preamble, pack->id, pack->payload_length, pack->crc);

    printf("          [ DATA ] = | ");
    for (int i = 0; i < pack->payload_length; i++)
    {
        printf("%2X | ", pack->payload[i]);
    }
    printf("\n");
}

bool packet_cmp(radio_packet* first, radio_packet* second)
{
    unsigned char success = 0;
    if (first->preamble == second->preamble)
        success |= (1 << 0);
    if (first->id == second->id)
        success |= (1 << 1);
    if (first->payload_length == second->payload_length)
        success |= (1 << 2);
    if (memcmp(first->payload, second->payload, first->payload_length) == 0)
        success |= (1 << 3);
    if (first->crc == second->crc)
        success |= (1 << 4);

    return (success & 0b00011111) == 0b00011111 ? true : false;
}
