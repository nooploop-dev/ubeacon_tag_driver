#include "ubeacon_tag.h"

ubeacon_checksum_t ubeacon_get_sum(const void *data, int data_size) {
  const uint8_t *p = (uint8_t *)data;
  ubeacon_checksum_t sum = 0;
  for (int i = 0; i < data_size; ++i) {
    sum += p[i];
  }
  return sum;
}
