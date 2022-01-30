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

static inline bool ends_with(const std::string_view  & value, const std::string_view  & ending)
{
  if (ending.size() > value.size()) {
    return false;
  }

  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

class shell : public service {
public:
  typedef std::function<void(const std::vector<std::string> &)> handler_t;

  virtual ~shell() = default;

  virtual void loop() {
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

int main() {
  stdio_init_all();

  shell s;
  s.setup();

  s.register_command("hello", [](const std::vector<std::string> & args) {
    std::cout << "hello !" << std::endl;
    for (const auto & arg : args) {
      std::cout << "arg: " << arg << std::endl;
    }
  });

  while(1) {
    s.loop();
  }
}