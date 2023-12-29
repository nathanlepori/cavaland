//
// Created by simrat39 on 12/26/23.
//

#include <thread>
#include <iostream>
#include <sstream>
#include <cstring>
#include "CavaService.hpp"

CavaService::CavaService() {
    dispatcher.connect([this]() {
        process_queue();
    });

    std::thread([&]() {
        std::string command = "cava -p ../cava_config";  // Replace with the command you want to run

        // Open a pipe to read from the command
        FILE *pipe = popen(command.c_str(), "r");
        if (!pipe) {
            std::cerr << "Error opening pipe to command." << std::endl;
            return EXIT_FAILURE;
        }

        // Create a stream to read from the pipe
        char buffer[60];
        double data[30]{0};

        // Infinite loop to continuously read from stdout
        while (true) {
            // Read a line from the pipe
            auto bytes_read = fread(buffer, 1, 60, pipe);
            if (bytes_read < 60) continue;

            // Process the line as needed
            const uint16_t* uint16Buffer = reinterpret_cast<const uint16_t*>(buffer);
            for (int i = 0; i < 30; ++i) {
                // Read uint16_t values directly from the buffer
                data[i] = static_cast<float>(uint16Buffer[i]) / 65535;
            }
            // You can do further processing on the line if necessary
            enqueue(data);
            dispatcher.emit();
        }

        // Close the pipe
        pclose(pipe);
        return EXIT_SUCCESS;
    }).detach();
}

void CavaService::enqueue(double *data) {
    std::lock_guard<std::mutex> m{queue_mutex};
    queue.push(data);
}

void CavaService::process_queue() {
    std::lock_guard<std::mutex> m{queue_mutex};

    while (!queue.empty()) {
        auto top = queue.front();
        queue.pop();
        signal_data.emit(top);
    }
}

CavaService::~CavaService() = default;