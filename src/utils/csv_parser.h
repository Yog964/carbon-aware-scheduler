#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

namespace carbongrid {

/**
 * @brief A row in a CSV file, providing field-based access.
 */
struct CSVRow {
    std::vector<std::string> fields;
    
    const std::string& operator[](std::size_t index) const {
        return fields[index];
    }
    
    std::string& operator[](std::size_t index) {
        return fields[index];
    }
    
    std::size_t size() const {
        return fields.size();
    }
};

/**
 * @brief Simple header-only CSV parser that correctly handles quoted strings.
 */
class CSVParser {
public:
    static std::vector<CSVRow> parse(const std::string& filepath, bool has_header = true) {
        std::vector<CSVRow> rows;
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filepath);
        }
        
        std::string line;
        bool first_line = true;
        while (std::getline(file, line)) {
            if (first_line && has_header) {
                first_line = false;
                continue;
            }
            if (line.empty()) continue;
            
            rows.push_back(parse_line(line));
        }
        return rows;
    }
    
    static std::vector<std::string> get_headers(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filepath);
        }
        
        std::string line;
        if (std::getline(file, line)) {
            return parse_line(line).fields;
        }
        return {};
    }

private:
    static CSVRow parse_line(const std::string& line) {
        CSVRow row;
        std::string current_field;
        bool in_quotes = false;
        
        for (std::size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            if (c == '"') {
                if (in_quotes && i + 1 < line.length() && line[i+1] == '"') {
                    current_field += '"';
                    ++i;
                } else {
                    in_quotes = !in_quotes;
                }
            } else if (c == ',' && !in_quotes) {
                row.fields.push_back(current_field);
                current_field.clear();
            } else {
                current_field += c;
            }
        }
        row.fields.push_back(current_field);
        return row;
    }
};

} // namespace carbongrid
