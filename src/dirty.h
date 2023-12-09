#pragma once

#include <Arduino.h>

template <typename T> class Dirty {
 public:
  Dirty(T value) : _value(value), _dirty(false) {};

  T get() { return _value; };
  void set(T value) { _value = value; _dirty = true; };

  void clean() { _dirty = false; };
  bool get_dirty() { return _dirty; };

  const bool& dirty = _dirty;
  const T& value = _value;

  void dump(const char *name) {
    Serial.print("    ");
    Serial.print(name);
    Serial.print(" dirty ");
    Serial.print(this->dirty);
    Serial.print(" / ");
    Serial.print(this->get_dirty());
    Serial.print(" value ");
    Serial.println(this->get());
  };

 private:
  T _value;
  bool _dirty;
};
