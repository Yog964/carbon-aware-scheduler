#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace carbongrid {

/**
 * @brief Simple header-only JSON serialization utility.
 * Compatible with C++11/14 (no std::variant).
 */
class JsonObject {
public:
    JsonObject() = default;

    void set(const std::string& key, const std::string& val) {
        entries_.push_back({key, "\"" + escape(val) + "\""});
    }

    void set(const std::string& key, const char* val) {
        set(key, std::string(val));
    }

    void set(const std::string& key, int val) {
        entries_.push_back({key, std::to_string(val)});
    }

    void set(const std::string& key, double val) {
        std::ostringstream oss;
        oss << val;
        entries_.push_back({key, oss.str()});
    }

    void set(const std::string& key, bool val) {
        entries_.push_back({key, val ? "true" : "false"});
    }

    // Nested object
    void set(const std::string& key, const JsonObject& val) {
        entries_.push_back({key, val.to_string()});
    }

    // Array of strings
    void set_array(const std::string& key, const std::vector<std::string>& arr) {
        std::string result = "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            result += "\"" + escape(arr[i]) + "\"";
            if (i + 1 < arr.size()) result += ",";
        }
        result += "]";
        entries_.push_back({key, result});
    }

    // Array of doubles
    void set_array(const std::string& key, const std::vector<double>& arr) {
        std::string result = "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            std::ostringstream oss;
            oss << arr[i];
            result += oss.str();
            if (i + 1 < arr.size()) result += ",";
        }
        result += "]";
        entries_.push_back({key, result});
    }

    // Array of nested objects
    void set_array(const std::string& key, const std::vector<JsonObject>& arr) {
        std::string result = "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            result += arr[i].to_string();
            if (i + 1 < arr.size()) result += ",";
        }
        result += "]";
        entries_.push_back({key, result});
    }

    std::string to_string() const {
        std::string result = "{";
        for (size_t i = 0; i < entries_.size(); ++i) {
            result += "\"" + escape(entries_[i].first) + "\":" + entries_[i].second;
            if (i + 1 < entries_.size()) result += ",";
        }
        result += "}";
        return result;
    }

private:
    // Ordered key-value pairs (preserves insertion order)
    std::vector<std::pair<std::string, std::string>> entries_;

    static std::string escape(const std::string& str) {
        std::string escaped;
        for (char c : str) {
            if (c == '"') escaped += "\\\"";
            else if (c == '\\') escaped += "\\\\";
            else if (c == '\b') escaped += "\\b";
            else if (c == '\f') escaped += "\\f";
            else if (c == '\n') escaped += "\\n";
            else if (c == '\r') escaped += "\\r";
            else if (c == '\t') escaped += "\\t";
            else escaped += c;
        }
        return escaped;
    }
};

} // namespace carbongrid
