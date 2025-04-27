#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ubeacon_tag_common.h"

// IIC读写的寄存器地址即common中定义的msg_id，
// 但由于IIC协议本身区分读写操作，因此无需在数据中进一步区分读写，
// 例如common中有定义如下
// UBEACON_MSG_READ_PARAM = 60,
// UBEACON_MSG_WRITE_PARAM = 61,
// UBEACON_MSG_PARAM = UBEACON_MSG_WRITE_PARAM,
// 则PARAM数据对应的读写寄存器地址都是UBEACON_MSG_PARAM

#ifdef __cplusplus
}
#endif
