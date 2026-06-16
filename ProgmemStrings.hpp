#ifndef PROGMEMSTRINGS_HPP
#define PROGMEMSTRINGS_HPP

template <int BUF_LEN>
class ProgmemStrings {
  const char * const *_strings;
  int _length;
public:
  ProgmemStrings(const char * const * strings, int length) : _strings(strings), _length(length) { }
  int length() const { return _length; }
  String item(int index) const {
      index = constrain(index, 0, _length);
      char buffer[BUF_LEN];
      strcpy_P(buffer, (char *)pgm_read_word(&(_strings[index])));
      return String(buffer);
  }
  String operator[] (int index) const {
    return item(index);
  }
};

#endif
