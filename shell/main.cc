#include <iostream>
#include <sstream>
#include <functional>
#include <iterator>
#include <string>
#include <vector>
#include "pico/stdlib.h"

class service {
public:
  virtual ~service() = default;

  virtual void setup() {
  }

  virtual void loop() {
  }
};

class application {
  application() = default;

public:
  void register_service(const std::string_view &name, service *svc) {
    m_services.emplace(name, svc);
  }

  service *get_service(const std::string_view &name) {
    auto it = m_services.find(name);
    assert(it != m_services.end());
    return it->second;
  }


  void setup() {
    for (const auto &[name, svc] : m_services) {
      svc->setup();
    }
  }

  void loop() {
    for (const auto &[name, svc] : m_services) {
      svc->loop();
    }
  }

  static application* instance() {
    return s_instance;
  }

  static void init() {
    s_instance = new application();
  }

private:
  static application *s_instance;
  std::unordered_map<std::string_view, service *> m_services;
};

application *application::s_instance = nullptr;

static inline bool ends_with(const std::string_view &value, const std::string_view &ending) {
  if (ending.size() > value.size()) {
    return false;
  }

  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

class shell : public service {
public:
  typedef std::function<void(const std::vector<std::string> &)> handler_t;

  virtual ~shell() = default;

  virtual void setup() override {
    register_command("help", [&](const std::vector<std::string> & args) {
      std::cout << "Commands are:" << std::endl;
      for (const auto & [name, handler] : m_handlers) {
        std::cout << name << std::endl;
      }
    });
  }

  virtual void loop() override {
    int value = getchar_timeout_us(0);
    if (value == PICO_ERROR_TIMEOUT) {
      return;
    }

    char c = static_cast<char>(value & 0xFF);
    std::cout << c << std::flush;

    if (c != '\r') {
      m_current_line += c;
      return;
    }

    std::cout << std::endl;

    execute(m_current_line);

    m_current_line = "";
  }

  void register_command(std::string_view cmd, handler_t handler) {
    m_handlers.emplace(std::move(cmd), std::move(handler));
  }

private:
  void execute(const std::string &line) const {
    std::istringstream iss(line);
    std::vector<std::string> tokens;

    std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_insert_iterator(tokens));

    if (tokens.size() == 0) {
      std::cout << "No command provided." << std::endl;
      return;
    }

    const auto &cmd = tokens.front();

    auto it = m_handlers.find(cmd);
    if (it == m_handlers.end()) {
      std::cout << "Unknown command '" << cmd << "'. Commands are:" << std::endl;
      for (const auto & [name, handler] : m_handlers) {
        std::cout << name << std::endl;
      }
      return;
    }

    tokens.erase(tokens.begin());
    it->second(tokens);
  }

  std::unordered_map<std::string_view, handler_t> m_handlers;
  std::string m_current_line;
};

class logger : public service {
public:
  enum class level { debug, info, error };

  virtual ~logger() = default;

  virtual void setup() override {
    auto *sh = static_cast<shell *>(application::instance()->get_service("shell"));

    sh->register_command("logger-level-get", [&](const std::vector<std::string> & args) {
      std::cout << "Current logger level is '";

      switch (m_level) {
      case level::debug:
        std::cout << "debug";
        break;

      case level::info:
        std::cout << "info";
        break;

      case level::error:
        std::cout << "error";
        break;
      }

      std::cout << "'" << std::endl;
    });

    sh->register_command("logger-level-set", [&](const std::vector<std::string> & args) {
      if (args.size() < 1) {
        std::cout << "No level provided." << std::endl;
      }

      const auto &slevel = args.front();

      if (slevel == "debug") {
        m_level = level::debug;
        std::cout << "Logger level set to  'debug'" << std::endl;
      } else if (slevel == "info") {
        m_level = level::info;
        std::cout << "Logger level set to  'info'" << std::endl;
      } else if (slevel == "error") {
        m_level = level::error;
        std::cout << "Logger level set to  'error'" << std::endl;
      } else {
        std::cout << "Invalid level provided '" << slevel << "'" << std::endl;
      }
    });
  }

private:
  level m_level = level::error;
};

int main() {
  stdio_init_all();

  application::init();

  auto app = application::instance();

  app->register_service("shell", new shell());
  app->register_service("logger", new logger());

  app->setup();

  while(1) {
    app->loop();
  }

}