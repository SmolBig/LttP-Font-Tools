#include "extract.h"
#include <iostream>

int main(int argc, char** argv) {
  if(argc != 3) {
    std::cout << "Usage:\n  extract <base rom filename> <output png filename>\n\n";
    return 0;
  }

  try {
    extract(argv[1], argv[2]);
  }
  catch(std::exception& e) {
    std::cout << e.what() << "\n\n";
  }
}
