#include "catch2/catch_all.hpp"
#include "ubeacon_tag_uart.h"

TEST_CASE("checksum") {
  ubeacon_checksum_t sum = 0x08;
  constexpr auto DATA_SIZE = 15;
  constexpr auto TOTAL_SIZE = DATA_SIZE + sizeof(ubeacon_checksum_t);
  uint8_t data[TOTAL_SIZE] = {0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
                              0x89, 0x9A, 0xAB, 0xBC, 0xCD, 0xDE, 0xEF};
  REQUIRE(ubeacon_get_sum(data, DATA_SIZE) == sum);
  ubeacon_update_sum(data, TOTAL_SIZE);
  REQUIRE(ubeacon_verify_sum(data, TOTAL_SIZE));
}
