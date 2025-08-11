#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ubeacon_tag_common.h"

/**
IIC读写的寄存器地址即common中定义的msg_id，
但由于IIC协议本身区分读写操作，因此无需在数据中进一步区分读写，例如common中有定义如下
UBEACON_MSG_READ_PARAM = 60,
UBEACON_MSG_WRITE_PARAM = 61,
UBEACON_MSG_PARAM = UBEACON_MSG_WRITE_PARAM,
则PARAM数据对应的读写寄存器地址都是UBEACON_MSG_PARAM

每次有数据更新时，都会将通知IO拉低至少5us
- 用户可捕获IO下降沿得知数据更新
- 读取MSG_ID为0的数据（长度16字节）
- 遍历所有bit，读到第几个bit为1，则表示MSG_ID为几的数据项有更新
- 继续读取有更新的数据项
*/

#ifdef __cplusplus
}
#endif
