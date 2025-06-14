#include "Server.hpp"	// Include the Server class header file to define the server functionality.
#include <iostream>		// For std::cerr and std::cout to print error messages and debug information.

int main(int argc, char* argv[]) {
    std::string config_file = (argc > 1) ? argv[1] : "config/default.conf";
	// Default configuration file is "config/default.conf" if no argument is provided.
    try {
        Config config(config_file);
		// Try to create a Config object with the provided configuration file.
        Server server(config);
		// Create a Server object with the configuration settings.
        server.start();
		// Start the server to listen for incoming connections.
    } catch (const std::exception& e) {
		// Catch any exceptions thrown during the setup or execution of the server.
        std::cerr << "Error: " << e.what() << std::endl;
		// Print the error message to standard error output if an exception occurs.
        return 1;
		// Return a non-zero exit code to indicate an error.
    }
    return 0;
	// Return 0 to indicate successful execution of the program.
}