#include "llama.h"
#include <iostream>

int main() {
    std::cout << "Program started\n";

    llama_backend_init();
    std::cout << "llama backend initialized\n";

    llama_backend_free();
    std::cout << "Program exiting\n";


    return 0;
}
