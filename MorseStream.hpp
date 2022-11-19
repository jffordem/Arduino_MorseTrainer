#ifndef MORSESTREAM_HPP
#define MORSESTREAM_HPP

class RepeatStream : public Enumerable<char> {
  String _chars;
  int _pos;
public:
  RepeatStream(const char *text) : _chars(text), _pos(0) { }
  RepeatStream(const String chars = "") : _chars(chars), _pos(0) { }
  void update(String s) { _chars = s; _pos = 0; }
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
    int index = max(0, min(_level, KochLevels-1));
    int limit = KochSeries[index];
    int i = random(0, limit);
    i = max(0, min(i, NumWords));
    char buffer[128];
    strcpy_P(buffer, (char *)pgm_read_word(&(Words[i])));
    _word = buffer;
    _word += " ";
    _pos = 0;
  }
  char current() const { return _word[_pos]; }
  void next() {
    if (++_pos >= _word.length()) {
      choose();
    }
  }
  bool done() const { return false; }
  void print() {
    Serial.print("WORDS: ");
    Serial.print(NumWords, DEC);
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
  String _numbers[10] = {
    "-----", ".----", "..---", "...--", "....-", // 0-4
    ".....", "-....", "--...", "---..", "----."  // 5-9
  };
  String _letters[26] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.",   // A-F
    "--.", "....", "..", ".---", "-.-", ".-..", // G-L
    "--", "-.", "---", ".--.", "--.-", ".-.",   // M-R
    "...", "-", "..-", "...-", ".--", "-..-",   // S-X
    "-.--", "--.."                              // Y-Z
  };
  String _period = ".-.-.-";
  String _comma = "--..--";
  String _dash = "-....-";
  String _question = "..--..";
  String _apostrophe = ".----.";
  String _space = " ";
public:
  MorseStream(Enumerable<char> &text) : _text(text) {
    _char.update(lookup(_text.current()));
  }
  char current() const { return _char.current(); }
  void next() {
    _char.next();
    if (_char.done()) {
      _text.next();
      _char.update(lookup(_text.current()));
    }
  }
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
  String lookup(char ch) const {
    if (ch >= 'A' && ch <= 'Z') return _letters[ch - 'A'];
    if (ch >= 'a' && ch <= 'z') return _letters[ch - 'a'];
    if (ch >= '0' && ch <= '9') return _numbers[ch - '0'];
    if (ch == '.') return _period;
    if (ch == ',') return _comma;
    if (ch == '-') return _dash;
    if (ch == '?') return _question;
    return _space;
  }
};

#endif