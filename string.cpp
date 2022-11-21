#include <iostream>
#include <cstring>
#include <algorithm>

class String {
 private:
  char* str = nullptr;
  size_t size = 0;
  size_t max_size = 1;

 public:
  String() = default;
  String(const char* input) {
    size = strlen(input);
    delete[] str;
    while (max_size < size) {
      max_size *= 2;
    }
    str = new char[max_size];
    memcpy(str, input, size);
  }
  String(size_t amount, char c) {
    while (max_size < amount) {
      max_size *= 2;
    }
    str = new char[max_size];
    size = amount;
    memset(str, c, amount);
  }
  String(char c) {
    size = 1;
    max_size = 2;
    str = new char[1];
    str[0] = c;
  }
  String(const String& cpy_string) {
    str = new char[cpy_string.size];
    size = cpy_string.size;
    max_size = cpy_string.max_size;
    memcpy(str, cpy_string.str, cpy_string.size);
  }
  void swap(String& swap_string) {
    std::swap(str, swap_string.str);
    std::swap(size, swap_string.size);
  }
  size_t length() const {
    return size;
  }
  void pop_back() {
    size--;
    if (size <= max_size / 4) {
      max_size /= 2;
      char* new_str = new char[max_size];
      memcpy(new_str, str, size);
      delete[] str;
      str = new_str;
    }
  }
  void push_back(const char c) {
    size++;
    if (size >= max_size) {
      max_size *= 2;
      char* new_str = new char[max_size];
      memcpy(new_str, str, size - 1);
      str = new_str;
    }
    str[size - 1] = c;
  }
  char& front() {
    return str[0];
  }
  const char& front() const {
    return str[0];
  }
  char& back() {
    return str[size - 1];
  }
  const char& back() const {
    return str[size - 1];
  }
  bool empty() const {
    return size <= 0;
  }
  void clear() {
    delete[] str;
    str = nullptr;
    size = 0;
    max_size = 1;
  }
  size_t find(const String& substring) const {
    bool flag = false;
    for (size_t i = 0; i <= size - substring.size; ++i) {
      if (str[i] == substring.str[0]) {
        flag = true;
        for (size_t j = 0; j < substring.size; ++j) {
          if (str[i + j] != substring.str[j]) {
            flag = false;
            break;
          }
        }
        if (flag) {
          return i;
        }
      }
    }
    return size;
  }
  size_t rfind(const String& substring) const {
    bool flag = false;
    for (size_t i = size - substring.size + 1; i > 0; --i) {
      if (str[i] == substring.str[0]) {
        flag = true;
        for (size_t j = 0; j < substring.size; ++j) {
          if (str[i + j] != substring.str[j]) {
            flag = false;
            break;
          }
        }
        if (flag) {
          return i;
        }
      }
    }
    return size;
  }
  String substr(size_t start, size_t count) {
    count = std::min(count, size - start);
    String sub_string;
    for (size_t i = start; i < count + start; ++i) {
      sub_string.push_back(str[i]);
    }
    return sub_string;
  }
  String substr(size_t start, size_t count) const {
    count = std::min(count, size - start);
    String sub_string;
    for (size_t i = start; i < count + start; ++i) {
      sub_string.push_back(str[i]);
    }
    return sub_string;
  }
  friend bool operator==(const String& string, const String& sec_string);
  friend bool operator==(const String& string, const char* word);
  friend bool operator==(const String& string, char symbol);
  friend bool operator==(char symbol, const String& string);
  friend bool operator==(const char* word, const String& string);
  String& operator=(const String& cpy_string) {
    if (str != cpy_string.str) {
      String copy_string = cpy_string;
      swap(copy_string);
    }
    return *this;
  }
  String& operator=(const char input[]) {
    for (size_t i = 0; i < strlen(input); ++i) {
      this->push_back(input[i]);
    }
    return *this;
  }
  char& operator[](const size_t index) {
    return str[index];
  }
  const char& operator[](const size_t index) const {
    return str[index];
  }
  String& operator+=(const String& add_string) {
    if (size + add_string.size >= max_size) {
      max_size = std::max(max_size, add_string.max_size) * 2;
      char* new_str = new char[max_size];
      memcpy(new_str, str, size);
      memcpy(&new_str[size], add_string.str, add_string.size);
      size += add_string.size;
      str = new_str;
    } else {
      memcpy(&str[size], add_string.str, add_string.size);
      size += add_string.size;
    }
    return *this;
  }
  String& operator+=(const char c) {
    push_back(c);
    return *this;
  }
  friend String operator+(const String& first_string, const String& sec_string);
  friend String operator+(const String& first_string, char c);
  friend String operator+(char c, const String& string);
  friend std::ostream& operator<<(std::ostream& output, const String& string);
  friend std::istream& operator>>(std::istream& input, String& string);
  ~String() {
    delete[] str;
  }
};
bool operator==(const String& string, const String& sec_string) {
  if (string.size == sec_string.size) {
    for (size_t i = 0; i < string.size; ++i) {
      if (string.str[i] != sec_string.str[i]) {
        return false;
      }
    }
    return true;
  }
  return false;
}
bool operator==(const String& string, const char* word) {
  if (string.size == strlen(word)) {
    for (size_t i = 0; i < string.size; ++i) {
      if (string.str[i] != word[i]) {
        return false;
      }
    }
    return true;
  }
  return false;
}
bool operator==(const String& string, const char symbol) {
  if (string.size == 1) {
    if (string.str[0] == symbol) return true;
    return false;
  }
  return false;
}
bool operator==(const char symbol, const String& string) {
  if (string.size == 1) {
    if (string.str[0] == symbol) return true;
    return false;
  }
  return false;
}
bool operator==(const char* word, const String& string) {
  if (string.size == strlen(word)) {
    for (size_t i = 0; i < string.size; ++i) {
      if (string.str[i] != word[i]) {
        return false;
      }
    }
    return true;
  }
  return false;
}
String operator+(const String& first_string, const String& sec_string) {
  String new_string(first_string);
  new_string += sec_string;
  return new_string;
}
String operator+(const String& first_string, char c) {
  String new_string(first_string);
  new_string.push_back(c);
  return new_string;
}
String operator+(char c, const String& string) {
  String new_string(1, c);
  new_string += string;
  return new_string;
}
std::ostream& operator<<(std::ostream& output, const String& string) {
  output.tie(nullptr);
  for (size_t i = 0; i < string.size; ++i) {
    output << string.str[i];
  }
  return output;
}
std::istream& operator>>(std::istream& input, String& string) {
  input.tie(nullptr);
  string.clear();
  char symbol;
  do {
    input >> symbol;
    string.push_back(symbol);
  } while (input.peek() != 10 && input.peek() != 32 && input.peek() != -1);
  return input;
}