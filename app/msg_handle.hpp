#pragma once

#include "ubeacon_tag_uart.h"
#include <algorithm>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <sstream>

template <typename T> void copy_data(T *dst, const void *data, int data_size) {
  memset(dst, 0, sizeof(T));
  auto min_size = std::min(int(sizeof(T)), data_size);
  memcpy(dst, data, min_size);
}

inline void handle_location_result(const UbeaconLocationResult &d) {
  std::string anchors_str;
  for (int i = 0; i < d.anchor_count; ++i) {
    if (i > 0)
      anchors_str += ",";
    auto &anchor = d.anchors[i];
    anchors_str +=
        fmt::format("({},{},{})", anchor.addr, anchor._rx_rssi / (-2.0),
                    anchor._rx_rate / 255.0);
  }
  spdlog::info(
      "UbeaconLocationResult: local_time_us={}, pos=[{}, {}, {}], "
      "vel=[{:.2f}, {:.2f}, {:.2f}]m, pos_noise=[{:.2f}, {:.2f}, {:.2f}]m, "
      "vel_noise=[{:.2f}, {:.2f}, {:.2f}]m, map_id={}, error_code={}, "
      "area_id={}, reserved={}, anchor_count={}, "
      "anchors(addr,rx_rssi,rx_rate)=[{}]",
      d.local_time_us, d.pos[0], d.pos[1], d.pos[2], d.vel_cm[0] / 100.0,
      d.vel_cm[1] / 100.0, d.vel_cm[2] / 100.0, d.pos_noise_cm[0] / 100.0,
      d.pos_noise_cm[1] / 100.0, d.pos_noise_cm[2] / 100.0,
      d.vel_noise_cm[0] / 100.0, d.vel_noise_cm[1] / 100.0,
      d.vel_noise_cm[2] / 100.0, int(d.map_id), int(d.error_code),
      int(d.area_id), int(d.reserved), int(d.anchor_count), anchors_str);
}

inline void handle_anchor_ddoas(const UbeaconAnchorDdoas &d) {
  std::string datas_str;
  for (int i = 0; i < d.count; ++i) {
    if (i > 0)
      datas_str += ",";
    auto &data = d.datas[i];
    datas_str +=
        fmt::format("({},{},{:.2f})", data.a0, data.a1, data.ddoa_cm / 100.0);
  }
  spdlog::info("UbeaconAnchorDdoas: local_time_us={}, reserved={}, count={}, "
               "datas(a0,a1,ddoa)=[{}]",
               d.local_time_us, int(d.reserved), int(d.count), datas_str);
}

inline void user_msg_cb(void *, const UbeaconUartMsg *msg) {
  auto msg_str = fmt::format(
      "id={}, payload=({}B){:02x}", msg->id, msg->payload_size,
      fmt::join(msg->payload, msg->payload + msg->payload_size, " "));
  spdlog::debug("msg: {}", msg_str);
  //   仅定长消息检查长度是否一致，不一致就存在版本差异
  int fixed_len_msg_payload_size = 0;
  switch (msg->id) {
  case UBEACON_MSG_LOCATION_RESULT: {
    UbeaconLocationResult d;
    copy_data(&d, msg->payload, msg->payload_size);
    handle_location_result(d);
  } break;
  case UBEACON_MSG_ANCHOR_DDOAS: {
    UbeaconAnchorDdoas d;
    copy_data(&d, msg->payload, msg->payload_size);
    handle_anchor_ddoas(d);
  } break;
  default:
    spdlog::warn("unknown msg: {}", msg_str);
    break;
  }
  if (fixed_len_msg_payload_size > 0 &&
      fixed_len_msg_payload_size != msg->payload_size) {
    spdlog::warn("msg payload_size mismatch: expected={}, actual={}, {}",
                 fixed_len_msg_payload_size, msg->payload_size, msg_str);
  }
}
