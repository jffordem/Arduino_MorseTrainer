#ifndef MORSESTREAM_HPP
#define MORSESTREAM_HPP

#include "AsciiMorse.hpp"

class RepeatStream : public Enumerable<char> {
  String _chars;
  int _pos;
public:
  RepeatStream(const char *text) : _chars(text), _pos(0) { }
  RepeatStream(const String chars = "") : _chars(chars), _pos(0) { }
  void update(String s) { _chars = s; _pos = 0; }
  void reset() { _pos = 0; }
  char current() const {
    // A space between words can have a different length in the Farnsworth Method
    if (_chars[_pos] == ' ') return '/';
    return _chars[_pos];
  }
  void next() { _pos = (_pos + 1) % _chars.length(); }
  bool done() const { return false; }
};

class RandomWords : public Enumerable<char> {
  String _word;
  int _pos;
  int &_level;
public:
  RandomWords(int &level) : _level(level) {
    choose();
  }
  void replay() {
    _pos = 0;
  }
  void choose() {
    int index = constrain(_level, 0, KochLevels-1);
    int limit = KochSeries[index];
    _word = Words[random(0, limit)];
    _word += " ";
    _pos = 0;
  }
  char current() const { return _word[_pos]; }
  void reset() { _pos = 0; }
  void next() {
    if (++_pos >= _word.length()) {
      choose();
    }
  }
  bool done() const { return false; }
  void print() {
    Serial.print("WORDS: ");
    Serial.print(Words.length(), DEC);
    Serial.print(" WORD: ");
    Serial.print(_word);
    Serial.print(" POS: ");
    Serial.print(_pos, DEC);
    Serial.print(" CUR: ");
    Serial.print(current());
    Serial.print(" ");
  }
  void println() {
    print();
    Serial.println();
  }
};

class StringStream : public Enumerable<char> {
  String _chars;
  int _pos;
public:
  StringStream(String chars = "") : _chars(chars), _pos(0) { }
  void update(String s) { _chars = s; _chars += " "; _pos = 0; }
  // String &operator = (const String &s) { _chars = s; _pos = 0; }
  char current() const { return _chars[_pos]; }
  void next() { _pos++; }
  void reset() { _pos = 0; }
  bool done() const { return _pos >= _chars.length(); }
  void print() {
    Serial.print(" CHARS: ");
    Serial.print(_chars);
    Serial.print(" POS: ");
    Serial.print(_pos, DEC);
    Serial.print(" CUR: ");
    Serial.print(current());
    Serial.print(" ");
  }
  void println() {
    print();
    Serial.println();
  }
};

class MorseStream : public Enumerable<char> {
  Enumerable<char> &_text;
  StringStream _char;
public:
  MorseStream(Enumerable<char> &text) : _text(text) {
    _char.update(morse(_text.current()));
  }
  char current() const { return _char.current(); }
  void next() {
    _char.next();
    if (_char.done()) {
      _text.next();
      _char.update(morse(_text.current()));
    }
  }
  void reset() { _text.reset(); _char.reset(); }
  bool done() const { return false; }
  void print() {
    Serial.print(" TEXT: ");
    Serial.print(_text.current());
    Serial.print(" CHAR: ");
    Serial.print(_char.current());
    Serial.print(" CUR: ");
    Serial.print(current());
    Serial.print(" ");
  }
  void println() {
    print();
    Serial.println();
  }
private:
  static String morse(char ch) {
    if (ch >= 0 && ch < AsciiMorse.length()) {
      return AsciiMorse[ch];
    }
    return String(" ");
  }
};

#endif