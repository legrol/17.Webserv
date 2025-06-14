#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>		// For std::string
#include <vector>		// For std::vector
#include <map>			// For std::map
#include <fstream>		// For std::ifstream
#include <stdexcept>	// For std::runtime_error
#include <algorithm>	// For std::find_if



class Config {
	// This class is responsible for reading configuration settings from a file.
	// It stores the settings in a map for easy access by key.
public:
	Config(const std::string& filename);
	// Constructor that takes a filename as an argument.
	// It reads the configuration file and populates the settings map.
	// If the file cannot be opened, it throws a std::runtime_error. 
    std::string get(const std::string& key) const;
	// std::string get(const std::string& key) const;
	// Retrieves the value associated with the given key.
	// If the key does not exist, it throws a std::runtime_error.
	// If the key exists, it returns the value as a std::string.

private:
	std::map<std::string, std::string> settings;
	// A map to store the configuration settings, where the key is a string
	// and the value is also a string.
	// The map allows for efficient retrieval of settings by key.
	// The map is private to ensure that it can only be accessed through the public methods.
    void parse(const std::string& filename);
	// Parses the configuration file and populates the settings map.
	// It reads each line of the file, splits it into key and value,
	// and stores them in the settings map.
};

#endif