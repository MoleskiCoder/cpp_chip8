#pragma once

#include <cstdint>
#include <string>

#include <boost/property_tree/ptree.hpp>

class ConfigurationReader {

public:
	ConfigurationReader(const std::string& path);

	bool GetBooleanValue(const std::string& path, bool defaultValue) const;
	bool GetBooleanValue(const std::string& path) const;

	uint8_t GetByteValue(const std::string& path, uint8_t defaultValue) const;
	uint8_t GetByteValue(const std::string& path) const;

	uint16_t GetUShortValue(const std::string& path, uint16_t defaultValue) const;
	uint16_t GetUShortValue(const std::string& path) const;

	int GetIntValue(const std::string& path, int defaultValue) const;
	int GetIntValue(const std::string& path) const;

	double GetDoubleValue(const std::string& path, double defaultValue) const;
	double GetDoubleValue(const std::string& path) const;

	std::string GetStringValue(const std::string& path, const std::string& defaultValue) const;
	std::string GetStringValue(const std::string& path) const;

private:
	bool GetBooleanValue(const boost::property_tree::ptree& root, const std::string& path, bool defaultValue) const;
	bool GetBooleanValue(const boost::property_tree::ptree& root, const std::string& path) const;

	uint8_t GetByteValue(const boost::property_tree::ptree& root, const std::string& path, uint8_t defaultValue) const;
	uint8_t GetByteValue(const boost::property_tree::ptree& root, const std::string& path) const;

	uint16_t GetUShortValue(const boost::property_tree::ptree& root, const std::string& path, uint16_t defaultValue) const;
	uint16_t GetUShortValue(const boost::property_tree::ptree& root, const std::string& path) const;

	int GetIntValue(const boost::property_tree::ptree& root, const std::string& path, int defaultValue) const;
	int GetIntValue(const boost::property_tree::ptree& root, const std::string& path) const;

	double GetDoubleValue(const boost::property_tree::ptree& root, const std::string& path, double defaultValue) const;
	double GetDoubleValue(const boost::property_tree::ptree& root, const std::string& path) const;

	std::string GetStringValue(const boost::property_tree::ptree& root, const std::string& path, std::string defaultValue) const;
	std::string GetStringValue(const boost::property_tree::ptree& root, const std::string& path) const;

	boost::property_tree::ptree m_root;
};