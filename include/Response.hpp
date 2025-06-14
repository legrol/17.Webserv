#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Config.hpp"	// Include the Config class for configuration settings
#include "Request.hpp"	// Include the Request class for handling HTTP requests
#include <string>		// Include the string class for handling strings
#include <map>			// Include the map class for storing key-value pairs	

class Response {
	// The Response class represents an HTTP response.
	// It is designed to generate a complete HTTP response based on the request and configuration.
	// It contains methods to set the status, headers, and body of the response.
	// It provides a method to generate the complete response string.
public:
    Response(const Request& request, const Config& config);
	// Constructor that takes a Request object and a Config object.
    std::string generate();
	// Generates the complete HTTP response string.
    void setStatus(int code, const std::string& message);
	// Sets the HTTP status code and message for the response.
	// e.g., setStatus(200, "OK") sets the status code to 200 and the message to "OK".
    void setHeader(const std::string& key, const std::string& value);
	// Sets a specific header in the response.
	// e.g., setHeader("Content-Type", "text/html") sets the Content-Type header to text/html.
    void setBody(const std::string& body_content);
	// Sets the body content of the response.

private:
    int status_code;
	// The HTTP status code (e.g., 200, 404) of the response.
    std::string status_message;
	// The HTTP status message (e.g., "OK", "Not Found") of the response.
    std::map<std::string, std::string> headers;
	// A map to store the headers of the response, where the key is the header name and the value is the header value.
    std::string body;
	// The body content of the response, which contains the actual data being sent back to the client.
    const Request& request;
	// Reference to the Request object that contains the details of the HTTP request.
    const Config& config;

    std::string readFile(const std::string& path);
	// Reads the content of a file and returns it as a string.
    std::string getContentType(const std::string& path);
	// Returns the content type based on the file extension.
    void handleGetRequest();
	// Handles GET requests by reading the requested file and setting the appropriate headers.
};

#endif