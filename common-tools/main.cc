#include "pico/stdlib.h"
#include "common/application.hh"
#include "common/shell.hh"
#include "common/logger.hh"

int main() {
  auto logger_name = "main";

  stdio_init_all();

  mylife::application::init();

  auto app = mylife::application::instance();

  app->register_service("shell", new mylife::shell());
  app->register_service("logger", new mylife::logger());

  auto *sh = static_cast<mylife::shell *>(app->get_service("shell"));
  sh->register_command("test-log", [&](const std::vector<std::string> & args) {
    DEBUG << "debug msg";
    INFO << "info msg";
    ERROR << "error msg" << 42;
  });

  app->setup();

  while(1) {
    app->loop();
  }

}