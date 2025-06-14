#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>		// For std::string
#include <map>			// For std::map

class Request {
	// The Request class represents an HTTP request.
	// It is designed to parse and store the components of an HTTP request.
	// It contains methods to parse the request line, headers, and body.
	// It provides accessors for the method, URI, version, headers, and body.
public:
    Request();			// Default constructor
    bool parse(const std::string& raw_request);
	// Parses the raw HTTP request string. It is boolean to indicate success or failure.
	// Returns true if parsing was successful, false otherwise.
    std::string getMethod() const;
	// Returns the HTTP method (e.g., GET, POST).
    std::string getUri() const;
	// Returns the request URI. URI is the Uniform Resource Identifier.
	// It is a string that identifies the resource being requested.
    std::string getVersion() const;
	// Returns the HTTP version (e.g., HTTP/1.1).
    std::string getHeader(const std::string& key) const;
	// Returns the value of a specific header by key (key is the name of the header).
	// Headers is a map of header names to values (e.g., Host: localhost).
    std::string getBody() const;
	// Returns the body of the request, which is the content sent with the request.

private:
    std::string method;
	// The HTTP method (e.g., GET, POST).
    std::string uri;
	// The request URI, which identifies the resource being requested (e.g., /index.html).
    std::string version;
	// The HTTP version (e.g., HTTP/1.1).
    std::map<std::string, std::string> headers;
	// The map of headers, where the key is the header name and the value is the header value.
	// Headers are case-insensitive, so they should be stored in a case-insensitive manner.
    std::string body;
	// The body of the request, which contains the content sent with the request.
	// The body is typically used in POST requests to send data to the server.
    bool parseRequestLine(const std::string& line);
	// Parses the request line (e.g., "GET /index.html HTTP/1.1").
	// Returns true if parsing was successful, false otherwise.
    bool parseHeader(const std::string& line);
	// Parses a single header line (e.g., "Host: example.com").
	// Returns true if parsing was successful, false otherwise.
};

#endif