#ifndef __HELP_H__
#define __HELP_H__

#include "stdbool.h"
#include "test_defs.h"

bool packet_cmp(radio_packet* first, radio_packet* second);

void print_rp(radio_packet* pack);

#endif  //__HELP_H__
