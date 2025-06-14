#ifndef SERVER_HPP
#define SERVER_HPP

#include "Config.hpp"	 	// Include the Config class for configuration handling
#include "Request.hpp"		// Include the Request class for handling HTTP requests
#include "Response.hpp"		// Include the Response class for generating HTTP responses
#include <sys/socket.h>	 	// For socket programming
#include <netinet/in.h>	 	// For sockaddr_in structure to define internet addresses
#include <poll.h>	 		// For poll functionality to handle multiple file descriptors
#include <vector>			// For using std::vector to manage multiple file descriptors
#include <string>			// For using std::string to handle configuration keys and values



class Server {
	// This class is responsible for setting up and managing a server.
	// It uses the Config class to read configuration settings from a file.
	// The server listens for incoming connections and handles them using poll.
	// It is designed to be efficient and scalable, allowing multiple connections to be handled simultaneously.
public:
    Server(const Config& config);
	// Constructor that takes a Config object to initialize the server settings.
    ~Server();
	// Destructor to clean up resources when the server is no longer needed.
    void start();
	// Starts the server, setting up the socket and listening for incoming connections.

private:
    int server_fd;
	// File descriptor for the server socket.
    struct sockaddr_in address;
	// Structure to hold the server's address information.
    std::vector<struct pollfd> fds;
	// Vector to hold file descriptors for polling.
    Config config;
	// Instance of Config to access configuration settings.

    void setupSocket();
	// Sets up the server socket based on configuration settings.
    void handleConnections();
	// Handles incoming connections and manages them using poll.
};

#endif