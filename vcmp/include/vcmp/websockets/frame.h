#ifndef _VCMP_WEBSOCKETS_FRAME_H
#define _VCMP_WEBSOCKETS_FRAME_H

#include <stdbool.h>
#include <stdint.h>

#define WS_OPCODE_CONTINUATION 0
#define WS_OPCODE_TEXT 1
#define WS_OPCODE_BINARY 2
#define WS_OPCODE_CLOSE 8
#define WS_OPCODE_PING 9
#define WS_OPCODE_PONG 10

typedef struct ws_frame {
    bool fin;
    uint8_t opcode;
    uint64_t len;
    uint8_t *begin;
} ws_frame_t;

/* Parses frames received from a ws client, meaning it includes the masking key
 */
int ws_parse_frame(uint8_t *buf, const uint64_t buf_len, ws_frame_t *frame_out);

/* Returns a frame size based on `payload_len` without a masking key */
int ws_calc_frame_size(const uint64_t payload_len);

/* Makes a frame for ws clients to be sent */
int ws_make_frame(const bool fin, const uint8_t opcode,
                  const uint64_t payload_len, uint8_t *buf_out,
                  const uint64_t buf_len);

#endif // _VCMP_WEBSOCKETS_FRAME_H