#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

class BigInteger {
 public:
  BigInteger() = default;
  BigInteger(long long num) {
    blocks_amount = 1;
    blocks = std::vector<long long>(blocks_amount);
    is_positive = num >= 0;
    blocks[0] = std::abs(num);
  }
  BigInteger(const std::string& num) {
    if (num[0] == '-') {
      is_positive = true;
    } else {
      is_positive = false;
    }
    long long in_blocks_amount = num.size() - is_positive;
    blocks_amount = in_blocks_amount / pr + 1;
    blocks = std::vector<long long>(blocks_amount);
    int counter = 0;
    for (long long i = num.size() - 1; i >= 1 - is_positive; i -= pr) {
      if (i >= pr - 1 + 1 - is_positive) {
        blocks[counter] = std::stoi(num.substr(i - pr + 1, pr));
        counter++;
      } else {
        blocks[counter] = std::stoi(num.substr(1 - is_positive, i + is_positive));
      }
    }
    resize();
    adduction();
  }
  BigInteger(const BigInteger& num) {
    blocks_amount = num.blocks_amount;
    blocks = num.blocks;
    is_positive = num.is_positive;
  }

  BigInteger& operator+=(const BigInteger& num) {
    bool old_sign = is_positive;
    if (is_positive == num.is_positive) {
      sum_unsigned(num);
      is_positive = old_sign;
    } else {
      subtract_unsigned(num);
      if (!old_sign) {
        is_positive = !is_positive;
      }
    }
    resize();
    adduction();
    return *this;
  }
  BigInteger& operator-=(const BigInteger& num) {
    bool old_sign = is_positive;
    if (is_positive == num.is_positive) {
      subtract_unsigned(num);
      if (!old_sign) {
        is_positive = !is_positive;
      }
    } else {
      sum_unsigned(num);
      is_positive = old_sign;
    }
    resize();
    adduction();
    return *this;
  }
  BigInteger& operator*=(const BigInteger& num) {
    std::vector<long long> copy(blocks_amount + num.blocks_amount);
    for (long long i = 0; i < blocks_amount; ++i) {
      for (int j = 0; j < num.blocks_amount; ++j) {
        copy[i + j] += blocks[i] * num.blocks[j];
        if (copy[i + j] > base) {
          copy[i + j + 1] += copy[i + j] / base;
          copy[i + j] %= base;
        }
      }
    }
    is_positive = !(is_positive ^ num.is_positive);
    blocks = copy;
    blocks_amount += num.blocks_amount;
    resize();
    adduction();
    return *this;
  }
  BigInteger& operator/=(const BigInteger& num) {
    divide_unsigned(num);
    is_positive = !(is_positive ^ num.is_positive);
    resize();
    adduction();
    return *this;
  }
  BigInteger& operator%=(const BigInteger& num) {
    blocks = divide_unsigned(num);
    blocks_amount = blocks.blocks_amount();
    resize();
    adduction();
    return *this;
  }
  BigInteger& operator=(const BigInteger& num) {
    blocks_amount = num.blocks_amount;
    blocks = num.blocks;
    is_positive = num.is_positive;
    return *this;
  }
  BigInteger& operator++() {
    *this += 1;
    return *this;
  }
  BigInteger& operator--() {
    *this -= 1;
    return *this;
  }
  BigInteger operator++(int) {
    BigInteger copy = *this;
    ++(*this);
    return copy;
  }
  BigInteger operator--(int) {
    BigInteger copy = *this;
    --(*this);
    return copy;
  }

  explicit operator int() {
    long long result = 0;
    if (blocks_amount > 1) result += blocks[1];
    if (blocks_amount > 0) result += blocks[0] * base;
    return int(result);
  }
  explicit operator long long() {
    return blocks[0];
  }
  explicit operator bool() {
    if (is_positive && blocks_amount > 0 && blocks[0] > 0)
      return true;
    return false;
  }
  friend bool operator<(const BigInteger& num_1, const BigInteger& num_2);
  void clear() {
    blocks.clear();
    blocks_amount = 0;
  }
  std::string toString() const {
    std::string output;
    if (!is_positive) {
      output.push_back('-');
    }
    for (long long i = blocks_amount - 1; i >= 0; --i) {
      auto number = std::to_string(blocks[i]);
      if (i == blocks_amount - 1) {
        output += number;
      } else {
        output += std::string(pr - number.length(), '0') + number;
      }
    }
    return output;
  }
  BigInteger abs() const {
    BigInteger copy = *this;
    copy.is_positive = true;
    return copy;
  }

 private:
  std::vector<long long> blocks; // in reverse order
  long long blocks_amount = 0;
  int base = 1e9;
  bool is_positive = true;
  int pr = 9;
  void adduction() {
    for (long long i = 0; i < blocks_amount; ++i) {
      if (blocks[i] >= base) {
        if (i == blocks_amount - 1) {
          blocks.push_back(0);
          blocks_amount++;
        }
        blocks[i + 1] += blocks[i] / base;
        blocks[i] %= base;
      }
      if (blocks[i] < 0) {
        if (i == blocks_amount - 1) {
          blocks[i] *= -1;
          is_positive = false;
        } else {
          blocks[i] += base;
          blocks[i + 1]--;
        }
      }
    }
    if (blocks_amount == 1 && blocks[0] == 0) {
      is_positive = true;
    }
  }
  void resize() {
    for (long long i = blocks_amount - 1; i > 0; --i) {
      if (blocks[i] == 0) {
        blocks.pop_back();
        blocks_amount--;
      } else {
        i = 0;
      }
    }
  }
  void sum_unsigned(const BigInteger& num) {
    blocks.resize(std::max(blocks_amount, num.blocks_amount) + 1);
    for (long long i = 0; i < num.blocks_amount; ++i) {
      blocks[i] += num.blocks[i];
      if (blocks[i] >= base) {
        blocks[i + 1]++;
        blocks[i] -= base;
      }
    }
    is_positive = true;
    blocks_amount = std::max(blocks_amount, num.blocks_amount) + 1;
  }
  void subtract_unsigned(const BigInteger& num) {
    blocks.resize(std::max(blocks_amount, num.blocks_amount));
    for (long long i = 0; i < num.blocks_amount; ++i) {
      blocks[i] -= num.blocks[i];
      if (blocks[i] < 0 && i != num.blocks_amount - 1) {
        blocks[i] += base;
        blocks[i + 1]--;
      }
    }
    is_positive = true;
    blocks_amount = std::max(blocks_amount, num.blocks_amount);
    if (blocks[blocks_amount - 1] < 0) {
      is_positive = false;
      blocks[blocks_amount - 1] *= -1;
      for (long long i = blocks_amount - 2; i >= 0; --i) {
        blocks[i] = base - blocks[i];
        blocks[i + 1]--;
      }
    }
  }
  std::vector<long long> divide_unsigned(const BigInteger& num) {
    std::vector<long long> carry(num.blocks_amount);
    std::vector<long long> result(blocks_amount);
    std::vector<long long> count_arr;
    long long upper_bound = 0;
    long long bottom_bound = 0;
    long long current_bound = 0;
    for (long long i = 0; i < blocks_amount; ++i) {
      carry.insert(carry.begin(), blocks[blocks_amount - 1 - i]);
      carry[carry.blocks_amount() - 2] += carry[carry.blocks_amount() - 1] * base;
      carry.pop_back();
      upper_bound = carry[carry.blocks_amount() - 1] / num.blocks[num.blocks_amount - 1];
      bottom_bound = carry[carry.blocks_amount() - 1] / (num.blocks[num.blocks_amount - 1] + 1);
      current_bound = (upper_bound + bottom_bound) / 2;
      while (upper_bound != bottom_bound) {
        count_arr = carry;
        for (long long j = 0; j < num.blocks_amount; ++j) {
          count_arr[j] -= current_bound * num.blocks[j];
          if (count_arr[j] < 0) {
            if (j != num.blocks_amount - 1) {
              count_arr[j + 1] -= std::abs(count_arr[j]) / base;
              if (std::abs(count_arr[j]) % base) count_arr[j + 1]--;
              count_arr[j] %= base;
              count_arr[j] += base;
            }
          }
        }
        if (count_arr[num.blocks_amount - 1] < 0) {
          if (upper_bound - bottom_bound == 1) upper_bound = bottom_bound;
          else {
            upper_bound = current_bound;
            current_bound = (bottom_bound + upper_bound) / 2;
          }
        } else {
          if (upper_bound - bottom_bound == 1) {
            if (current_bound == upper_bound) bottom_bound = upper_bound;
            else current_bound = upper_bound;
          } else {
            bottom_bound = current_bound;
            current_bound = (bottom_bound + upper_bound) / 2;
          }
        }
      }
      result[i] = bottom_bound;
      for (long long j = 0; j < num.blocks_amount; ++j) {
        carry[j] -= bottom_bound * num.blocks[j];
        if (carry[j] < 0) {
          carry[j + 1] -= std::abs(carry[j]) / base;
          if (std::abs(carry[j]) % base != 0) carry[j + 1]--;
          carry[j] %= base;
          if (carry[j] < 0) carry[j] += base;
        }
      }
    }
    std::reverse(result.begin(), result.end());
    blocks = result;
    blocks_amount = result.blocks_amount();
    return carry;
  }
};

BigInteger operator+(const BigInteger& num_1, const BigInteger& num_2) {
  BigInteger copy = num_1;
  copy += num_2;
  return copy;
}
BigInteger operator-(const BigInteger& num_1, const BigInteger& num_2) {
  BigInteger copy = num_1;
  copy -= num_2;
  return copy;
}
BigInteger operator*(const BigInteger& num_1, const BigInteger& num_2) {
  BigInteger copy = num_1;
  copy *= num_2;
  return copy;
}
BigInteger operator/(const BigInteger& num_1, const BigInteger& num_2) {
  BigInteger copy = num_1;
  copy /= num_2;
  return copy;
}
BigInteger operator%(const BigInteger& num_1, const BigInteger& num_2) {
  BigInteger copy = num_1;
  copy %= num_2;
  return copy;
}
BigInteger operator-(const BigInteger& num) {
  BigInteger copy = num;
  copy *= -1;
  return copy;
}

bool operator<(const BigInteger& num_1, const BigInteger& num_2) {
  int result = 0;
  // 0 - less
  //1 - more
  if (num_1.is_positive && !num_2.is_positive) {
    return false;
  } else if (!num_1.is_positive && num_2.is_positive) {
    return true;
  } else {
    if (num_2.blocks_amount < num_1.blocks_amount) {
      result = 1;
    } else if (num_2.blocks_amount == num_1.blocks_amount) {
      long long i = num_1.blocks_amount - 1;
      while (num_1.blocks[i] == num_2.blocks[i] && i > 0) --i;
      if (num_1.blocks[i] > num_2.blocks[i]) {
        result = 1;
      } else if (num_1.blocks[i] < num_2.blocks[i]) {
        result = 0;
      } else {
        return false;
      }
    }
    if (!num_1.is_positive) {
      result += 1;
      result %= 2;
    }
    return !result;
  }

}
bool operator>(const BigInteger& num_1, const BigInteger& num_2) {
  return num_2 < num_1;
}
bool operator==(const BigInteger& num_1, const BigInteger& num_2) {
  return !((num_2 < num_1) || (num_1 < num_2));
}
bool operator!=(const BigInteger& num_1, const BigInteger& num_2) {
  return !(num_1 == num_2);
}
bool operator<=(const BigInteger& num_1, const BigInteger& num_2) {
  return (num_1 < num_2) || (num_1 == num_2);
}
bool operator>=(const BigInteger& num_1, const BigInteger& num_2) {
  return (num_1 > num_2) || (num_1 == num_2);
}
std::ostream& operator<<(std::ostream& output, const BigInteger& biggie) {
  output << biggie.toString();
  return output;
}
std::istream& operator>>(std::istream& input, BigInteger& biggie) {
  input.tie(nullptr);
  biggie.clear();
  std::string input_num;
  input >> input_num;
  biggie = BigInteger(input_num);
  return input;
}
//######################################################################################################################
class Rational {
 public:
  Rational() = default;
  Rational(const BigInteger& num) : numerator(num) {}
  Rational(int num) : numerator(num) {}
  Rational& operator+=(const Rational& num) {
    numerator *= num.denominator;
    numerator += num.numerator * denominator;
    denominator *= num.denominator;
    shrink();
    return *this;
  }
  Rational& operator-=(const Rational& num) {
    numerator *= num.denominator;
    numerator -= num.numerator * denominator;
    denominator *= num.denominator;
    shrink();
    return *this;
  }
  Rational& operator*=(const Rational& num) {
    numerator *= num.numerator;
    denominator *= num.denominator;
    shrink();
    return *this;
  }
  Rational& operator/=(const Rational& num) {
    numerator *= num.denominator;
    if (num.numerator < 0) {
      numerator *= -1;
      denominator *= (-num.numerator);
    } else {
      denominator *= num.numerator;
    }
    shrink();
    return *this;
  }
  friend bool operator<(const Rational& num_1, const Rational& num_2);
  explicit operator double() {
    std::string output = asDecimal(20);
    return std::atof(output.c_str());
  }

  std::string toString() const {
    std::string output = numerator.toString();
    if (denominator != 1) {
      output.push_back('/');
      output += denominator.toString();
    }
    return output;
  }
  std::string asDecimal(blocks_amount_t precision = 0) {
    std::string precised;
    BigInteger copy = numerator;
    if (numerator * denominator < 0 && copy / denominator == 0) precised += '-';
    precised += (copy / denominator).toString();
    copy = copy.abs();
    copy %= denominator;
    for (blocks_amount_t i = 0; i <= precision; ++i) {
      if (i == 0) precised.push_back('.');
      else {
        copy *= 10;
        precised += (copy / denominator).toString();
        copy %= denominator;
      }
    }
    return precised;
  }
 private:
  BigInteger numerator;
  BigInteger denominator = 1;

  BigInteger gcd(const BigInteger& num_1, const BigInteger& num_2) {
    if (num_2 == 0) return num_1;
    return gcd(num_2, num_1 % num_2);
  }
  void shrink() {
    BigInteger num = numerator.abs();
    BigInteger den = denominator.abs();
    if (denominator != 1) {
      while (den != 0) {
        std::swap(num, den);
        den %= num;
      }
      BigInteger gcf = num;
      if (gcf != 1) {
        numerator /= gcf;
        denominator /= gcf;
      }
    }
  }
};

Rational operator+(const Rational& num_1, const Rational& num_2) {
  Rational copy = num_1;
  copy += num_2;
  return copy;
}
Rational operator-(const Rational& num_1, const Rational& num_2) {
  Rational copy = num_1;
  copy -= num_2;
  return copy;
}
Rational operator*(const Rational& num_1, const Rational& num_2) {
  Rational copy = num_1;
  copy *= num_2;
  return copy;
}
Rational operator/(const Rational& num_1, const Rational& num_2) {
  Rational copy = num_1;
  copy /= num_2;
  return copy;
}
Rational operator-(const Rational& num_1) {
  Rational copy = num_1;
  copy *= -1;
  return copy;

}

bool operator<(const Rational& num_1, const Rational& num_2) {
  return num_1.numerator * num_2.denominator < num_2.numerator * num_1.denominator;
}
bool operator>(const Rational& num_1, const Rational& num_2) {
  return num_2 < num_1;
}
bool operator==(const Rational& num_1, const Rational& num_2) {
  return !(num_2 < num_1 || num_1 < num_2);
}
bool operator!=(const Rational& num_1, const Rational& num_2) {
  return !(num_1 == num_2);
}
bool operator<=(const Rational& num_1, const Rational& num_2) {
  return !(num_1 > num_2);
}
bool operator>=(const Rational& num_1, const Rational& num_2) {
  return !(num_1 < num_2);
}
std::istream& operator>>(std::istream& input, Rational& rattie) {
  input.tie(nullptr);
  std::string input_num;
  input >> input_num;
  rattie = Rational(input_num);
  return input;
}
std::ostream& operator<<(std::ostream& output, const Rational& biggie) {
  output.tie(nullptr);
  output << biggie.toString();
  return output;
}