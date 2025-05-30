#include "msg_handle.hpp"
#include "serial_port.hpp"
#include "ubeacon_tag_uart.h"
#include <CLI/CLI.hpp>
#include <asio.hpp>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

struct Args {
  std::string app_name = APP_NAME;
  std::string version = "1.0.0";
  std::string log_level = "info";
  std::string port;
  int baudrate = 115200;

  std::string str() const {
    return fmt::format(
        "app_name={}, version={}, log_level={}, port={}, baudrate={}", app_name,
        version, log_level, port, baudrate);
  }

  void init(int argc, char *argv[]) {
    CLI::App app{app_name};
    app.set_version_flag("-v,--version", version);
    app.add_option(
           "--log_level", log_level,
           R"(specify the log level with ["verbose", "debug", "info", "warning", "error", "critical"])")
        ->check(CLI::IsMember(
            {"verbose", "debug", "info", "warning", "error", "critical"}));
    app.add_option("--port", port, "specify the serial port")->required();
    app.add_option("--baudrate", baudrate, "specify the baud rate");
    try {
      app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
      exit(app.exit(e));
    }
  }
};

void init_log(const std::string &log_level) {
  std::vector<spdlog::sink_ptr> sinks;
  {
    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    sinks.push_back(sink);
  }
  {
    auto output_file_name = fmt::format(
        "logs/{}_{}.log", APP_NAME,
        fmt::format("{:%Y%m%d_%H%M%S}", std::chrono::system_clock::now()));
    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        output_file_name, true);
    sinks.push_back(sink);
  }
  auto logger =
      std::make_shared<spdlog::logger>("", sinks.begin(), sinks.end());
  logger->set_level(spdlog::level::from_str(log_level));
  spdlog::set_default_logger(logger);
  spdlog::flush_every(std::chrono::seconds(1));
}

int main(int argc, char *argv[]) {
  Args args;
  args.init(argc, argv);
  init_log(args.log_level);
  spdlog::info(args.str());

  asio::io_context ioc;

  UbeaconUartFrameParser frame_parser;
  ubeacon_uart_frame_parser_init(
      &frame_parser,
      [](void *, const uint8_t *uid, const uint8_t *payload, int payload_size) {
        // 通过帧校验，分包后的数据
        spdlog::debug("uid: {:02x}, payload: {}B, {:02x}",
                      fmt::join(uid, uid + 6, ""), payload_size,
                      fmt::join(payload, payload + payload_size, " "));
        ubeacon_uart_msg_parser_handle_data(user_msg_cb, nullptr, payload,
                                            payload_size);
      },
      nullptr);

  auto data_ready = [&frame_parser](const std::string &data) {
    // 串口原始数据，字节流，可能有分包或者黏包出现
    spdlog::debug("data rx: {}B, {:02x}", data.size(), fmt::join(data, " "));
    ubeacon_uart_frame_parser_handle_data(&frame_parser, data.data(),
                                          int(data.size()));
  };
  SerialPort serial_port(ioc, args.port, args.baudrate, data_ready);
  ioc.run();
  return 0;
}
