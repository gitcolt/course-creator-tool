#ifndef INPUT_H
#define INPUT_H

#include "command.h"

#include <vector>
#include <functional>
#include <unordered_map>
#include <queue>
#include <utility>

#define BIND_GLFW_KEY_TO_INPUT_KEY(triggered_key, glfw_key, input_key) do { \
    if (triggered_key == glfw_key) { \
      if (action == GLFW_PRESS) \
        input.key_down(input_key); \
      else if (action == GLFW_RELEASE) \
        input.key_up(input_key); \
    } \
  } while (0)

enum Key {
  KEY_W,
  KEY_A,
  KEY_S,
  KEY_D,
  KEY_E,
  KEY_Q,
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_ESCAPE,
  NUM_KEYS
};

struct KeyState {
  enum Key key;
  bool is_pressed{false};
  Command &command;
};

class Input {
  public:
    Input();
    void register_command(const enum Key key, Command &command);
    void key_down(const enum Key key);
    void key_up(const enum Key key);
    void process_commands();

  private:
    std::vector<KeyState> key_states{};
};

#endif
