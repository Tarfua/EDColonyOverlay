#pragma once

#include <string>
#include <unordered_map>
#include <algorithm>

namespace edcolony {

inline std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return s;
}

inline std::string trimSpaces(std::string s) {
    auto not_space = [](unsigned char c){ return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    // collapse inner spaces
    std::string out; out.reserve(s.size());
    bool prev_space = false;
    for (char c : s) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!prev_space) out.push_back(' ');
            prev_space = true;
        } else {
            out.push_back(c);
            prev_space = false;
        }
    }
    return out;
}

// Map known aliases/typos to canonical names
inline const std::unordered_map<std::string, std::string>& commodityAliasMap() {
    static const std::unordered_map<std::string, std::string> m = {
        // {"power regulator", "Power Regulator"},
        // {"power regulators", "Power Regulator"},
        // Extend with real mappings as we meet them
    };
    return m;
}

inline std::string normalizeCommodityName(const std::string& name) {
    std::string s = trimSpaces(name);
    std::string key = toLower(s);
    auto it = commodityAliasMap().find(key);
    if (it != commodityAliasMap().end()) return it->second;
    return s; // default: keep original casing after trimming
}

}


