#include "logclient.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
        std::cerr << "Example: " << argv[0] << " localhost 8064\n";
        return 1;
    }

    try {
        altair::LogClient client(argv[1], std::stoi(argv[2]));
        client.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}