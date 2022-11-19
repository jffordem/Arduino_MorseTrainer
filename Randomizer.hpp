#ifndef RANDOMIZER_HPP
#define RANDOMIZER_HPP

template <class Ta, class Tb>
class Randomizer : public Clock, private EdgeDetectorBase {
  long _waitTime = 15000;
  bool _clockValue = HIGH;
  Ta &_inputA;
  Tb &_inputB;
public:
  Randomizer(PollerComposite &schedule, Ta &inputA, Tb &inputB) :
    Clock(schedule, _waitTime ,_waitTime, _clockValue),
    EdgeDetectorBase(schedule, _clockValue),
    _inputA(inputA), _inputB(inputB) { }
  void onRisingEdge() { }
  void onFallingEdge() {
    unsigned long value = (3 * _inputA + 5 * _inputB + 7 * analogRead(A0)) * (micros() % millis());
    randomSeed(value);
  }
};

#endif