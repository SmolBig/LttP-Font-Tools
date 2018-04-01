#include "inject.h"
#include <iostream>

int main(int argc, char** argv) {
  if(argc != 4) {
    std::cout << "Usage:\n  inject <base rom filename> <edited texture filename> <new rom filename>\n\n";
    return 0;
  }
  
  try {
    inject(argv[1], argv[2], argv[3]);
  }
  catch(std::exception& e) {
    std::cout << e.what() << "\n\n";
  }
}
