#ifndef __TEST_DEFS_H__
#define __TEST_DEFS_H__

#include "stdint.h"

#define BROADCAST_ID 0x00
#define MAX_PROTO_MESSAGE_LENGTH UINT8_MAX                           // base 64 encoded
#define MAX_PAYLOAD_LENGTH ((MAX_PROTO_MESSAGE_LENGTH * 3 / 4) - 1)  // base64 decode length

#define PREAMBLE_REQUEST 0xFFFF
#define PREAMBLE_RESPONSE 0xAAAA
#define PREAMBLE_SET_ID 0x2222
#define PREAMBLE_UNIQUE_ID 0x1111
#define PREAMBLE_BROADCAST 0x3333

#pragma pack(push)
#pragma pack(1)
typedef struct
{
    uint16_t preamble;
    uint8_t  id;
    uint8_t  payload_length;
    uint8_t  payload[MAX_PROTO_MESSAGE_LENGTH];
    uint16_t crc;
} radio_packet;
#pragma pack(pop)

#endif
