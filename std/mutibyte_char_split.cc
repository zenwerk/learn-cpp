#include <iostream>
#include <string>

int main() {
  std::string str = "こんͰにち\u0604は、§世界! abcdw魑!魅😀😀魍Z魎";

  std::string s1 = "";
  std::string s2 = "";
  std::string s3 = "";
  std::string s4 = "";

  for (size_t i = 0; i < str.size();) {
    unsigned char c = str[i];
    size_t char_size;

    if ((c & 0x80) == 0)       char_size = 1;  // 1バイト文字
    else if ((c & 0xE0) == 0xC0) char_size = 2;  // 2バイト文字
    else if ((c & 0xF0) == 0xE0) char_size = 3;  // 3バイト文字
    else if ((c & 0xF8) == 0xF0) char_size = 4;  // 4バイト文字
    else {
      // 不正なUTF-8文字
      std::cerr << "Invalid UTF-8 sequence." << std::endl;
      return 1;
    }

    std::string character = str.substr(i, char_size);

    switch (char_size) {
      case 1:
        s1 += character;
        break;
      case 2:
        s2 += character;
        break;
      case 3:
        s3 += character;
        break;
      case 4:
        s4 += character;
        break;
    }

    i += char_size;
  }

  std::cout << "input str = " << str << std::endl;
  std::cout << "------------" << std::endl;
  std::cout << "1bit chars: " << s1 << std::endl;
  std::cout << "2bit chars: " << s2 << std::endl;
  std::cout << "3bit chars: " << s3 << std::endl;
  std::cout << "4bit chars: " << s4 << std::endl;

  return 0;
}