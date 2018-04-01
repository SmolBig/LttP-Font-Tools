#include "make_ips.h"
#include <iostream>

int main(int argc, char** argv) {
  if(argc != 3) {
    std::cout << "Usage:\n  make_ips <edited texture filename> <new ips filename>\n\n";
    return 0;
  }

  std::string srcPath, outPath;

  try {
    make_ips(argv[1], argv[2]);
  }
  catch(std::exception& e) {
    std::cout << e.what() << "\n\n";
  }
}
