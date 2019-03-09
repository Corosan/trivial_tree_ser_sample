#pragma once

#include <variant>
#include <vector>
#include <string>
#include <memory>
#include <iosfwd>

class tree {
public:
    using data_t = std::variant<int, double, std::string>;

private:
    struct node {
        data_t m_data;
        std::vector<std::unique_ptr<node>> m_children;
    };

    std::unique_ptr<node> m_root;

    static data_t parse_from_string(const std::string& value);

public:
    void serialize(std::ostream& oss);
    void deserialize(std::istream& iss);
};
