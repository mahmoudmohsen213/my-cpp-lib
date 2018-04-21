#ifndef BIG_INTEGER_H_
#define BIG_INTEGER_H_

#include <cstdlib>
#include <cctype>
#include <string>
#include <stdexcept>

#include "my_bitset.h"

class big_integer {
private:
  // sign = -1, if negative number
  // sign = 1, if positive number
  signed char sign;
  my_bitset mag;

  my_bitset static add(const my_bitset &op1, const my_bitset &op2);
  my_bitset static sub(const my_bitset &op1, const my_bitset &op2);
  my_bitset static mul(const my_bitset &op1, const my_bitset &op2);
  my_bitset static div_and_mod(
      const my_bitset &op1,
      const my_bitset &op2,
      const int &out_type);
  unsigned char static * add_base10(
      const unsigned char * op1,
      const unsigned char * op2,
      int size_in1, int size_in2, int &size_out);

public:
  big_integer();
  // copy constructor
  big_integer(const big_integer &value);
  // numerical constructor (base 10)
  big_integer(const long long &value);
  big_integer(const unsigned long long &value, const signed char &sign);
  // string constructor (base 10)
  big_integer(const std::string &value);
  ~big_integer();

  big_integer& operator = (const big_integer &value);
  big_integer& operator = (const long long &value);

  big_integer operator + (const big_integer &_big_integer) const;
  big_integer operator - (const big_integer &_big_integer) const;
  big_integer operator * (const big_integer &_big_integer) const;
  big_integer operator / (const big_integer &_big_integer) const;
  big_integer operator % (const big_integer &_big_integer) const;

  big_integer operator & (const big_integer &_big_integer) const;
  big_integer operator | (const big_integer &_big_integer) const;
  big_integer operator ^ (const big_integer &_big_integer) const;
  big_integer operator ~ () const;
  big_integer operator << (const int &places) const;
  big_integer operator >> (const int &places) const;

  big_integer pow(unsigned long long exp) const;
  big_integer pow_mod(const big_integer &exp, const big_integer &mod) const;
  big_integer gcd(const big_integer &_big_integer) const;
  big_integer mod_inverse(const big_integer &_big_integer) const;

  big_integer static gcd_extended(
      const big_integer &a,
      const big_integer &b,
      big_integer &x,
      big_integer &y);

  bool operator == (const big_integer &_big_integer) const;
  bool operator != (const big_integer &_big_integer) const;
  bool operator > (const big_integer &_big_integer) const;
  bool operator >= (const big_integer &_big_integer) const;
  bool operator < (const big_integer &_big_integer) const;
  bool operator <= (const big_integer &_big_integer) const;

  std::string to_string() const;
  std::string to_bin_string() const;
  std::string to_oct_string() const;
  std::string to_dec_string() const;
  std::string to_hex_string() const;
  long long to_llong() const;
};

///////////////////////////////////////

/*
my_bitset big_integer::add(const my_bitset &op1, const my_bitset &op2){
  int max_size = ((op1.size() >= op2.size()) ? op1.size() : op2.size());

  if(max_size == 0) return my_bitset(0);

  my_bitset result(max_size, 0);
  my_bitset _op1 = op1.pad_left(result.size() - op1.size(), 0);
  my_bitset _op2 = op2.pad_left(result.size() - op2.size(), 0);

  // full binary adder processing one bit at a time
  bool carry = 0;
  for(int i = max_size - 1; i >= 0; --i){
    bool bit1 = _op1.get(i), bit2 = _op2.get(i);
    result.set(i, ((bit1 ^ bit2) ^ carry));
    carry = ((bit1 & bit2) | (carry & (bit1 ^ bit2)));
  }

  if(carry)
    result = result.pad_left(1, 0), result.set_word(0, 1);

  if((result.words_count() > 0) && (result.get_word(0) == 0))
    result = result.trim_left();

  return result;
}
*/

my_bitset big_integer::add(const my_bitset &op1, const my_bitset &op2){
  int max_bit_size = ((op1.size() >= op2.size()) ? op1.size() : op2.size());

  if(max_bit_size == 0) return my_bitset(0);

  my_bitset result(max_bit_size, 0);
  my_bitset _op1 = op1.pad_left(result.size() - op1.size(), 0);
  my_bitset _op2 = op2.pad_left(result.size() - op2.size(), 0);
  int words_count = _op1.words_count();
  int word_size = my_bitset::get_word_size();

  // full binary adder processing one word at a time
  unsigned long long carry = 0, sum = 0, base = 1;
  base <<= word_size;
  unsigned long long mod_mask = base - 1;
  for(int i = words_count - 1; i >= 0; --i){
    sum = carry + ((unsigned long long)_op1.get_word(i)) + ((unsigned long long)_op2.get_word(i));
    result.set_word(i, sum & mod_mask);
    carry = sum >> word_size;
  }

  if(carry)
    result = result.pad_left(1, 0), result.set_word(0, carry);

  if((result.words_count() > 0) && (result.get_word(0) == 0))
    result = result.trim_left();

  return result;
}

/*
my_bitset big_integer::sub(const my_bitset &op1, const my_bitset &op2){
  int max_size = ((op1.size() >= op2.size()) ? op1.size() : op2.size());

  if(max_size == 0) return my_bitset(0);
  if(op1 == op2) return my_bitset(0);

  my_bitset result(max_size, 0);
  my_bitset _op1 = op1.pad_left(result.size() - op1.size(), 0);
  my_bitset _op2 = op2.pad_left(result.size() - op2.size(), 0);

  // full binary subtracter processing one bit at a time
  bool borrow = 0;
  for(int i = max_size - 1; i >= 0; --i){
    bool bit1 = _op1.get(i), bit2 = _op2.get(i);
    result.set(i, ((bit1 ^ bit2) ^ borrow));
    borrow = (((!bit1) & bit2) | (borrow & (!(bit1 ^ bit2))));
  }

  if(borrow)
    throw std::runtime_error("big_integer::sub: op1 is less than op2");

  if((result.words_count() > 0) && (result.get_word(0) == 0))
    result = result.trim_left();

  return result;
}
*/

my_bitset big_integer::sub(const my_bitset &op1, const my_bitset &op2){
  int max_size = ((op1.size() >= op2.size()) ? op1.size() : op2.size());

  if(max_size == 0) return my_bitset(0);
  if(op1 == op2) return my_bitset(0);

  my_bitset result(max_size, 0);
  my_bitset _op1 = op1.pad_left(result.size() - op1.size(), 0);
  my_bitset _op2 = op2.pad_left(result.size() - op2.size(), 0);
  int words_count = _op1.words_count();
  int word_size = my_bitset::get_word_size();

  // full binary subtracter processing one word at a time
  long long borrow = 0, diff = 0, base = 1;
  base <<= word_size;
  long long mod_mask = base - 1;
  for(int i = words_count - 1; i >= 0; --i){
    diff = ((long long)_op1.get_word(i)) - ((long long)_op2.get_word(i)) - borrow;
    borrow = (long long)(diff < 0);
    result.set_word(i, (diff + base) & mod_mask);
  }

  if(borrow)
    throw std::runtime_error("big_integer::sub: op1 is less than op2");

  if((result.words_count() > 0) && (result.get_word(0) == 0))
    result = result.trim_left();

  return result;
}

my_bitset big_integer::mul(const my_bitset &op1, const my_bitset &op2){
  if((op1.words_count() <= (int)((sizeof(unsigned long long)) >> 2)) &&
      (op2.words_count() <= (int)((sizeof(unsigned long long)) >> 2))) {
    return my_bitset(op1.to_ullong() * op2.to_ullong()).trim_left();
  }

  // Karatsuba multiplication algorithm
  // let the two operands are x and y
  // let Xh = left half of x (highest order digits)
  // let Xl = right half of x (lowest order digits)
  // let Yh = left half of y (highest order digits)
  // let Yl = right half of y (lowest order digits)
  // let a = Xh*Yh
  // let d = Xl*Yl
  // let r = the base of the operands
  // let n = the number of digits in the operands
  // let e = ((Xh+Xl)*(Yh+Yl)) - a - d
  // then X*y = (a*(r^n)) + (e*(r^(n/2))) + d
  // as the numbers are in binary (base 2)
  // mul by any power of is a left shift

  int bit_size = ((op1.size() >= op2.size()) ? op1.size() : op2.size());
  my_bitset _op1 = op1.pad_left(bit_size - op1.size(), 0);
  my_bitset _op2 = op2.pad_left(bit_size - op2.size(), 0);

  // my_bitset high_mask(bit_size, 1);
  // high_mask = high_mask << (bit_size>>1);

  my_bitset low_mask(bit_size, 1);
  low_mask = low_mask >> (bit_size >> 1);

  my_bitset Xh = (_op1 >> (bit_size>>1)).trim_left((bit_size >> 1) / my_bitset::get_word_size());
  my_bitset Xl = (_op1 & low_mask).trim_left((bit_size >> 1) / my_bitset::get_word_size());

  my_bitset Yh = (_op2 >> (bit_size>>1)).trim_left((bit_size >> 1) / my_bitset::get_word_size());
  my_bitset Yl = (_op2 & low_mask).trim_left((bit_size >> 1) / my_bitset::get_word_size());

  my_bitset a = big_integer::mul(Xh, Yh);
  my_bitset d = big_integer::mul(Xl, Yl);
  my_bitset XhplusXl = big_integer::add(Xh, Xl);
  my_bitset YhplusYl = big_integer::add(Yh, Yl);
  my_bitset XhplusXlmulYhplusYl = big_integer::mul(XhplusXl, YhplusYl);
  my_bitset e = big_integer::sub(XhplusXlmulYhplusYl, a);
  e = big_integer::sub(e, d);
  a = (a.pad_left(bit_size, 0) << bit_size);
  e = (e.pad_left(bit_size, 0) << (bit_size >> 1));
  my_bitset result = big_integer::add(big_integer::add(a, e), d);

  if((result.words_count() > 0) && (result.get_word(0) == 0))
    result = result.trim_left();
  return result;
}

my_bitset big_integer::div_and_mod(const my_bitset &op1, const my_bitset &op2, const int &out_type){
  // integer division (unsigned) with remainder
  // initialize quotient q and remainder r to zero
  // for every bit in op1 starting from the most significant bit
  // left-shift R by 1 bit
  // set the least significant bit of r equal to bit i of the numerator op1
  // if r >= op2
  // set r = r - op2
  // set the bit i of q to 1

  const unsigned char NEED_TO_PAD_THRESHOLD = 1 << 7;
  my_bitset q(op1.size(), 0), r(0);

  for(int i = 0; i < op1.size(); ++i){
    if(r.get_word(0) >= NEED_TO_PAD_THRESHOLD)
      r = r.pad_left(r.size(), 0);

    r = r << 1;
    r.set(r.size() - 1, op1.get(i));
    if(r >= op2){
      r = big_integer::sub(r, op2);
      q.set(i, 1);
    }
  }

  if((q.words_count() > 0) && (q.get_word(0) == 0) && (out_type == 0)) q = q.trim_left();
  if((r.words_count() > 0) && (r.get_word(0) == 0) && (out_type != 0)) r = r.trim_left();

  if(out_type == 0) return q;
  else return r;
}

unsigned char * big_integer::add_base10(
    const unsigned char * op1,
    const unsigned char * op2,
    int size_in1, int size_in2, int &size_out){

  size_out = ((size_in1 >= size_in2) ? size_in1 : size_in2);
  int min_size = ((size_in1 <= size_in2) ? size_in1 : size_in2);
  unsigned char * result = new unsigned char[size_out];
  unsigned char carry = 0;
  memset(result, 0, size_out);

  for(int i = 1; i <= min_size; ++i){
    result[size_out - i] = carry + op1[size_in1 - i] + op2[size_in2 - i];
    carry = result[size_out - i] / 10;
    result[size_out - i] %= 10;
  }

  for(int i = min_size + 1; i <= size_in1; ++i){
    result[size_out - i] = carry + op1[size_in1 - i];
    carry = result[size_out - i] / 10;
    result[size_out - i] %= 10;
  }

  for(int i = min_size + 1; i <= size_in2; ++i){
    result[size_out - i] = carry + op2[size_in2 - i];
    carry = result[size_out - i] / 10;
    result[size_out - i] %= 10;
  }

  if(carry) {
    unsigned char * temp = new unsigned char[size_out + 1];
    memcpy(temp + 1, result, size_out);
    delete[] result;
    result = temp, temp = 0;
    result[0] = 1, size_out += 1;
  }

  return result;
}

///////////////////////////////////////

big_integer::big_integer(){
  this->sign = 0;
  this->mag = 0;
}

// copy constructor
big_integer::big_integer(const big_integer &value){
  this->sign = value.sign;
  this->mag = value.mag.trim_left();
}

// numerical constructor (base 10)
big_integer::big_integer(const long long &value){
  this->sign = ((value == 0) ? 0 : ((value < 0) ? -1 : 1));
  this->mag = my_bitset(value * this->sign).trim_left();
}

big_integer::big_integer(const unsigned long long &value, const signed char &sign){
  this->sign = ((sign == 0) ? 0 : ((sign < 0) ? -1 : 1));
  this->mag = my_bitset(value).trim_left();
}

// string constructor (base 10)
big_integer::big_integer(const std::string &value){
  int cursor = 0;
  if((value.size() - cursor) == 0)
    throw std::runtime_error("big_integer::big_integer: invalid value string: empty magnitude");

  big_integer base(10);
  big_integer result(0);

  if(value[0] == '-')
    cursor += 1;

  if((value.size() - cursor) == 0)
    throw std::runtime_error("big_integer::big_integer: invalid value string: empty magnitude");

  if(value.find('-', cursor) != std::string::npos)
    throw std::runtime_error("big_integer::big_integer: invalid value string: multiple negative sign characters");

  // converting a big integer string representation to the internal binary
  // representation. consider the input string as a set of groups, each group
  // has at most 18 digits the magnitude of each group is the long long value
  // of each group multiplied by 10^(the index of that group from the end of
  // the input text), the result is the summation of the magnitude of all groups.
  // for example:
  // let the input string is 98745632158, and let our group size is 3
  // then the numerical value of such string is:
  // 987*(10 pow 8) +
  // 456*(10 pow 5) +
  // 321*(10 pow 2) +
  // 58*(10 pow 0)
  // since each group has at most 18 decimal digits, it can fit in an unsigned
  // long long which can be used to construct a big integer easily, then we have
  // another big integer initialized to 10, and can be used to compute 10 pow any
  // value up to 18446744073709551615, then the two big integers are used to
  // compute the magnitude of each group and the result is added to the running total.

  // the maximum number of decimal digits that fits in less than 2^63
  int max_digits_per_group = 18;
  while(cursor < (int)value.size()){
    unsigned long long group = 0;
    for(int i = 0; (i < max_digits_per_group) && (cursor < (int)value.size()); ++i, ++cursor){
      if(!isdigit(value[cursor]))
        throw std::runtime_error("big_integer::big_integer: invalid value string: non decimal digit character");

      group *= 10, group += ((long long)value[cursor]) - ((long long)'0');
    }

    result = result + (big_integer(group) * base.pow(value.size() - cursor));
  }

  if(value[0] == '-')
    result.sign = -1;

  if((result.mag.words_count() > 0) && (result.mag.get_word(0) == 0))
      result.mag = result.mag.trim_left();

  if(result.mag.size() == 0) result.sign = 0;

  this->sign = result.sign;
  this->mag = result.mag.trim_left();
}

big_integer::~big_integer() { }

big_integer& big_integer::operator = (const big_integer &value){
  this->sign = value.sign;
  this->mag = value.mag;
  return (*this);
}

big_integer& big_integer::operator = (const long long &value){
  this->sign = ((value == 0) ? 0 : ((value < 0) ? -1 : 1));
  this->mag = (value * this->sign);
  return (*this);
}

big_integer big_integer::operator + (const big_integer &_big_integer) const{
  if(this->sign == 0)
    return _big_integer;

  if(_big_integer.sign == 0)
    return (*this);

  if(this->sign == _big_integer.sign) {
    big_integer result;
    result.sign = this->sign;
    result.mag = big_integer::add(this->mag, _big_integer.mag);
    if(result.mag.size() == 0) result.sign = 0;
    return result;
  }

  int mag_comparison = my_bitset::compare(this->mag, _big_integer.mag);

  if(mag_comparison == -1) {
    big_integer result;
    result.sign = this->sign;
    result.mag = big_integer::sub(this->mag, _big_integer.mag);
    if(result.mag.size() == 0) result.sign = 0;
    return result;
  }

  if(mag_comparison == 1) {
    big_integer result;
    result.sign = _big_integer.sign;
    result.mag = big_integer::sub(_big_integer.mag, this->mag);
    if(result.mag.size() == 0) result.sign = 0;
    return result;
  }

  return 0;
}

big_integer big_integer::operator - (const big_integer &_big_integer) const{
  big_integer _big_integer2(_big_integer);
  _big_integer2.sign *= -1;
  return this->operator + (_big_integer2);
}

big_integer big_integer::operator * (const big_integer &_big_integer) const{
  if(this->sign == 0 || _big_integer.sign == 0)
    return 0;

  big_integer result;
  result.sign = this->sign * _big_integer.sign;
  result.mag = big_integer::mul(this->mag, _big_integer.mag);
  if(result.mag.size() == 0) result.sign = 0;
  return result;
}

big_integer big_integer::operator / (const big_integer &_big_integer) const{
  if(_big_integer.sign == 0)
    throw std::runtime_error("big_integer::operator /: integer division by 0");

  if(this->sign == 0)
    return 0;

  big_integer result;
  result.sign = this->sign * _big_integer.sign;
  result.mag = big_integer::div_and_mod(this->mag, _big_integer.mag, 0);
  if(result.mag.size() == 0) result.sign = 0;
  return result;
}

big_integer big_integer::operator % (const big_integer &_big_integer) const{
  if(_big_integer.sign == 0)
    throw std::runtime_error("big_integer::operator %: integer division by 0");

  if(this->sign == 0)
    return 0;

  big_integer result;
  result.sign = this->sign;
  result.mag = big_integer::div_and_mod(this->mag, _big_integer.mag, 1);
  if(result.mag.size() == 0) result.sign = 0;
  return result;
}

big_integer big_integer::operator & (const big_integer &_big_integer) const{
  int max_bit_size = ((this->mag.size() >= _big_integer.mag.size()) ?
      this->mag.size() : _big_integer.mag.size());

  if(max_bit_size == 0) return big_integer(0);

  my_bitset _op1 = this->mag.pad_left(max_bit_size - this->mag.size(), 0);
  my_bitset _op2 = _big_integer.mag.pad_left(max_bit_size - _big_integer.mag.size(), 0);
  big_integer result;
  result.sign = this->sign;
  result.mag = (_op1 & _op2);

  if((result.mag.words_count() > 0) && (result.mag.get_word(0) == 0))
      result.mag = result.mag.trim_left();

  if(result.mag.size() == 0) result.sign = 0;
  return result;
}

big_integer big_integer::operator | (const big_integer &_big_integer) const{
  int max_bit_size = ((this->mag.size() >= _big_integer.mag.size()) ?
      this->mag.size() : _big_integer.mag.size());

  if(max_bit_size == 0) return big_integer(0);

  my_bitset _op1 = this->mag.pad_left(max_bit_size - this->mag.size(), 0);
  my_bitset _op2 = _big_integer.mag.pad_left(max_bit_size - _big_integer.mag.size(), 0);
  big_integer result;
  result.sign = this->sign;
  result.mag = (_op1 | _op2);

  if((result.mag.words_count() > 0) && (result.mag.get_word(0) == 0))
      result.mag = result.mag.trim_left();

  if(result.mag.size() == 0) result.sign = 0;
  return result;
}

big_integer big_integer::operator ^ (const big_integer &_big_integer) const{
  int max_bit_size = ((this->mag.size() >= _big_integer.mag.size()) ?
      this->mag.size() : _big_integer.mag.size());

  if(max_bit_size == 0) return big_integer(0);

  my_bitset _op1 = this->mag.pad_left(max_bit_size - this->mag.size(), 0);
  my_bitset _op2 = _big_integer.mag.pad_left(max_bit_size - _big_integer.mag.size(), 0);
  big_integer result;
  result.sign = this->sign;
  result.mag = (_op1 ^ _op2);

  if((result.mag.words_count() > 0) && (result.mag.get_word(0) == 0))
      result.mag = result.mag.trim_left();

  if(result.mag.size() == 0) result.sign = 0;
  return result;
}

big_integer big_integer::operator ~ () const{
  big_integer result;
  result.sign = this->sign;
  result.mag = ~(this->mag);

  if((result.mag.words_count() > 0) && (result.mag.get_word(0) == 0))
      result.mag = result.mag.trim_left();

  if(result.mag.size() == 0) result.sign = 0;
  return result;
}

big_integer big_integer::operator << (const int &places) const{
  big_integer result;
  result.sign = this->sign;
  result.mag = (this->mag) << places;

  if((result.mag.words_count() > 0) && (result.mag.get_word(0) == 0))
      result.mag = result.mag.trim_left();

  if(result.mag.size() == 0) result.sign = 0;
  return result;
}

big_integer big_integer::operator >> (const int &places) const{
  big_integer result;
  result.sign = this->sign;
  result.mag = (this->mag) >> places;

  if((result.mag.words_count() > 0) && (result.mag.get_word(0) == 0))
      result.mag = result.mag.trim_left();

  if(result.mag.size() == 0) result.sign = 0;
  return result;
}

big_integer big_integer::pow(unsigned long long exp) const{
  if(exp == 0) return 1;
  if(exp == 1) return (*this);

  big_integer base(*this);
  big_integer result(1);
  while(exp > 0){
    if (exp & 1)  result = result * base;
    exp = exp >> 1;
    base = base * base;
  }

  if(result.mag.size() == 0) result.sign = 0;
  return result;
}

big_integer big_integer::pow_mod(const big_integer &exp, const big_integer &mod) const{
  if(mod.sign == 0)
    throw std::runtime_error("big_integer::pow_mod: integer division by 0");

  if(exp == 0) return (big_integer(1) % mod);
  if(exp == 1) return ((*this) % mod);
  if(this->sign == 0) return 0;

  big_integer _exp(exp);

  big_integer base(*this);
  big_integer result(1);
  while(_exp > 0){
    if (_exp.mag.get(_exp.mag.size() - 1))  result = (result * base) % mod;
    _exp = _exp >> 1;
    base = (base * base) % mod;
  }

  if(result.mag.size() == 0) result.sign = 0;
  return result;
}

big_integer big_integer::gcd(const big_integer &_big_integer) const{
  if(this->sign == 0)
    return _big_integer;
  if(_big_integer.sign == 0)
    return (*this);

  int mag_comprison = my_bitset::compare(this->mag, _big_integer.mag);
  if(mag_comprison == 0)
    return _big_integer;

  // stein’s GCD algorithm (binary GCD algorithm)
  // let first operand is a, second operand is b
  // get the greatest power of 2 that divides both numbers
  // divide both numbers by that value
  // dividing a by a power of 2 to be odd
  // repeat while b > 0
  //   dividing b by a power of 2 to be odd
  //   swap if a > b
  //   set b = b - a
  // end loop body
  // at the end adjust the sign equals to the sign of the smaller operand

  // using pointers instead of local variables to optimize the swap
  my_bitset *op1_ptr = new my_bitset(this->mag);
  my_bitset *op2_ptr = new my_bitset(_big_integer.mag);

  // the greatest power of 2 that divides both numbers
  int max_pow_2 = 0;
  while((max_pow_2 < op1_ptr->size())
      && (!op1_ptr->get(op1_ptr->size() - max_pow_2 - 1))
      && (max_pow_2 < op2_ptr->size())
      && (!op2_ptr->get(op2_ptr->size() - max_pow_2 - 1))){
    ++max_pow_2;
  }

  (*op1_ptr) = (*op1_ptr) >> max_pow_2;
  (*op2_ptr) = (*op2_ptr) >> max_pow_2;

  // dividing a by a power of 2 to be odd
  int last_set_bit = 0;
  while((last_set_bit < op1_ptr->size())
      && (!op1_ptr->get(op1_ptr->size() - last_set_bit - 1))){
    ++last_set_bit;
  }

  (*op1_ptr) = (*op1_ptr) >> last_set_bit;

  while (op2_ptr->size() > 0) {
    // dividing b by a power of 2 to be odd
    int last_set_bit = 0;
    while((last_set_bit < op2_ptr->size())
        && (!op2_ptr->get(op2_ptr->size() - last_set_bit - 1))){
      ++last_set_bit;
    }

    (*op2_ptr) = (*op2_ptr) >> last_set_bit;

    // swap if a > b
    if(my_bitset::compare((*op1_ptr), (*op2_ptr)) == -1){
      std::swap(op1_ptr, op2_ptr);
    }

    // set b = b - a
    (*op2_ptr) = big_integer::sub((*op2_ptr), (*op1_ptr));
    if((op2_ptr->words_count() > 0) && (op2_ptr->get_word(0) == 0))
      (*op2_ptr) = op2_ptr->trim_left();

  }

  // restore common factors of 2 by left shift
  big_integer result;
  result.sign = ((mag_comprison > 0) ? this->sign : _big_integer.sign);
  result.mag = ((*op1_ptr).pad_left(max_pow_2, 0)) << max_pow_2;
  if((result.mag.words_count() > 0) && (result.mag.get_word(0) == 0))
    result.mag = result.mag.trim_left();
  if(result.mag.size() == 0) result.sign = 0;

  delete op1_ptr;
  delete op2_ptr;
  return result;
}

big_integer big_integer::mod_inverse(const big_integer &_big_integer) const{
  big_integer x, y;
  big_integer gcd = big_integer::gcd_extended((*this), _big_integer, x, y);

  if(gcd != 1) return big_integer(0);
  // _big_integer is added to handle negative x
  return (((x % _big_integer) + _big_integer) % _big_integer);
}

big_integer big_integer::gcd_extended(
    const big_integer &a,
    const big_integer &b,
    big_integer &x,
    big_integer &y) {

  // extended euclidean algorithm
  if(a == 0){
    x = 0, y = 1;
    return b;
  }

  big_integer x1, y1;
  big_integer gcd = big_integer::gcd_extended((b % a), a, x1, y1);
  x = y1 - ((b / a) * x1), y = x1;
  return gcd;
}

bool big_integer::operator == (const big_integer &_big_integer) const{
  return (this->sign == _big_integer.sign) &&
      (my_bitset::compare(this->mag, _big_integer.mag) == 0);
}

bool big_integer::operator != (const big_integer &_big_integer) const{
  return (this->sign != _big_integer.sign) ||
      (my_bitset::compare(this->mag, _big_integer.mag) != 0);
}

bool big_integer::operator > (const big_integer &_big_integer) const{
  if(this->sign > _big_integer.sign) return true;
  if(this->sign < _big_integer.sign) return false;
  if(this->sign == 0) return false;
  int mag_comparison = my_bitset::compare(this->mag, _big_integer.mag);
  return (mag_comparison == (-1 * this->sign));
}

bool big_integer::operator >= (const big_integer &_big_integer) const{
  if(this->sign > _big_integer.sign) return true;
  if(this->sign < _big_integer.sign) return false;
  if(this->sign == 0) return true;
  int mag_comparison = my_bitset::compare(this->mag, _big_integer.mag);
  return ((mag_comparison == 0) || (mag_comparison == (-1 * this->sign)));
}

bool big_integer::operator < (const big_integer &_big_integer) const{
  if(this->sign > _big_integer.sign) return false;
  if(this->sign < _big_integer.sign) return true;
  if(this->sign == 0) return false;
  int mag_comparison = my_bitset::compare(this->mag, _big_integer.mag);
  return (mag_comparison == (1 * this->sign));
}

bool big_integer::operator <= (const big_integer &_big_integer) const{
  if(this->sign > _big_integer.sign) return false;
  if(this->sign < _big_integer.sign) return true;
  if(this->sign == 0) return true;
  int mag_comparison = my_bitset::compare(this->mag, _big_integer.mag);
  return ((mag_comparison == 0) || (mag_comparison == (1 * this->sign)));
}

std::string big_integer::to_string() const{
  if(this->sign == 0) return "0";
  if(this->mag.size() == 0) return "0";

  unsigned char * dec_one = new unsigned char[1];
  dec_one[0] = 1;

  int result_size = 1;
  unsigned char * temp;
  unsigned char * result = new unsigned char[1];
  result[0] = 0;

  // conversion to base 10 using alternate method

  for(int i = 0; i < this->mag.size() - 1; ++i){
    if(this->mag.get(i)){
      temp = result;
      result = big_integer::add_base10(
          result, dec_one,
          result_size, 1, result_size);
      delete[] temp;
    }

    temp = result;
    result = big_integer::add_base10(
        result, result,
        result_size, result_size, result_size);
    delete[] temp;
  }

  if(this->mag.get(this->mag.size() - 1)){
    temp = result;
    result = big_integer::add_base10(
        result, dec_one,
        result_size, 1, result_size);
    delete[] temp;
  }

  for(int i = 0; i < result_size; ++i)
    result[i] += (unsigned char)'0';

  std::string result_str((char*)result, result_size);
  if(this->sign < 0) return "-" + result_str;
  else return result_str;
}

std::string big_integer::to_bin_string() const{
  if(this->sign == 0) return "0";
  if(this->mag.size() == 0) return "0";

  int bits = this->mag.size();
  std::string result_str(bits, '0');
  for(int i = 0; i < bits; ++i)
    result_str[i] = (this->mag.get(i) ? '1' : '0');

  int first_non_zero = 0;
  for(int i = 0; i < (int)result_str.size(); ++i){
    if(result_str[i] != '0') {
      first_non_zero = i;
      break;
    }
  }

  if(this->sign < 0) return "-" + result_str.substr(first_non_zero);
  else return result_str.substr(first_non_zero);
}

std::string big_integer::to_oct_string() const{
  if(this->sign == 0) return "0";
  if(this->mag.size() == 0) return "0";

  char oct_digits_map[8] = {
      '0', '1', '2', '3',
      '4', '5', '6', '7'
  };

  int bits = this->mag.size();
  std::string result_str;
  result_str.reserve((bits / 3) + 1);

  for(int i = bits - 1; i >= 0;){
    int dig = 0;
    for(int exp = 0; (exp < 3) && (i >= 0); --i, ++exp){
      dig = (((int)this->mag.get(i)) << exp) + dig;
    }

    result_str.push_back(oct_digits_map[dig]);
  }

  int result_str_size = result_str.size();
  int half_result_str_size = result_str_size >> 1;
  for(int i=0; i < half_result_str_size; ++i)
    std::swap(result_str[i], result_str[result_str_size - i - 1]);

  int first_non_zero = 0;
  for(int i = 0; i < (int)result_str.size(); ++i){
    if(result_str[i] != '0') {
      first_non_zero = i;
      break;
    }
  }

  if(this->sign < 0) return "-" + result_str.substr(first_non_zero);
  else return result_str.substr(first_non_zero);
}

std::string big_integer::to_dec_string() const{
  return this->to_string();
}

std::string big_integer::to_hex_string() const{
  if(this->sign == 0) return "0";
  if(this->mag.size() == 0) return "0";

  char hex_digits_map[16] = {
      '0', '1', '2', '3',
      '4', '5', '6', '7',
      '8', '9', 'a', 'b',
      'c', 'd', 'e', 'f'
  };

  int words = this->mag.words_count();
  std::string result_str(words << 1, '0');

  for(int i = 0; i < words; ++i) {
    unsigned char hi_word = this->mag.get_word(i) >> 4;
    unsigned char lo_word = this->mag.get_word(i) & 15;
    result_str[i << 1] = hex_digits_map[hi_word];
    result_str[(i << 1) + 1] = hex_digits_map[lo_word];
  }

  int first_non_zero = 0;
  for(int i = 0; i < (int)result_str.size(); ++i){
    if(result_str[i] != '0') {
      first_non_zero = i;
      break;
    }
  }

  if(this->sign < 0) return "-" + result_str.substr(first_non_zero);
  else return result_str.substr(first_non_zero);
}

long long big_integer::to_llong() const{
  unsigned long long ullong_mag = this->mag.to_ullong();
  unsigned long long max_value = 1;
  max_value = (max_value << ((sizeof(unsigned long long) * 8) - 1)) - 1;

  if(ullong_mag > max_value)
    throw std::overflow_error("big_integer::to_llong: overflow_error");

  long long llong_mag = ullong_mag;
  return llong_mag * this->sign;
}

#endif /* BIG_INTEGER_H_ */
