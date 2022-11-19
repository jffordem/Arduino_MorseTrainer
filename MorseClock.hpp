#ifndef MORSECLOCK_HPP
#define MORSECLOCK_HPP

class MorseClock : private Scheduled, public Enabled {
  Enumerable<char> &_message;
  Timer _timer;
  int &_charSpeed;
  int &_farnsworthSpeed;
  bool &_value;
  bool _enabled;
public:
  MorseClock(Schedule &schedule, int &charSpeed, int &farnsworthSpeed, Enumerable<char> &message, bool &value) :
    Scheduled(schedule), _charSpeed(charSpeed), _farnsworthSpeed(farnsworthSpeed),
    _message(message), _value(value), _enabled(false) {
    enable(true);
  }
  void enable(bool value) {
    if (_enabled != value) {
      _enabled = value;
      _value = LOW;
      if (_enabled) {
        _timer.reset(ditTime());
      }
    }
  }
  void toggle() { enable(!_enabled); }
  void poll() {
    if (_timer.expired() && _enabled) {
      if (_value == HIGH) {
        long wait = advance();
        _value = LOW;
        _timer.reset(wait);
      } else {
        _value = HIGH;
        if (_message.current() == '-' || _message.current() == '_') {
          _timer.reset(ditTime() * 3);
        } else {
          _timer.reset(ditTime());
        }
      }
    }
  }
private:
  long ditTime() {
    if (_charSpeed <= 0) return 1200;
    return (long)((float)1200/(float)_charSpeed);
  }
  long farnsworthTime() {
    if (_farnsworthSpeed <= 0) return 1200;
    return (long)((float)1200/(float)_farnsworthSpeed);
  }
  long advance() {
    long result = ditTime();
    _message.next();
    while (_message.current() == ' ' || _message.current() == '/') {
      result += (_message.current() == '/' ? 3 : 2) * farnsworthTime();
      _message.next();
    }
    return result;
  }
};

#endif