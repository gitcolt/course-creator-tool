#include "input.h"

#include <algorithm>

Input::Input() {}

void Input::register_command(const enum Key key, Command &command) {
  key_states.push_back( { .key = key, .command = command } );
}

void Input::key_down(const enum Key key) {
  auto it = std::find_if(key_states.begin(), key_states.end(),
      [key](auto &ks){ return ks.key == key; });
  if (it == key_states.end())
    return;
  (*it).is_pressed = true;
}

void Input::key_up(const enum Key key) {
  auto it = std::find_if(key_states.begin(), key_states.end(),
      [key](auto &ks){ return ks.key == key; });
  if (it == key_states.end())
    return;
  (*it).is_pressed = false;
}

void Input::process_commands() {
  for (const KeyState &ks : key_states) {
    if (!ks.is_pressed)
      continue;
    ks.command.execute();
  }
}
