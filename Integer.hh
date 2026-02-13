#pragma once

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class Integer {
  std::vector<int> digits;
  bool negative;

  void removeLeadingZeros() {
    while (digits.size() > 1 && digits.back() == 0)
      digits.pop_back();
    if (digits.size() == 1 && digits[0] == 0)
      negative = false;
  }

  static int compareAbsolute(const Integer &first, const Integer &second) {
    if (first.digits.size() != second.digits.size())
      return first.digits.size() < second.digits.size() ? -1 : 1;
    for (int index = static_cast<int>(first.digits.size()) - 1; index >= 0;
         --index)
      if (first.digits[index] != second.digits[index])
        return first.digits[index] < second.digits[index] ? -1 : 1;
    return 0;
  }

  static Integer addAbsolute(const Integer &first, const Integer &second) {
    Integer result;
    result.digits.resize(
        std::max(first.digits.size(), second.digits.size()) + 1, 0);
    int carry = 0;
    for (size_t index = 0; index < result.digits.size(); ++index) {
      int sum = carry;
      if (index < first.digits.size())
        sum += first.digits[index];
      if (index < second.digits.size())
        sum += second.digits[index];
      result.digits[index] = sum % 10;
      carry = sum / 10;
    }
    result.removeLeadingZeros();
    return result;
  }

  static Integer subtractAbsolute(const Integer &first, const Integer &second) {
    Integer result;
    result.digits.resize(first.digits.size(), 0);
    int borrow = 0;
    for (size_t index = 0; index < first.digits.size(); ++index) {
      int value = first.digits[index] - borrow -
                  (index < second.digits.size() ? second.digits[index] : 0);
      if (value < 0) {
        value += 10;
        borrow = 1;
      } else
        borrow = 0;
      result.digits[index] = value;
    }
    result.removeLeadingZeros();
    return result;
  }

public:
  Integer() : digits(1, 0), negative(false) {}

  Integer(long long value) : negative(value < 0) {
    unsigned long long absolute =
        (value < 0) ? static_cast<unsigned long long>(-(value + 1)) + 1
                    : static_cast<unsigned long long>(value);
    if (absolute == 0) {
      digits.push_back(0);
      return;
    }
    while (absolute) {
      digits.push_back(absolute % 10);
      absolute /= 10;
    }
  }

  Integer(const std::string &text) : negative(false) {
    if (text.empty()) {
      digits.push_back(0);
      return;
    }
    size_t start = 0;
    if (text[0] == '-') {
      negative = true;
      start = 1;
    } else if (text[0] == '+') {
      start = 1;
    }
    while (start < text.size() - 1 && text[start] == '0')
      ++start;
    for (size_t index = text.size(); index > start; --index) {
      char character = text[index - 1];
      if (character < '0' || character > '9')
        throw std::invalid_argument("Invalid character in Integer");
      digits.push_back(character - '0');
    }
    if (digits.empty())
      digits.push_back(0);
    removeLeadingZeros();
  }

  std::string toString() const {
    std::string result;
    if (negative)
      result += '-';
    for (int index = static_cast<int>(digits.size()) - 1; index >= 0; --index)
      result += static_cast<char>('0' + digits[index]);
    return result;
  }

  bool isZero() const { return digits.size() == 1 && digits[0] == 0; }
  bool isNegative() const { return negative; }

  Integer operator-() const {
    Integer result = *this;
    if (!result.isZero())
      result.negative = !result.negative;
    return result;
  }

  Integer absolute() const {
    Integer result = *this;
    result.negative = false;
    return result;
  }

  bool operator==(const Integer &other) const {
    return negative == other.negative && digits == other.digits;
  }
  bool operator!=(const Integer &other) const { return !(*this == other); }
  bool operator<(const Integer &other) const {
    if (negative != other.negative)
      return negative;
    return negative ? compareAbsolute(*this, other) > 0
                    : compareAbsolute(*this, other) < 0;
  }
  bool operator>(const Integer &other) const { return other < *this; }
  bool operator<=(const Integer &other) const { return !(other < *this); }
  bool operator>=(const Integer &other) const { return !(*this < other); }

  Integer operator+(const Integer &other) const {
    if (negative == other.negative) {
      Integer result = addAbsolute(*this, other);
      result.negative = negative;
      result.removeLeadingZeros();
      return result;
    }
    int comparison = compareAbsolute(*this, other);
    if (comparison == 0)
      return Integer(0LL);
    if (comparison > 0) {
      Integer result = subtractAbsolute(*this, other);
      result.negative = negative;
      result.removeLeadingZeros();
      return result;
    }
    Integer result = subtractAbsolute(other, *this);
    result.negative = other.negative;
    result.removeLeadingZeros();
    return result;
  }

  Integer operator-(const Integer &other) const { return *this + (-other); }

  Integer operator*(const Integer &other) const {
    Integer result;
    result.digits.resize(digits.size() + other.digits.size(), 0);
    for (size_t i = 0; i < digits.size(); ++i) {
      int carry = 0;
      for (size_t j = 0; j < other.digits.size() || carry; ++j) {
        long long current =
            result.digits[i + j] + static_cast<long long>(carry) +
            (j < other.digits.size()
                 ? static_cast<long long>(digits[i]) * other.digits[j]
                 : 0LL);
        result.digits[i + j] = static_cast<int>(current % 10);
        carry = static_cast<int>(current / 10);
      }
    }
    result.negative = negative != other.negative;
    result.removeLeadingZeros();
    return result;
  }

  friend std::pair<Integer, Integer>
  divideWithRemainder(const Integer &dividend, const Integer &divisor);

  Integer operator/(const Integer &other) const {
    return divideWithRemainder(*this, other).first;
  }
  Integer operator%(const Integer &other) const {
    return divideWithRemainder(*this, other).second;
  }
};

inline std::pair<Integer, Integer> divideWithRemainder(const Integer &dividend,
                                                       const Integer &divisor) {
  if (divisor.isZero())
    throw std::domain_error("Division by zero");

  Integer absoluteDividend = dividend.absolute();
  Integer absoluteDivisor = divisor.absolute();
  if (absoluteDividend < absoluteDivisor)
    return {Integer(0LL), dividend};

  Integer quotient, remainder;
  quotient.digits.resize(absoluteDividend.digits.size(), 0);

  for (int index = static_cast<int>(absoluteDividend.digits.size()) - 1;
       index >= 0; --index) {
    remainder.digits.insert(remainder.digits.begin(), 0);
    remainder.digits[0] = absoluteDividend.digits[index];
    remainder.removeLeadingZeros();

    int low = 0, high = 9, digit = 0;
    while (low <= high) {
      int middle = (low + high) / 2;
      if (absoluteDivisor * Integer(static_cast<long long>(middle)) <=
          remainder) {
        digit = middle;
        low = middle + 1;
      } else {
        high = middle - 1;
      }
    }
    quotient.digits[index] = digit;
    remainder =
        remainder - absoluteDivisor * Integer(static_cast<long long>(digit));
  }

  quotient.negative = dividend.isNegative() != divisor.isNegative();
  remainder.negative = dividend.isNegative();
  quotient.removeLeadingZeros();
  remainder.removeLeadingZeros();
  return {quotient, remainder};
}

inline Integer power(const Integer &base, const Integer &exponent) {
  if (exponent.isNegative())
    throw std::domain_error("Negative exponent");
  if (exponent.isZero())
    return Integer(1LL);
  std::string exponentString = exponent.toString();
  if (exponentString.size() > 7)
    throw std::domain_error("Exponent too large");
  long long exponentValue = std::stoll(exponentString);
  if (exponentValue > 10000)
    throw std::domain_error("Exponent too large");
  Integer result(1LL), current = base;
  while (exponentValue > 0) {
    if (exponentValue & 1)
      result = result * current;
    current = current * current;
    exponentValue >>= 1;
  }
  return result;
}

inline Integer factorial(const Integer &number) {
  if (number.isNegative())
    throw std::domain_error("Factorial of negative number");
  std::string numberString = number.toString();
  if (numberString.size() > 5)
    throw std::domain_error("Number too large for factorial");
  long long value = std::stoll(numberString);
  if (value > 10000)
    throw std::domain_error("Factorial argument too large");
  Integer result(1LL);
  for (long long i = 2; i <= value; ++i)
    result = result * Integer(i);
  return result;
}

inline Integer gcd(const Integer &first, const Integer &second) {
  Integer x = first.absolute(), y = second.absolute();
  while (!y.isZero()) {
    Integer temporary = x % y;
    x = y;
    y = temporary;
  }
  return x;
}

inline Integer lcm(const Integer &first, const Integer &second) {
  if (first.isZero() || second.isZero())
    return Integer(0LL);
  return first.absolute() / gcd(first, second) * second.absolute();
}

inline bool isPrime(const Integer &number) {
  if (number.isNegative() || number <= Integer(1LL))
    return false;
  if (number == Integer(2LL) || number == Integer(3LL))
    return true;
  if ((number % Integer(2LL)).isZero())
    return false;
  std::string numberString = number.toString();
  if (numberString.size() > 12)
    throw std::domain_error("Number too large for primality check");
  long long value = std::stoll(numberString);
  for (long long i = 3; i * i <= value; i += 2)
    if (value % i == 0)
      return false;
  return true;
}
