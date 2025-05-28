#include "ubeacon_tag_uart.h"
#include <string.h>

ubeacon_uart_checksum_t ubeacon_uart_get_sum(const void *data, int data_size) {
  const uint8_t *p = (uint8_t *)data;
  ubeacon_uart_checksum_t sum = 0;
  for (int i = 0; i < data_size; ++i) {
    sum += p[i];
  }
  return sum;
}
bool ubeacon_uart_verify_sum(const void *data, int data_size) {
  return *(const ubeacon_uart_checksum_t *)((const uint8_t *)data + data_size -
                                            sizeof(ubeacon_uart_checksum_t)) ==
         ubeacon_uart_get_sum(data,
                              data_size - sizeof(ubeacon_uart_checksum_t));
}
void ubeacon_uart_update_sum(void *data, int data_size) {
  *(ubeacon_uart_checksum_t *)((uint8_t *)data + data_size -
                               sizeof(ubeacon_uart_checksum_t)) =
      ubeacon_uart_get_sum(data, data_size - sizeof(ubeacon_uart_checksum_t));
}

void ubeacon_uart_frame_parser_init(UbeaconUartFrameParser *parser,
                                    ubeacon_uart_frame_payload_cb_f payload_cb,
                                    void *arg) {
  parser->payload_cb_ = payload_cb;
  parser->arg = arg;
  parser->index_begin_ = 0;
  parser->index_end_ = 0;
}
static inline int frame_parser_buffer_size(UbeaconUartFrameParser *parser) {
  return parser->index_end_ - parser->index_begin_;
}
static inline uint8_t *frame_parser_buffer(UbeaconUartFrameParser *parser,
                                           int index) {
  return parser->buffer_ + parser->index_begin_ + index;
}
static inline void frame_parser_pop_front(UbeaconUartFrameParser *parser,
                                          int size) {
  parser->index_begin_ += size;
}

static void frame_parser_append(UbeaconUartFrameParser *parser,
                                const uint8_t *data, int data_size) {
  memcpy(parser->buffer_ + parser->index_end_, data, data_size);
  parser->index_end_ += data_size;
  while (frame_parser_buffer_size(parser) >= UBEACON_UART_FRAME_SIZE_MIN) {
    if (*frame_parser_buffer(parser, 0) != UBEACON_UART_SOF) {
      frame_parser_pop_front(parser, 1);
      continue;
    }
    int payload_size = *(ubeacon_uart_payload_size_t *)(frame_parser_buffer(
        parser, sizeof(ubeacon_uart_sof_t)));
    if (payload_size > UBEACON_UART_FRAME_PAYLOAD_SIZE_MAX) {
      frame_parser_pop_front(parser, 1);
      continue;
    }
    int frame_size = UBEACON_UART_FRAME_SIZE_MIN + payload_size;
    if (frame_parser_buffer_size(parser) < frame_size) {
      break;
    }
    if (!ubeacon_uart_verify_sum(frame_parser_buffer(parser, 0), frame_size)) {
      frame_parser_pop_front(parser, 1);
      continue;
    }

    const UbeaconUartPayloadUp *payload_up =
        (const UbeaconUartPayloadUp *)(frame_parser_buffer(
            parser,
            sizeof(ubeacon_uart_sof_t) + sizeof(ubeacon_uart_payload_size_t)));
    if (payload_up->frame_id == 0x05) {
      parser->payload_cb_(parser->arg, payload_up->uid, payload_up->msg_buf,
                          payload_size - sizeof(payload_up->frame_id) -
                              sizeof(payload_up->uid));
    }
    frame_parser_pop_front(parser, frame_size);
  }
  if (parser->index_begin_ > 0) {
    memmove(parser->buffer_, parser->buffer_ + parser->index_begin_,
            frame_parser_buffer_size(parser));
    parser->index_end_ -= parser->index_begin_;
    parser->index_begin_ = 0;
  }
}

void ubeacon_uart_frame_parser_handle_data(UbeaconUartFrameParser *parser,
                                           const void *data, int data_size) {
  const uint8_t *p = (const uint8_t *)data;
  int offset = 0;
  while (offset < data_size) {
    int available_place = UBEACON_UART_FRAME_SIZE_MAX - parser->index_end_;
    int remain_size = data_size - offset;
    int size = (available_place < remain_size) ? available_place : remain_size;
    frame_parser_append(parser, p + offset, size);
    offset += size;
  }
}

void ubeacon_uart_msg_parser_handle_data(ubeacon_uart_msg_cb_f cb, void *arg,
                                         const void *data, int data_size) {
  int addr = 0;
  const uint8_t *p = (const uint8_t *)data;
  while (addr < data_size) {
    const UbeaconUartMsg *msg = (const UbeaconUartMsg *)(p + addr);
    cb(arg, msg);
    addr += ubeacon_uart_msg_size(msg);
  }
}
