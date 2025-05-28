#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ubeacon_tag_common.h"
#include <stddef.h>

// 串口数据是字节流数据，因此额外添加帧格式，用于分包
// 帧格式：sof payload_size payload checksum
#define UBEACON_UART_FRAME_SIZE_MIN 4
#define UBEACON_UART_FRAME_SIZE_MAX 1024
#define UBEACON_UART_FRAME_PAYLOAD_SIZE_MAX                                    \
  (UBEACON_UART_FRAME_SIZE_MAX - UBEACON_UART_FRAME_SIZE_MIN)

#define UBEACON_UART_SOF 0xAA
typedef uint8_t ubeacon_uart_sof_t;
typedef uint16_t ubeacon_uart_payload_size_t;
// payload: UbeaconUartPayloadDown or UbeaconUartPayloadUp
typedef uint8_t ubeacon_uart_checksum_t;

ubeacon_uart_checksum_t ubeacon_uart_get_sum(const void *data, int data_size);
bool ubeacon_uart_verify_sum(const void *data, int data_size);
void ubeacon_uart_update_sum(void *data, int data_size);

#define UBEACON_UART_PAYLOAD_DOWN_SIZE_MAX 1019
typedef struct {
  uint8_t frame_id; // 0x02
  uint8_t msg_buf[UBEACON_UART_PAYLOAD_DOWN_SIZE_MAX];
} UbeaconUartPayloadDown;

#define UBEACON_UART_PAYLOAD_UP_SIZE_MAX 1005
typedef struct {
  uint8_t uid[6];
  uint8_t frame_id; // 0x05
  uint8_t msg_buf[UBEACON_UART_PAYLOAD_UP_SIZE_MAX];
} UbeaconUartPayloadUp;

// UbeaconUartPayloadDown UbeaconUartPayloadUp中的msg_buf则可存放不定数量的Msg

typedef void (*ubeacon_uart_frame_payload_cb_f)(void *arg, const uint8_t *uid,
                                                const uint8_t *payload,
                                                int payload_size);
typedef struct {
  ubeacon_uart_frame_payload_cb_f payload_cb_;
  void *arg;
  uint8_t buffer_[UBEACON_UART_FRAME_SIZE_MAX];
  int index_begin_;
  int index_end_;
} UbeaconUartFrameParser;

void ubeacon_uart_frame_parser_init(UbeaconUartFrameParser *parser,
                                    ubeacon_uart_frame_payload_cb_f payload_cb,
                                    void *arg);

void ubeacon_uart_frame_parser_handle_data(UbeaconUartFrameParser *parser,
                                           const void *data, int data_size);

#define UBEACON_UART_MSG_PAYLOAD_SIZE_MAX 127
typedef struct {
  ubeacon_msg_id_t id;
  uint8_t payload_size : 7;
  uint8_t reserved : 1;
  uint8_t payload[UBEACON_UART_MSG_PAYLOAD_SIZE_MAX];
} UbeaconUartMsg;
static inline int ubeacon_uart_msg_size(const UbeaconUartMsg *msg) {
  return offsetof(UbeaconUartMsg, payload) + msg->payload_size;
}

typedef void (*ubeacon_uart_msg_cb_f)(void *arg, const UbeaconUartMsg *msg);
void ubeacon_uart_msg_parser_handle_data(ubeacon_uart_msg_cb_f cb, void *arg,
                                         const void *data, int data_size);

#ifdef __cplusplus
}
#endif
