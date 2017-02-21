#include "stdafx.h"
#include "ConfigurationReader.h"

#include <boost/property_tree/json_parser.hpp>

ConfigurationReader::ConfigurationReader(std::string path) {
	boost::property_tree::read_json(path, m_root);
}

bool ConfigurationReader::GetBooleanValue(std::string path, bool defaultValue) const {
	return GetBooleanValue(m_root, path, defaultValue);
}

bool ConfigurationReader::GetBooleanValue(std::string path) const {
	return GetBooleanValue(path, false);
}

uint8_t ConfigurationReader::GetByteValue(std::string path, uint8_t defaultValue) const {
	return GetByteValue(m_root, path, defaultValue);
}

uint8_t ConfigurationReader::GetByteValue(std::string path) const {
	return GetByteValue(path, 0);
}

uint16_t ConfigurationReader::GetUShortValue(std::string path, uint16_t defaultValue) const {
	return GetUShortValue(m_root, path, defaultValue);
}

uint16_t ConfigurationReader::GetUShortValue(std::string path) const {
	return GetUShortValue(path, 0);
}

int ConfigurationReader::GetIntValue(std::string path, int defaultValue) const {
	return GetIntValue(m_root, path, defaultValue);
}

int ConfigurationReader::GetIntValue(std::string path) const {
	return GetIntValue(path, 0);
}

double ConfigurationReader::GetDoubleValue(std::string path, double defaultValue) const {
	return GetDoubleValue(m_root, path, defaultValue);
}

double ConfigurationReader::GetDoubleValue(std::string path) const {
	return GetDoubleValue(path, 0);
}

std::string ConfigurationReader::GetStringValue(std::string path, std::string defaultValue) const {
	return GetStringValue(m_root, path, defaultValue);
}

std::string ConfigurationReader::GetStringValue(std::string path) const {
	return GetStringValue(path, "");
}

//

bool ConfigurationReader::GetBooleanValue(const boost::property_tree::ptree& root, std::string path, bool defaultValue) const {
	return root.get(path, defaultValue);
}

bool ConfigurationReader::GetBooleanValue(const boost::property_tree::ptree& root, std::string path) const {
	return GetBooleanValue(root, path, false);
}

uint8_t ConfigurationReader::GetByteValue(const boost::property_tree::ptree& root, std::string path, uint8_t defaultValue) const {
	return root.get(path, defaultValue);
}

uint8_t ConfigurationReader::GetByteValue(const boost::property_tree::ptree& root, std::string path) const {
	return GetByteValue(root, path, 0);
}

uint16_t ConfigurationReader::GetUShortValue(const boost::property_tree::ptree& root, std::string path, uint16_t defaultValue) const {
	std::stringstream conversion_stream;
	auto read = GetStringValue(root, path);
	if (read == "")
		return defaultValue;
	conversion_stream << std::hex << read;
	if (conversion_stream.fail())
		return defaultValue;
	uint16_t returnValue;
	conversion_stream >> returnValue;
	if (conversion_stream.fail())
		return defaultValue;
	return returnValue;
}

uint16_t ConfigurationReader::GetUShortValue(const boost::property_tree::ptree& root, std::string path) const {
	return GetUShortValue(root, path, 0);
}

int ConfigurationReader::GetIntValue(const boost::property_tree::ptree& root, std::string path, int defaultValue) const {
	return root.get(path, defaultValue);
}

int ConfigurationReader::GetIntValue(const boost::property_tree::ptree& root, std::string path) const {
	return GetIntValue(root, path, 0);
}

double ConfigurationReader::GetDoubleValue(const boost::property_tree::ptree& root, std::string path, double defaultValue) const {
	return root.get(path, defaultValue);
}

double ConfigurationReader::GetDoubleValue(const boost::property_tree::ptree& root, std::string path) const {
	return GetDoubleValue(root, path, 0.0);
}

std::string ConfigurationReader::GetStringValue(const boost::property_tree::ptree& root, std::string path, std::string defaultValue) const {
	return root.get(path, defaultValue);
}

std::string ConfigurationReader::GetStringValue(const boost::property_tree::ptree& root, std::string path) const {
	return GetStringValue(root, path, "");
}

