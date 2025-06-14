#include "Config.hpp"

Config::Config(const std::string& filename) {
    parse(filename);
	// Constructor call to parse with the name of the faile.
}

void Config::parse(const std::string& filename) {
    std::ifstream file(filename.c_str());
	// Open the configuration file for reading
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + filename);
		// If the file cannot be opened, throw an error.
    }
    std::string line;
    while (std::getline(file, line)) {
        // Remove whitespace and skip empty lines or comments
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line.empty() || line[0] == '#') {
            continue;
			// if the line is empty or starts with a comment character, skip it.
        }
        // Split line into key and value
        size_t delimiterPos = line.find("=");
		// Find the position of the delimiter '='
        if (delimiterPos != std::string::npos) {
			// If the delimiter is found, split the line into key and value
			// Extract key and value to store in the map
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            settings[key] = value;
        }
    }
    file.close();
	// Close the file after reading all lines
}

std::string Config::get(const std::string& key) const {
	// Recover the value associated with the given key
    std::map<std::string, std::string>::const_iterator it = settings.find(key);
	// Find the key in the settings map
    if (it != settings.end()) {
        return it->second;
		// If the key is found, return the associated value
    }
    return "";
	// If the key is not found, return an empty string
}