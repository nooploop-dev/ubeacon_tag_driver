#pragma once

#include "msg_handle.hpp"
#include <algorithm>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

namespace {

void handle_msg_data(const UbeaconLocationResult &d) {
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
      "vel=[{:.2f}, {:.2f}, {:.2f}], pos_noise=[{:.2f}, {:.2f}, {:.2f}], "
      "vel_noise=[{:.2f}, {:.2f}, {:.2f}], map_id={}, error_code={}, "
      "area_id={}, reserved={}, anchor_count={}, "
      "anchors(addr,rx_rssi,rx_rate)=[{}]",
      d.local_time_us, d.pos[0], d.pos[1], d.pos[2], d.vel_cm[0] * 0.01,
      d.vel_cm[1] * 0.01, d.vel_cm[2] * 0.01, d.pos_noise_cm[0] * 0.01,
      d.pos_noise_cm[1] * 0.01, d.pos_noise_cm[2] * 0.01,
      d.vel_noise_cm[0] * 0.01, d.vel_noise_cm[1] * 0.01,
      d.vel_noise_cm[2] * 0.01, d.map_id, d.error_code, d.area_id, d.reserved,
      d.anchor_count, anchors_str);
}

void handle_msg_data(const UbeaconAnchorDdoas &d) {
  std::string datas_str;
  for (int i = 0; i < d.count; ++i) {
    if (i > 0)
      datas_str += ",";
    auto &data = d.datas[i];
    datas_str +=
        fmt::format("({},{},{:.2f})", data.a0, data.a1, data.ddoa_cm * 0.01);
  }
  spdlog::info("UbeaconAnchorDdoas: local_time_us={}, reserved={}, count={}, "
               "datas(a0,a1,ddoa)=[{}]",
               d.local_time_us, d.reserved, d.count, datas_str);
}

void handle_msg_data(const UbeaconRestart &d) {
  spdlog::info("UbeaconRestart: delay={}, only_restart_when_need={}", d.delay,
               d.only_restart_when_need);
}

void handle_msg_data(const UbeaconFind &d) {
  spdlog::info("UbeaconFind: duration={}", d.duration);
}

void handle_msg_data(const UbeaconParam &d) {
  spdlog::info(
      "UbeaconParam: reserved=[{:02x} {:02x} {:02x} {:02x}], expect_z={}, "
      "z_noise={}, smooth_window={}, _max_acceleration=[{}, {}, {}], "
      "output=[tag_pos={}, anchor_packet={}, anchor_pos={}, "
      "anchor_link_data={}, anchor_signal={}, anchor_ddoa={}, "
      "tag_pos_even_error={}, anchor_link_status={}], sniff_duty_cycle={}, "
      "update_interval_max={}, reset_interval={}",
      d.reserved[0], d.reserved[1], d.reserved[2], d.reserved[3], d.expect_z,
      d._z_noise * 0.01, d.smooth_window, d._max_acceleration[0] * 0.02,
      d._max_acceleration[1] * 0.02, d._max_acceleration[2] * 0.02,
      d.output.tag_pos, d.output.anchor_packet, d.output.anchor_pos,
      d.output.anchor_link_data, d.output.anchor_signal, d.output.anchor_ddoa,
      d.output.tag_pos_even_error, d.output.anchor_link_status,
      d.sniff_duty_cycle, d.update_interval_max, d.reset_interval);
}

void handle_msg_data(const UbeaconRunTimeParam &d) {
  spdlog::info("UbeaconRunTimeParam: sniff_duty_cycle={}", d.sniff_duty_cycle);
}

void handle_msg_data(const UbeaconInterfaceParam &d) {
  spdlog::info("UbeaconInterfaceParam: uart={}, iic={}, uwb={}, ble={}", d.uart,
               d.iic, d.uwb, d.ble);
}

void handle_msg_data(const UbeaconUartInterfaceParam &d) {
  spdlog::info("UbeaconUartInterfaceParam: baudrate={}", d.baudrate);
}

void handle_msg_data(const UbeaconIicInterfaceParam &d) {
  spdlog::info("UbeaconIicInterfaceParam: addr={}", d.addr);
}

void handle_msg_data(const UbeaconUwbInterfaceParam &d) {
  spdlog::info("UbeaconUwbInterfaceParam: reserved={}, random_window={}, "
               "max_rx_interval={}",
               d.reserved, d._random_window * 0.01, d._max_rx_interval * 0.05);
}

void handle_msg_data(const UbeaconBleInterfaceParam &d) {
  spdlog::info("UbeaconBleInterfaceParam: company_id=[{:02x},{:02x}], "
               "data_type={:02x}, random_window={}, send_count={}",
               d.company_id[0], d.company_id[1], d.data_type,
               d._random_window * 0.01, d.send_count);
}

void handle_msg_data(const UbeaconHeartbeat &d) {
  spdlog::info(
      "UbeaconHeartbeat: battery_percent={}, battery_charging={}, "
      "need_restart={}, reset_info_dirty={}, assert_info_dirty={}, "
      "restart_cnt={}, hardware_enabled=[uart={},iic={},uwb={},ble={}], "
      "firmware_series={}, firmware_version={}.{}.{}.{}, uid={:02x}",
      d.battery_percent, d.battery_charging, d.need_restart, d.reset_info_dirty,
      d.assert_info_dirty, d.restart_cnt, d.hardware_enabled_uart,
      d.hardware_enabled_iic, d.hardware_enabled_uwb, d.hardware_enabled_ble,
      d.firmware_series, d.firmware_version[0], d.firmware_version[1],
      d.firmware_version[2], d.firmware_version[3],
      fmt::join(d.uid, d.uid + sizeof(d.uid), ""));
}

void handle_msg_data(const UbeaconAnchorSignal &d) {
  std::string datas_str;
  for (int i = 0; i < d.count; ++i) {
    if (i > 0)
      datas_str += ",";
    const auto &data = d.datas[i];
    datas_str += fmt::format(
        "({},{},{},{},{},{},{},{},{})", data.addr, data._fp_index * 0.1,
        int(data.fp_to_peak), data._mc * 0.01, -data._rx_rssi * 0.5,
        -data._fp_rssi * 0.5, data._uwb_clock_offset_ppm * 0.01,
        data._mcu_clock_offset_ppm * 0.01, data._rx_rate / 255.0);
  }
  spdlog::info("UbeaconAnchorSignal: local_time_us={}, count={}, area_id={}, "
               "reserved={}, "
               "datas(addr,fp_index,fp_to_peak,mc,rx_rssi,fp_rssi,uwb_clk_off,"
               "mcu_clk_off,rx_rate)=[{}]",
               d.local_time_us, int(d.count), int(d.area_id), d.reserved,
               datas_str);
}

template <typename T>
void handle_payload(const void *payload, int payload_size) {
  T d;
  // 版本兼容处理
  memset(&d, 0, sizeof(T));
  auto min_size = std::min(int(sizeof(T)), payload_size);
  memcpy(&d, payload, min_size);
  handle_msg_data(d);
}
} // namespace

void user_msg_cb(void *, const UbeaconUartMsg *msg) {
  auto msg_str = fmt::format(
      "id={}, payload=({}B){:02x}", msg->id, msg->payload_size,
      fmt::join(msg->payload, msg->payload + msg->payload_size, " "));
  spdlog::debug("msg: {}", msg_str);
  //   仅定长消息检查长度是否一致，不一致就存在版本差异
  int fixed_len_msg_payload_size = 0;
  switch (msg->id) {
  case UBEACON_MSG_RESTART:
    fixed_len_msg_payload_size = sizeof(UbeaconRestart);
    handle_payload<UbeaconRestart>(msg->payload, msg->payload_size);
    break;
  case UBEACON_MSG_FIND:
    fixed_len_msg_payload_size = sizeof(UbeaconFind);
    handle_payload<UbeaconFind>(msg->payload, msg->payload_size);
    break;
  case UBEACON_MSG_PARAM:
    fixed_len_msg_payload_size = sizeof(UbeaconParam);
    handle_payload<UbeaconParam>(msg->payload, msg->payload_size);
    break;
  case UBEACON_MSG_RUN_TIME_PARAM:
    fixed_len_msg_payload_size = sizeof(UbeaconRunTimeParam);
    handle_payload<UbeaconRunTimeParam>(msg->payload, msg->payload_size);
    break;
  case UBEACON_MSG_INTERFACE_PARAM:
    fixed_len_msg_payload_size = sizeof(UbeaconInterfaceParam);
    handle_payload<UbeaconInterfaceParam>(msg->payload, msg->payload_size);
    break;
  case UBEACON_MSG_UART_INTERFACE_PARAM:
    fixed_len_msg_payload_size = sizeof(UbeaconUartInterfaceParam);
    handle_payload<UbeaconUartInterfaceParam>(msg->payload, msg->payload_size);
    break;
  case UBEACON_MSG_IIC_INTERFACE_PARAM:
    fixed_len_msg_payload_size = sizeof(UbeaconIicInterfaceParam);
    handle_payload<UbeaconIicInterfaceParam>(msg->payload, msg->payload_size);
    break;
  case UBEACON_MSG_UWB_INTERFACE_PARAM:
    fixed_len_msg_payload_size = sizeof(UbeaconUwbInterfaceParam);
    handle_payload<UbeaconUwbInterfaceParam>(msg->payload, msg->payload_size);
    break;
  case UBEACON_MSG_BLE_INTERFACE_PARAM:
    fixed_len_msg_payload_size = sizeof(UbeaconBleInterfaceParam);
    handle_payload<UbeaconBleInterfaceParam>(msg->payload, msg->payload_size);
    break;
  case UBEACON_MSG_LOCATION_RESULT:
    handle_payload<UbeaconLocationResult>(msg->payload, msg->payload_size);
    break;
  case UBEACON_MSG_HEARTBEAT:
    fixed_len_msg_payload_size = sizeof(UbeaconHeartbeat);
    handle_payload<UbeaconHeartbeat>(msg->payload, msg->payload_size);
    break;
  case UBEACON_MSG_ANCHOR_SIGNAL:
    handle_payload<UbeaconAnchorSignal>(msg->payload, msg->payload_size);
    break;
  case UBEACON_MSG_ANCHOR_DDOAS:
    handle_payload<UbeaconAnchorDdoas>(msg->payload, msg->payload_size);
    break;
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
