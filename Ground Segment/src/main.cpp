#include "gateway.hpp"
#include <iostream>
#include <csignal>
#include <thread>

static std::atomic<bool> running{true};

void signalHandler(int) {
    running = false;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <tcp_port> <uart_device>\n";
        std::cerr << "Example: " << argv[0] << " 8080 /dev/ttyUSB0\n";
        return 1;
    }

    try {
        // Set up signal handling for clean shutdown
        // signal(SIGINT, signalHandler);
        // signal(SIGTERM, signalHandler);

        // Parse command line args
        uint16_t port = static_cast<uint16_t>(std::stoi(argv[1]));
        std::string uart_device = argv[2];

        std::cout << "Starting Gateway on port " << port 
                  << " with UART device " << uart_device << "\n";

        // Create and start the gateway
        altair::Gateway gateway(port, uart_device);
        gateway.start();

        std::cout << "Gateway running. Press Ctrl+C to exit.\n";

        // Main loop
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout << "\nShutting down...\n";

        gateway.stop();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}