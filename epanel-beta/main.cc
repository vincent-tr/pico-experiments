#include "pico/stdlib.h"
#include "common/application.hh"
#include "common/shell.hh"
#include "common/logger.hh"
#include "state.hh"

int main() {
  auto logger_name = "main";

  stdio_init_all();

  mylife::application::init();

  auto app = mylife::application::instance();

  app->register_service("shell", new mylife::shell());
  app->register_service("logger", new mylife::logger());
  app->register_service("state", new mylife::state());

  app->setup();

  while(1) {
    app->loop();
  }

}