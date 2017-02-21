#pragma once

#include <string>
#include <cstdint>

#include <boost/property_tree/ptree.hpp>

class ConfigurationReader {

public:
	ConfigurationReader(std::string path);

	bool GetBooleanValue(std::string path, bool defaultValue) const;
	bool GetBooleanValue(std::string path) const;

	uint8_t GetByteValue(std::string path, uint8_t defaultValue) const;
	uint8_t GetByteValue(std::string path) const;

	uint16_t GetUShortValue(std::string path, uint16_t defaultValue) const;
	uint16_t GetUShortValue(std::string path) const;

	int GetIntValue(std::string path, int defaultValue) const;
	int GetIntValue(std::string path) const;

	double GetDoubleValue(std::string path, double defaultValue) const;
	double GetDoubleValue(std::string path) const;

	std::string GetStringValue(std::string path, std::string defaultValue) const;
	std::string GetStringValue(std::string path) const;

private:
	bool GetBooleanValue(const boost::property_tree::ptree& root, std::string path, bool defaultValue) const;
	bool GetBooleanValue(const boost::property_tree::ptree& root, std::string path) const;

	uint8_t GetByteValue(const boost::property_tree::ptree& root, std::string path, uint8_t defaultValue) const;
	uint8_t GetByteValue(const boost::property_tree::ptree& root, std::string path) const;

	uint16_t GetUShortValue(const boost::property_tree::ptree& root, std::string path, uint16_t defaultValue) const;
	uint16_t GetUShortValue(const boost::property_tree::ptree& root, std::string path) const;

	int GetIntValue(const boost::property_tree::ptree& root, std::string path, int defaultValue) const;
	int GetIntValue(const boost::property_tree::ptree& root, std::string path) const;

	double GetDoubleValue(const boost::property_tree::ptree& root, std::string path, double defaultValue) const;
	double GetDoubleValue(const boost::property_tree::ptree& root, std::string path) const;

	std::string GetStringValue(const boost::property_tree::ptree& root, std::string path, std::string defaultValue) const;
	std::string GetStringValue(const boost::property_tree::ptree& root, std::string path) const;

	boost::property_tree::ptree m_root;
};