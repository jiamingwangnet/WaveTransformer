#pragma once

#include <string>
#include <vector>

// Source - https://stackoverflow.com/a/868894
// Posted by iain, modified by community. See post 'Timeline' for change history
// Retrieved 2025-12-26, License - CC BY-SA 3.0

class InputParser {
public:
    InputParser(int& argc, char** argv) {
        for (int i = 1; i < argc; ++i)
            this->tokens.push_back(std::string(argv[i]));
    }
    /// @author iain
    const std::string& getCmdOption(const std::string& option) const {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
            return *itr;
        }
        static const std::string empty_string("");
        return empty_string;
    }
    /// @author iain
    bool cmdOptionExists(const std::string& option) const {
        return std::find(this->tokens.begin(), this->tokens.end(), option)
            != this->tokens.end();
    }
    std::vector<std::string> getMultipleOptions(const std::string& option) const 
    {
		// find the option and return all subsequent arguments until the next option or end
		std::vector<std::string> results;
		auto itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (itr != this->tokens.end())
        {
            ++itr; // move to the next token
            while (itr != this->tokens.end() && itr->rfind("-", 0) != 0)
            {
                results.push_back(*itr);
                ++itr;
            }
        }
		return results;
    }
private:
    std::vector <std::string> tokens;
};