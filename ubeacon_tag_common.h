#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

// 有的参数存于rom中，不宜频繁变更
// 有的参数存于ram中，可频繁变更，但掉电丢失
// 时间相关数据单位默认s
// 距离相关数据单位默认m
// 有的数据为了节省空间使用了倍率转换，变量以下划线开头

typedef uint8_t ubeacon_msg_id_t;

typedef enum {
  UBEACON_MSG_RESTART = 2,
  UBEACON_MSG_FIND = 3,

  UBEACON_MSG_READ_PARAM = 60,
  UBEACON_MSG_WRITE_PARAM = 61,
  UBEACON_MSG_PARAM = UBEACON_MSG_WRITE_PARAM,

  UBEACON_MSG_READ_INTERFACE_PARAM = 62,
  UBEACON_MSG_WRITE_INTERFACE_PARAM = 63,
  UBEACON_MSG_INTERFACE_PARAM = UBEACON_MSG_WRITE_INTERFACE_PARAM,

  UBEACON_MSG_READ_UART_INTERFACE_PARAM = 64,
  UBEACON_MSG_WRITE_UART_INTERFACE_PARAM = 65,
  UBEACON_MSG_UART_INTERFACE_PARAM = UBEACON_MSG_WRITE_UART_INTERFACE_PARAM,

  UBEACON_MSG_READ_IIC_INTERFACE_PARAM = 66,
  UBEACON_MSG_WRITE_IIC_INTERFACE_PARAM = 67,
  UBEACON_MSG_IIC_INTERFACE_PARAM = UBEACON_MSG_WRITE_IIC_INTERFACE_PARAM,

  UBEACON_MSG_LOCATION_RESULT = 68,

  UBEACON_MSG_HEARTBEAT = 78,

  UBEACON_MSG_READ_UWB_INTERFACE_PARAM = 79,
  UBEACON_MSG_WRITE_UWB_INTERFACE_PARAM = 80,
  UBEACON_MSG_UWB_INTERFACE_PARAM = UBEACON_MSG_WRITE_UWB_INTERFACE_PARAM,

  UBEACON_MSG_ANCHOR_SIGNAL = 96,

  UBEACON_MSG_READ_RUN_TIME_PARAM = 100,
  UBEACON_MSG_WRITE_RUN_TIME_PARAM = 101,
  UBEACON_MSG_RUN_TIME_PARAM = UBEACON_MSG_WRITE_RUN_TIME_PARAM,

  UBEACON_MSG_READ_BLE_INTERFACE_PARAM = 102,
  UBEACON_MSG_WRITE_BLE_INTERFACE_PARAM = 103,
  UBEACON_MSG_BLE_INTERFACE_PARAM = UBEACON_MSG_WRITE_BLE_INTERFACE_PARAM,

  UBEACON_MSG_ANCHOR_DDOAS = 104,
} ubeacon_msg_e;

// 表示标签本地时间，单位us，复位后从0开始
typedef int64_t ubeacon_local_time_us_t;

// 设备通信短地址
typedef uint16_t ubeacon_addr_t;

#pragma pack(push, 1)

// 用于重启设备，部分配置参数变更后需要重启才会生效
typedef struct {
  uint8_t delay;                      // 延迟时间
  uint8_t only_restart_when_need : 1; // 仅需要时重启
} UbeaconRestart;

// 用于寻找设备
typedef struct {
  // 设备收到后持续对外提示(如震动，闪灯)的时间
  uint8_t duration;
} UbeaconFind;

// ROM 工作参数
typedef struct {
  uint8_t reserved[4];
  //  标签期望高度，也即标签大部分状态下距离地面的高度 默认1.2m
  float expect_z;
  // 标签期望高度的标准差，高度变化大的标准差也需要调大，但这部分不确定性会影响xy
  // z_noise = 0.01m*_z_noise 默认0.1m
  uint8_t _z_noise;
  // 滤波数据平滑窗口 越大越平滑 但对应延迟也越大 0~5 默认2
  uint8_t smooth_window : 4;
  // 标签大部分状态下的最大加速度，根据标签机动性配置
  // max_acceleration = 0.02m/s^2 * _max_acceleration   默认0.6 0.6 0.02
  uint8_t _max_acceleration[3];
  struct {
    // 输出标签定位结果 默认1
    uint8_t tag_pos : 1;
    // 转发信标发送给后台的数据包
    uint8_t anchor_packet : 1;
    // 输出标签收到的信标坐标信息
    uint8_t anchor_pos : 1;
    // 输出信标之间链路信息
    uint8_t anchor_link_data : 1;
    // 输出标签收到的信标定位信号数据
    uint8_t anchor_signal : 1;
    // 输出标签到信标间的距离差信息
    uint8_t anchor_ddoa : 1;
    // 即使定位结果无效也输出
    uint8_t tag_pos_even_error : 1;
    // 输出信标间链路同步状态，包括location_link mesh_link
    uint8_t anchor_link_status : 1;
  } output;
  // 没有收到信标信号时，周期嗅探的默认占空比 1~100 默认20
  uint8_t sniff_duty_cycle;
  // 前后两次更新的最大间隔时间
  uint8_t update_interval_max;
  // 前后两次更新超过此间隔时间将触发滤波重置
  uint16_t reset_interval;
} UbeaconParam;

// RAM 运行时工作参数
typedef struct {
  // 复位后初始化为Param.sniff_duty_cycle，之后可由用户随时修改
  uint8_t sniff_duty_cycle;
} UbeaconRunTimeParam;

// 定位结果
typedef struct {
  ubeacon_local_time_us_t local_time_us;
  float pos[3];
  int16_t vel_cm[3];
  uint8_t pos_noise_cm[3];
  uint8_t vel_noise_cm[3];
  uint8_t map_id;
  // 如果使能tag_pos_even_error，定位失败仍然会输出结果，但error_code非零
  uint8_t error_code : 4;
  uint8_t area_id : 4;
  uint8_t reserved;
  uint8_t anchor_count : 4;
  uint8_t : 0;
  struct {
    ubeacon_addr_t addr;
    // rx_rssi = _rx_rssi/-2
    uint8_t _rx_rssi;
    // rx_rate = _rx_rate/255
    uint8_t _rx_rate;
  } anchors[9];
} UbeaconLocationResult;

// ROM 标签和外部的通信方式软件配置
// 最终通信方式启用情况由硬件IO配置及软件配置共同决定 或关系 保底启用uwb通信
typedef struct {
  uint8_t uart : 1;
  uint8_t iic : 1;
  uint8_t uwb : 1;
  uint8_t ble : 1;
  uint8_t : 0;
} UbeaconInterfaceParam;

// ROM 串口通信参数
typedef struct {
  // 波特率 默认115200
  uint32_t baudrate;
} UbeaconUartInterfaceParam;

// ROM iic通信参数
typedef struct {
  // 从机地址 默认0x08
  uint8_t addr : 7;
} UbeaconIicInterfaceParam;

// ROM uwb通信参数
typedef struct {
  uint8_t reserved;
  // 发送信号的随机窗口，避免同时发送干扰，默认0.1s
  // random_window = 0.01s * _random_window
  uint8_t _random_window;
  // 两次打开接收的最大间隔时间，为零表示不限制，每次发送信号后都打开接收，默认1s
  // max_rx_interval = 0.05s * _max_rx_interval
  uint8_t _max_rx_interval;
} UbeaconUwbInterfaceParam;

// ROM ble通信参数
typedef struct {
  // 用于填充广播数据帧中company_id及data_type字段
  // 默认 0x06 0x39
  uint8_t company_id[2];
  // 默认0x50
  uint8_t data_type;
  // 发送信号的随机窗口，避免同时发送干扰，默认0.1s
  // random_window = 0.01s * _random_window
  uint8_t _random_window;
  // 每帧数据连续发送次数 默认2
  uint8_t send_count : 3;
} UbeaconBleInterfaceParam;

// 心跳消息，5s定时发送到后台
typedef struct {
  // 电量百分比
  uint8_t battery_percent : 7;
  // 是否充电中
  uint8_t battery_charging : 1;
  // 是否需要重启
  uint8_t need_restart : 1;
  // 是否存在新的重启信息
  uint8_t reset_info_dirty : 1;
  // 是否存在新的断言信息
  uint8_t assert_info_dirty : 1;
  // 任何原因重启，计数都会+1
  uint8_t restart_cnt : 3;
  uint8_t : 0;
  // 用户通过硬件IO选定使能的接口情况
  uint8_t hardware_enabled_uart : 1;
  uint8_t hardware_enabled_iic : 1;
  uint8_t hardware_enabled_uwb : 1;
  uint8_t hardware_enabled_ble : 1;
  uint8_t : 0;
  // 固件系列
  uint8_t firmware_series;
  // 固件版本
  uint8_t firmware_version[4];
  // 唯一id
  uint8_t uid[6];
  // 如使能电池，返回电压 voltage = _battery_voltage/40.0f;
  uint8_t _battery_voltage;
} UbeaconHeartbeat;

// 每次定位对应的所有信标信号数据 变长
typedef struct {
  ubeacon_local_time_us_t local_time_us;
  // 收到的信标数量
  uint8_t count : 4;
  // 对应的区域id
  uint8_t area_id : 4;
  uint8_t reserved;
  struct {
    ubeacon_addr_t addr;
    // fp_index = _fp_index*0.1
    int16_t _fp_index;
    int8_t fp_to_peak;
    // mc = _mc*0.01
    uint8_t _mc;
    // 接收信号强度 rx_rssi = -_rx_rssi*0.5
    uint8_t _rx_rssi;
    // 第一路径信号强度 fp_rssi = -_fp_rssi*0.5
    uint8_t _fp_rssi;
    // uwb_clock_offset_ppm = _uwb_clock_offset_ppm*0.01
    int16_t _uwb_clock_offset_ppm;
    // mcu_clock_offset_ppm = _mcu_clock_offset_ppm*0.01
    int16_t _mcu_clock_offset_ppm;
    // 近期该信标信号的收包率 rx_rate = _rx_rate/255
    uint8_t _rx_rate;
  } datas[9];
} UbeaconAnchorSignal;

// 信标之间的距离差数据
typedef struct {
  ubeacon_local_time_us_t local_time_us;
  uint8_t reserved;
  uint8_t count;
  struct {
    ubeacon_addr_t a0;
    ubeacon_addr_t a1;
    // ddoa = dis_tag_to_a1 - dis_tag_to_a0
    int16_t ddoa_cm;
  } datas[10];
} UbeaconAnchorDdoas;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
