#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ubeacon_tag_common.h"

// 串口数据是字节流数据，因此额外添加帧格式，用于分包
// 帧格式：sof payload_size payload checksum
#define UBEACON_UART_SOF 0xAA
typedef uint16_t ubeacon_uart_payload_size_t;
// payload: UbeaconUartPayloadDown or UbeaconUartPayloadUp
typedef uint8_t ubeacon_checksum_t;

static inline ubeacon_checksum_t ubeacon_get_sum(const void *data,
                                                 int data_size) {
  const uint8_t *p = (uint8_t *)data;
  ubeacon_checksum_t sum = 0;
  for (int i = 0; i < data_size; ++i) {
    sum += p[i];
  }
  return sum;
}
static inline bool ubeacon_verify_sum(const void *data, int data_size) {
  return *(const ubeacon_checksum_t *)((const uint8_t *)data + data_size -
                                       sizeof(ubeacon_checksum_t)) ==
         ubeacon_get_sum(data, data_size - sizeof(ubeacon_checksum_t));
}
static inline void ubeacon_update_sum(void *data, int data_size) {
  *(ubeacon_checksum_t *)((uint8_t *)data + data_size -
                          sizeof(ubeacon_checksum_t)) =
      ubeacon_get_sum(data, data_size - sizeof(ubeacon_checksum_t));
}

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

#define UBEACON_UART_MSG_PAYLOAD_SIZE_MAX 127
typedef struct {
  ubeacon_msg_id_t id;
  uint8_t payload_size : 7;
  uint8_t reserved : 1;
  uint8_t payload[UBEACON_UART_MSG_PAYLOAD_SIZE_MAX];
} Msg;

#ifdef __cplusplus
}
#endif
