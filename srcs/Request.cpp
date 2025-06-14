#include "Request.hpp"      // Include the header file for the Request class
#include <sstream>          // For std::istringstream

Request::Request() : method(""), uri(""), version("") {}
// Constructor initializes the method, uri, and version to empty strings.

bool Request::parse(const std::string& raw_request) {
    std::istringstream stream(raw_request);
    // std::istringstream is used to read the raw request string line by line.
    // The first line will be the request line, and subsequent lines will be headers,
    // until an empty line is encountered, which indicates the end of headers.
    // The body, if present, will be read after the headers.
    std::string line;
    // std::string is used to store each line of the request.
    
    // Parsear la primera línea
    if (!std::getline(stream, line) || line.empty()) {
        return false;
        // If the first line is empty or cannot be read, return false.
        // This indicates that the request is malformed or incomplete.
    }
    if (!parseRequestLine(line)) {
        return false;
        // If parsing the request line fails, return false.
        // This indicates that the request line is not in the expected format.
    }

    // Parsear cabeceras
    while (std::getline(stream, line) && line != "\r" && line != "") {
        // Read headers until an empty line or a line with just a carriage return is encountered.
        // An empty line indicates the end of headers.
        // A line with just a carriage return is also treated as an empty line.
        // This is common in HTTP requests to separate headers from the body.
        if (!parseHeader(line)) {
            return false;
            // If parsing a header fails, return false.
            // This indicates that the header line is not in the expected format.
        }
    }

    // Leer el cuerpo (si existe)
    std::string body_content;
    // std::string is used to store the body content.
    // The body content is read from the stream until the end of the stream.
    // The body is typically present in POST requests or other methods that send data.
    // If the request does not have a body, this will remain empty.
    while (std::getline(stream, line)) {
    // Read the body line by line until the end of the stream.
        body_content += line + "\n";
        // Append each line to the body content, adding a newline character.
    }
    body = body_content;
    // Set the body member variable to the accumulated body content.
    // This will contain the complete body of the request, including all lines read.
    // If the body is empty, it means there was no content sent with the request.
    return true;
    // Return true if the request was successfully parsed, including the request line, headers, and body.
}

bool Request::parseRequestLine(const std::string& line) {
    // Parse the request line, which is expected to be in the format:
    // "METHOD URI HTTP/VERSION", e.g., "GET /index.html HTTP/1.1".
    // The method is the HTTP method (e.g., GET, POST), the URI is the resource being requested,
    // and the version is the HTTP version (e.g., HTTP/1.1).
    std::istringstream line_stream(line);
    // std::istringstream is used to read the request line.
    // It allows us to extract the method, URI, and version from the line.
    // std::istringstream is a stream class to operate on strings.
    // It provides the same interface as std::istream, allowing us to use extraction operators.
    // std::istringstream is used to read the request line, which is expected to be in the format:
    // "METHOD URI HTTP/VERSION", e.g., "GET /index.html HTTP/1.1".
    // std::string is used to store the method, URI, and version.
    std::string temp_method, temp_uri, temp_version;
    // std::string is used to store the method, URI, and version temporarily.
    // std::string is a standard string class in C++ that represents a sequence of characters.
    // It is used to store the method (e.g., GET, POST), URI (e.g., /index.html),
    // and version (e.g., HTTP/1.1) extracted from the request line.
    if (!(line_stream >> temp_method >> temp_uri >> temp_version)) {
        return false;
        // If the extraction fails, return false.
        // This indicates that the request line is not in the expected format.
        // The expected format is "METHOD URI HTTP/VERSION", e.g., "GET /index.html HTTP/1.1".
    }
    if (temp_version != "HTTP/1.1" && temp_version != "HTTP/1.0") {
        return false;
        // If the version is not HTTP/1.1 or HTTP/1.0, return false.
        // This indicates that the request is using an unsupported HTTP version.
    }
    method = temp_method;       // Set the method member variable to the extracted method.
    uri = temp_uri;             // Set the uri member variable to the extracted URI. 
    version = temp_version;     // Set the version member variable to the extracted version.
    return true;
    // Return true if the request line was successfully parsed.
}

bool Request::parseHeader(const std::string& line) {
    // Parse a single header line, which is expected to be in the format:
    // "Header-Name: Header-Value", e.g., "Host: localhost".
    // parseHeader slip the header line into a key-value pair, using the colon (:) as the delimiter.
    size_t colon_pos = line.find(":");
    if (colon_pos == std::string::npos) {
        return false;
        // If there is no colon in the line, return false.
        // This indicates that the header line is not in the expected format.
        // The colon is used to separate the header name from the header value.
    }
    std::string key = line.substr(0, colon_pos);
    // Extract the key (header name) from the line, which is the part before the colon.
    // std::string::substr is used to extract a substring from the line.
    // The key is the header name (e.g., "Host").
    std::string value = line.substr(colon_pos + 1);
    // Extract the value (header value) from the line, which is the part after the colon.
    // std::string::substr is used to extract a substring from the line.
    // The value is the header value (e.g., "localhost").
    
    // Limpiar espacios iniciales y finales en el valor
    size_t start = value.find_first_not_of(" \t");
    // Find the first non-whitespace character in the value.
    size_t end = value.find_last_not_of(" \t\r");
    // Find the last non-whitespace character in the value.
    if (start != std::string::npos && end != std::string::npos) {
    // If both start and end are valid positions, extract the substring.
        value = value.substr(start, end - start + 1);
    // std::string::substr is used to extract the substring from start to end.
    } else {
        value = "";
        // If either start or end is not found, set the value to an empty string.
    }
    headers[key] = value;
    // Store the key-value pair in the headers map.
    return true;
    // Return true if the header line was successfully parsed.
    // The headers map is used to store the header names as keys and their corresponding values.
}

std::string Request::getMethod() const { return method; }
// Returns the HTTP method (e.g., GET, POST) of the request.
std::string Request::getUri() const { return uri; }
// Returns the request URI (e.g., /index.html) of the request.
std::string Request::getVersion() const { return version; }
// Returns the HTTP version (e.g., HTTP/1.1) of the request.
std::string Request::getHeader(const std::string& key) const {
    // Returns the value of a specific header by key.
    // The key is the name of the header (e.g., "Host").
    std::map<std::string, std::string>::const_iterator it = headers.find(key);
    // std::map::find is used to search for the key in the headers map.
    if (it != headers.end()) {
        // If the key is found in the headers map, return the corresponding value.
        // std::map::end returns an iterator to the end of the map, which is one past the last element.
        // If the key is not found, it returns an iterator to the end of the map.
        // The value is the header value (e.g., "localhost").
        return it->second;
    // it->second is used to access the value associated with the key in the map.
    }
    return "";  // If the key is not found, return an empty string.
}
std::string Request::getBody() const { return body; }
// Returns the body of the request, which contains the content sent with the request.
// The body is typically used in POST requests to send data to the server.
// If the request does not have a body, this will return an empty string.
// The body is the content sent with the request, which can be used to send data to the server.
// The body is typically present in POST requests or other methods that send data.