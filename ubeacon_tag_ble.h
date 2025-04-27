#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// CRC算法 CRC-8
// 多项式公式 x8+x2+x+1
// 宽度 8
// 多项式 07
// 初始值 00
// 结果异或值 00
// 输入反转 false
// 输出反转 false
// 示例数据
// 0639034401040318123a73048c000000f2040000770000001231
// 0639034401040318123a12048e000000fa040000780000000f48
// 验证 https://www.lddgo.net/encrypt/crc
typedef uint8_t ubeacon_ble_crc_t;

// 定位结果数据帧 有定位时发送
typedef struct {
  uint8_t ad_length_0;    // 0x02
  uint8_t flag_data_type; // 0x01
  uint8_t flag_data;      // 0x06
  uint8_t ad_length_1;    // 0x1b
  uint8_t ad_type;        // 0xff
  uint8_t company_id[2];  // 默认0x06 0x39 可配置
  uint8_t user_data_type; // 默认0x50 可配置
  uint8_t msg_id;         // 0x44
  uint8_t uid[6];         // 设备唯一id
  // 定位结果对应的本地时间低15位（可用于计算前后定位间隔及数据去重处理），单位10ms
  uint16_t local_time_10ms : 10;
  uint8_t map_id : 5;       // 定位结果所属的地图id
  uint16_t is_ecef_pos : 1; // 是否为ecef坐标
  int32_t pos_cm[3];        // 定位结果，单位cm
  uint8_t pos_noise_cm;     // 定位噪声，单位cm
  ubeacon_ble_crc_t crc;    // 从company_id开始到crc前所有数据的校验
} UbeaconBleLocationResult;

// 设备心跳数据帧 周期性发送
typedef struct {
  uint8_t ad_length_0;           // 0x02
  uint8_t flag_data_type;        // 0x01
  uint8_t flag_data;             // 0x06
  uint8_t ad_length_1;           // 0x1b
  uint8_t ad_type;               // 0xff
  uint8_t company_id[2];         // 默认0x06 0x39 可配置
  uint8_t user_data_type;        // 默认0x50 可配置
  uint8_t msg_id;                // 0x4e
  uint8_t uid[6];                // 设备唯一id
  uint32_t local_time;           // 设备本地时间，单位s
  uint8_t firmware_series;       // 固件系列
  uint8_t firmware_version[4];   // 固件版本
  uint8_t remaining_battery : 7; // 剩余电量百分比
  uint8_t is_charging : 1;       // 是否充电中
  uint8_t reserved[5];
  ubeacon_ble_crc_t crc; // 从company_id开始到crc前所有数据的校验
} UbeaconBleHeartbeat;

#ifdef __cplusplus
}
#endif
