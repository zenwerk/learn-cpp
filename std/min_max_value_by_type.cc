#include <iostream>

int main() {
  std::cout << "SCHAR_MIN       =  " << SCHAR_MIN << std::endl; // minimum value for a signed char
  std::cout << "SCHAR_MAX       =  " << SCHAR_MAX << std::endl; // maximum value for a signed char
  std::cout << "UCHAR_MAX       =  " << UCHAR_MAX << std::endl; // maximum value for an unsigned char
  std::cout << "CHAR_MIN        =  " << CHAR_MIN << std::endl; // minimum value for a char
  std::cout << "CHAR_MAX        =  " << CHAR_MAX << std::endl; // maximum value for a char
  std::cout << "SHRT_MIN        =  " << SHRT_MIN << std::endl; // minimum value for a short
  std::cout << "SHRT_MAX        =  " << SHRT_MAX << std::endl; // maximum value for a short
  std::cout << "USHRT_MAX       =  " << USHRT_MAX << std::endl; // maximum value for an unsigned short
  std::cout << "INT_MIN         =  " << INT_MIN << std::endl;  // minimum value for an int
  std::cout << "INT_MAX         =  " << INT_MAX << std::endl;  // maximum value for an int
  std::cout << "UINT_MAX        =  " << UINT_MAX << std::endl; // maximum value for an unsigned int
  std::cout << "LONG_MIN        =  " << LONG_MIN << std::endl; // minimum value for a long
  std::cout << "LONG_MAX        =  " << LONG_MAX << std::endl; // maximum value for a long
  std::cout << "ULONG_MAX       =  " << ULONG_MAX << std::endl; // maximum value for an unsigned long
  std::cout << "LLONG_MIN       =  " << LLONG_MIN << std::endl; // minimum value for a long long
  std::cout << "LLONG_MAX       =  " << LLONG_MAX << std::endl; // maximum value for a long long
  std::cout << "ULLONG_MAX      =  " << ULLONG_MAX << std::endl; // maximum value for an unsigned long long
  std::cout << "PTRDIFF_MIN     =  " << PTRDIFF_MIN << std::endl; // minimum value of ptrdiff_t
  std::cout << "PTRDIFF_MAX     =  " << PTRDIFF_MAX << std::endl; // maximum value of ptrdiff_t
  std::cout << "SIZE_MAX        =  " << SIZE_MAX << std::endl;    // maximum value of size_t
  std::cout << "SIG_ATOMIC_MIN  =  " << SIG_ATOMIC_MIN << std::endl; // minimum value of sig_atomic_t
  std::cout << "SIG_ATOMIC_MAX  =  " << SIG_ATOMIC_MAX << std::endl; // maximum value of sig_atomic_t
  std::cout << "WINT_MIN        =  " << WINT_MIN << std::endl;  // minimum value of wint_t
  std::cout << "WINT_MAX        =  " << WINT_MAX << std::endl;  // maximum value of wint_t
  std::cout << "WCHAR_MIN       =  " << WCHAR_MIN << std::endl; // minimum value of wchar_t
  std::cout << "WCHAR_MAX       =  " << WCHAR_MAX << std::endl; // maximum value of wchar_t
  std::cout << "CHAR_BIT        =  " << CHAR_BIT << std::endl;  // number of bits in a char
  std::cout << "MB_LEN_MAX      =  " << MB_LEN_MAX << std::endl; // maximum length of a multibyte character in bytes

  return 0;
}