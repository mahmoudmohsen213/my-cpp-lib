#ifndef my_bitset_H_
#define my_bitset_H_

#include <cstdlib>
#include <string>
#include <memory.h>
#include <stdexcept>

class my_bitset {
private:
  const static int word_size = (sizeof(unsigned char) * 8);
  unsigned char *arr;
  int words;
public:
  my_bitset();
  // basic constructor
  my_bitset(const int &size, const bool &value);

  // copy constructor
  my_bitset(const my_bitset &_my_bitset);
  my_bitset(const unsigned long long &val);
  my_bitset(const unsigned char* buffer, const int &buffer_size);

  // text constructor
  my_bitset(const char* buffer, const int &buffer_size);
  my_bitset(const std::string &str);

  // destructor
  ~my_bitset();

  // assignment operator
  my_bitset& operator = (const my_bitset &_my_bitset);

  // access
  int static get_word_size();
  int words_count() const;
  int size() const;
  bool get(const int &index) const;
  unsigned char get_word(const int &index) const;
  void set(const int &index, const bool &value);
  void set_word(const int &index, const unsigned char &value);

  // all operators should return a new value without changing either
  // the calling object (the this object) or the passed object
  my_bitset operator & (const my_bitset &_my_bitset) const;
  my_bitset operator | (const my_bitset &_my_bitset) const;
  my_bitset operator ^ (const my_bitset &_my_bitset) const;
  my_bitset operator ~ () const;
  my_bitset operator << (const int &places) const;
  my_bitset operator >> (const int &places) const;
  my_bitset rotate_left  (const int &places) const;
  my_bitset rotate_right (const int &places) const;
  my_bitset pad_left (const int &places, const bool &value) const;
  my_bitset pad_right (const int &places, const bool &value) const;
  my_bitset trim_left () const;
  my_bitset trim_left (const int &words_count) const;
  my_bitset trim_right () const;
  my_bitset trim_right (const int &words_count) const;

  // compares the arithmetic representation of the two arguments
  int static compare(const my_bitset &_my_bitset1, const my_bitset &_my_bitset2);

  // compares the big-endian arithmetic representation
  // of the calling object and the passed argument
  bool operator == (const my_bitset &_my_bitset) const;
  bool operator != (const my_bitset &_my_bitset) const;
  bool operator > (const my_bitset &_my_bitset) const;
  bool operator >= (const my_bitset &_my_bitset) const;
  bool operator < (const my_bitset &_my_bitset) const;
  bool operator <= (const my_bitset &_my_bitset) const;

  bool* dump(int &size) const;
  char* to_c_str() const;
  char* to_c_str(int &size) const;
  std::string to_string() const;
  unsigned long long to_ullong() const;
};

///////////////////////////////////////

my_bitset::my_bitset(){
  this->words = 0;
  this->arr = NULL;
}

my_bitset::my_bitset(const int &size, const bool &value){
  if(size < 0)
    throw std::runtime_error("my_bitset::my_bitset: invalid_size");

  if(size == 0) {
    this->words = 0;
    this->arr = NULL;
    return;
  }

  this->words = ((size - 1) / my_bitset::word_size) + 1;
  this->arr = new unsigned char[this->words];
  if(!value) memset(this->arr, 0, this->words);
  else memset(this->arr, 0xFF, this->words);
}

my_bitset::my_bitset(const my_bitset &_my_bitset){
  this->arr = new unsigned char[_my_bitset.words];
  this->words = _my_bitset.words;
  memcpy(this->arr, _my_bitset.arr, _my_bitset.words);
}

my_bitset::my_bitset(const unsigned long long &value){
  unsigned long long _value = value;
  unsigned long long base = 1 << my_bitset::word_size;
  this->words = sizeof(_value) / sizeof(unsigned char);
  this->arr = new unsigned char[this->words];
  memset(this->arr, 0, this->words);

  int index = this->words;
  while(index--)
    this->arr[index] = _value % base, _value /= base;
}

my_bitset::my_bitset(const unsigned char* buffer, const int &buffer_size){
  this->arr = new unsigned char[buffer_size];
  this->words = buffer_size;
  memcpy(this->arr, buffer, buffer_size);
}

// text constructor
my_bitset::my_bitset(const char* buffer, const int &buffer_size){
  this->arr = new unsigned char[buffer_size];
  this->words = buffer_size;
  memcpy(this->arr, buffer, buffer_size);
}

// text constructor
my_bitset::my_bitset(const std::string &str){
  this->arr = new unsigned char[str.size()];
  this->words = str.size();
  memcpy(this->arr, str.c_str(), str.size());
}

my_bitset::~my_bitset(){
  delete[] this->arr;
}

my_bitset& my_bitset::operator = (const my_bitset &_my_bitset){
  delete[] this->arr;
  this->arr = new unsigned char[_my_bitset.words];
  this->words = _my_bitset.words;
  memcpy(this->arr, _my_bitset.arr, _my_bitset.words);
  return (*this);
}

int my_bitset::get_word_size(){
  return my_bitset::word_size;
}

int my_bitset::words_count() const{
  return this->words;
}

int my_bitset::size() const{
  return (this->words * my_bitset::word_size);
}

bool my_bitset::get(const int &index) const{
  int size = this->words * my_bitset::word_size;
  if(index >= size)
    throw std::out_of_range("my_bitset::get: index_out_of_bound");
  if(index < 0)
    throw std::out_of_range("my_bitset::get: index_out_of_bound");

  int word_index = index / my_bitset::word_size;
  int bit_index = index % my_bitset::word_size;

  return (bool)(((this->arr[word_index]) >> (my_bitset::word_size - bit_index - 1)) & 1);
}

unsigned char my_bitset::get_word(const int &index) const{
  if(index >= this->words)
    throw std::out_of_range("my_bitset::get_word: index_out_of_bound");
  if(index < 0)
    throw std::out_of_range("my_bitset::get_word: index_out_of_bound");
  return this->arr[index];
}

void my_bitset::set(const int &index, const bool &value){
  int size = this->words * my_bitset::word_size;
  if(index >= size)
    throw std::out_of_range("my_bitset::set: index_out_of_bound");
  if(index < 0)
    throw std::out_of_range("my_bitset::set: index_out_of_bound");

  int word_index = index / my_bitset::word_size;
  int bit_index = index % my_bitset::word_size;

  unsigned char mask = (unsigned char)(1 << (my_bitset::word_size - bit_index - 1));
  if(value) this->arr[word_index] |= mask;
  else this->arr[word_index] &= (~mask);
}

void my_bitset::set_word(const int &index, const unsigned char &value){
  if(index >= this->words)
    throw std::out_of_range("my_bitset::set_word: index_out_of_bound");
  if(index < 0)
    throw std::out_of_range("my_bitset::set_word: index_out_of_bound");
  this->arr[index] = value;
}

my_bitset my_bitset::operator & (const my_bitset &_my_bitset) const{
  my_bitset result(this->words * my_bitset::word_size, 0);
  for(int i = 0;i < this->words;++i)
    result.arr[i] = this->arr[i] & _my_bitset.arr[i];
  return result;
}

my_bitset my_bitset::operator | (const my_bitset &_my_bitset) const{
  my_bitset result(this->words * my_bitset::word_size, 0);
  for(int i = 0;i < this->words;++i)
    result.arr[i] = this->arr[i] | _my_bitset.arr[i];
  return result;
}

my_bitset my_bitset::operator ^ (const my_bitset &_my_bitset) const{
  my_bitset result(this->words * my_bitset::word_size, 0);
  for(int i = 0;i < this->words;++i)
    result.arr[i] = this->arr[i] ^ _my_bitset.arr[i];
  return result;
}

my_bitset my_bitset::operator ~ () const{
  my_bitset result(this->words * my_bitset::word_size, 0);
  for(int i=0;i<this->words;++i)
    result.arr[i] = ~(this->arr[i]);
  return result;
}

my_bitset my_bitset::operator << (const int &places) const{
  if(places < 0)
    return (*this)>>(places * -1);

  int size = this->words * my_bitset::word_size;
  my_bitset result(size, 0);
  for(int i = 0; i < (size - places); ++i)
    result.set(i, this->get(i + places));
  return result;
}

my_bitset my_bitset::operator >> (const int &places) const{
  if(places < 0)
    return (*this)<<(places * -1);

  int size = this->words * my_bitset::word_size;
  my_bitset result(size, 0);
  for(int i = size - 1; i >= places; --i)
    result.set(i, this->get(i - places));
  return result;
}

my_bitset my_bitset::rotate_left  (const int &places) const{
  if(places < 0)
    return this->rotate_right(places * -1);

  int size = this->words * my_bitset::word_size;
  int _places = places % size;

  unsigned char *temp_buffer = new unsigned char[this->words * 2];
  memcpy(temp_buffer, this->arr, this->words);
  memcpy(temp_buffer + this->words, this->arr, this->words);
  my_bitset temp_bitset(temp_buffer, this->words * 2);
  my_bitset result(size, 0);

  for(int i = 0; i < size; ++i)
    result.set(i, temp_bitset.get(i + _places));
  return result;
}

my_bitset my_bitset::rotate_right (const int &places) const{
  if(places < 0)
    return this->rotate_left(places * -1);

  int size = this->words * my_bitset::word_size;
  int _places = places % size;

  unsigned char *temp_buffer = new unsigned char[this->words * 2];
  memcpy(temp_buffer, this->arr, this->words);
  memcpy(temp_buffer + this->words, this->arr, this->words);
  my_bitset temp_bitset(temp_buffer, this->words * 2);
  my_bitset result(size, 0);

  for(int i = size - 1; i >= 0; --i)
    result.set(i, temp_bitset.get(size + i - _places));
  return result;
}

my_bitset my_bitset::pad_left (const int &places, const bool &value) const{
  my_bitset result((this->words * my_bitset::word_size) + places, 0);
  memcpy(result.arr + (result.words - this->words), this->arr, this->words);
  if(value) memset(result.arr, 0xFF, (result.words - this->words));
  return result;
}

my_bitset my_bitset::pad_right (const int &places, const bool &value) const{
  my_bitset result((this->words * my_bitset::word_size) + places, 0);
  memcpy(result.arr, this->arr, this->words);
  if(value) memset(result.arr + this->words, 0xFF, (result.words - this->words));
  return result;
}

my_bitset my_bitset::trim_left () const{
  int index = 0;
  while((index < this->words) && (this->arr[index] == 0)) ++index;
  my_bitset result((this->words - index) * my_bitset::word_size, 0);
  memcpy(result.arr, this->arr + index, result.words);
  return result;
}

my_bitset my_bitset::trim_left (const int &words_count) const{
  if(words_count > this->words)
    throw std::out_of_range("my_bitset::trim_left: words_count_out_of_range");
  if(words_count < 0)
    throw std::out_of_range("my_bitset::trim_left: words_count_out_of_range");

  int index = words_count;
  my_bitset result((this->words - index) * my_bitset::word_size, 0);
  memcpy(result.arr, this->arr + index, result.words);
  return result;
}

my_bitset my_bitset::trim_right () const{
  int index = this->words - 1;
  while((index >= 0) && (this->arr[index] == 0)) --index;
  my_bitset result((index + 1) * my_bitset::word_size, 0);
  memcpy(result.arr, this->arr, index + 1);
  return result;
}

my_bitset my_bitset::trim_right (const int &words_count) const{
  if(words_count > this->words)
    throw std::out_of_range("my_bitset::trim_right: words_count_out_of_range");
  if(words_count < 0)
    throw std::out_of_range("my_bitset::trim_right: words_count_out_of_range");

  int index = this->words - words_count - 1;
  my_bitset result((index + 1) * my_bitset::word_size, 0);
  memcpy(result.arr, this->arr, index + 1);
  return result;
}

// compare the arithmetic representation of the two arguments
// return -1 if _my_bitset1 > _my_bitset2
// return  1 if _my_bitset1 < _my_bitset2
// return 0 if _my_bitset1 == _my_bitset2
int my_bitset::compare(const my_bitset &_my_bitset1, const my_bitset &_my_bitset2){
  int max_size = ((_my_bitset1.size() >= _my_bitset2.size()) ? _my_bitset1.size() : _my_bitset2.size());
  my_bitset op1 = _my_bitset1.pad_left(max_size - _my_bitset1.size(), 0);
  my_bitset op2 = _my_bitset2.pad_left(max_size - _my_bitset2.size(), 0);

  int flag = 0;
  for(int i = 0; (i < op1.words) && (flag == 0); ++i) {
    if(op1.arr[i] > op2.arr[i]) flag = -1;
    else if(op1.arr[i] < op2.arr[i]) flag = 1;
  }

  return flag;
}

bool my_bitset::operator == (const my_bitset &_my_bitset) const{
  int flag = my_bitset::compare((*this), _my_bitset);
  return (flag == 0);
}

bool my_bitset::operator != (const my_bitset &_my_bitset) const{
  int flag = my_bitset::compare((*this), _my_bitset);
  return (flag != 0);
}

bool my_bitset::operator > (const my_bitset &_my_bitset) const{
  int flag = my_bitset::compare((*this), _my_bitset);
  return (flag == -1);
}

bool my_bitset::operator >= (const my_bitset &_my_bitset) const{
  int flag = my_bitset::compare((*this), _my_bitset);
  return (flag != 1);
}

bool my_bitset::operator < (const my_bitset &_my_bitset) const{
  int flag = my_bitset::compare((*this), _my_bitset);
  return (flag == 1);
}

bool my_bitset::operator <= (const my_bitset &_my_bitset) const{
  int flag = my_bitset::compare((*this), _my_bitset);
  return (flag != -1);
}

bool* my_bitset::dump(int &size) const{
  size = this->words * my_bitset::word_size;
  bool *dump = new bool[size];
  for(int i=0; i<size; ++i)
    dump[i] = this->get(i);
  return dump;
}

char* my_bitset::to_c_str() const{
  char *buffer = new char[this->words + 1];
  memcpy(buffer, this->arr, this->words);
  buffer[this->words] = 0;
  return buffer;
}

char* my_bitset::to_c_str(int &size) const{
  size = this->words;
  char *buffer = new char[this->words + 1];
  memcpy(buffer, this->arr, this->words);
  buffer[this->words] = 0;
  return buffer;
}

std::string my_bitset::to_string() const{
  if(this->words == 0) return "";
  return std::string((char*)(this->arr), this->words);
}

unsigned long long my_bitset::to_ullong() const{
  if((this->words * sizeof(unsigned char)) > sizeof(unsigned long long))
    throw std::overflow_error("my_bitset::to_ullong: overflow_error");

  unsigned long long val = 0;
  for(int i=0; i<this->words; ++i)
    val <<= my_bitset::word_size, val |= this->arr[i];

  return val;
}

#endif /* my_bitset_H_ */
