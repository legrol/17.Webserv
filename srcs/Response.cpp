#include "Response.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>

Response::Response(const Request& req, const Config& cfg) : request(req), config(cfg) {
    status_code = 200;
    status_message = "OK";
    if (request.getMethod() == "GET") {
        handleGetRequest();
    } else {
        status_code = 501;
        status_message = "Not Implemented";
        setBody("<h1>501 Not Implemented</h1>");
    }
    std::ostringstream oss;
    oss << body.length();
    setHeader("Content-Length", oss.str());
}

std::string Response::generate() {
    std::ostringstream response;
    response << "HTTP/1.1 " << status_code << " " << status_message << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        response << it->first << ": " << it->second << "\r\n";
    }
    response << "\r\n" << body;
    return response.str();
}

void Response::setStatus(int code, const std::string& message) {
    status_code = code;
    status_message = message;
}

void Response::setHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}

void Response::setBody(const std::string& body_content) {
    body = body_content;
}

std::string Response::readFile(const std::string& path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        return "";
    }
    std::string content;
    char buffer[1024];
    ssize_t bytes;
    while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
        content.append(buffer, bytes);
    }
    close(fd);
    return content;
}

std::string Response::getContentType(const std::string& path) {
    if (path.rfind(".html") != std::string::npos) return "text/html";
    if (path.rfind(".css") != std::string::npos) return "text/css";
    if (path.rfind(".js") != std::string::npos) return "application/javascript";
    if (path.rfind(".jpg") != std::string::npos || path.rfind(".jpeg") != std::string::npos) return "image/jpeg";
    if (path.rfind(".png") != std::string::npos) return "image/png";
    return "text/plain";
}

void Response::handleGetRequest() {
    std::string root = config.get("root");
    std::string index = config.get("index");
    std::string error_page_404 = config.get("error_page_404");
    std::string path = root + request.getUri();

    // Si la URI es "/", usar el archivo índice
    if (request.getUri() == "/") {
        path = root + "/" + index;
    }

    std::string content = readFile(path);
    if (content.empty()) {
        status_code = 404;
        status_message = "Not Found";
        path = root + error_page_404;
        content = readFile(path);
        if (content.empty()) {
            content = "<h1>404 Not Found</h1>";
        }
    }
    setBody(content);
    setHeader("Content-Type", getContentType(path));
}