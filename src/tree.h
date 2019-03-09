#pragma once

#include <variant>
#include <vector>
#include <string>
#include <string_view>
#include <memory>
#include <iosfwd>

/**
 * Trivial sample of a binary tree which can be serialized/deserialized into a text stream
 * and pretty printed also. Written as a sample for Evgeny Medvedev
 */
class tree {
public:
    using data_t = std::variant<int, double, std::string>;

private:
    static constexpr int s_indent_size = 2;

    struct node {
        data_t m_data;
        std::vector<std::unique_ptr<node>> m_children;
    };

    std::unique_ptr<node> m_root;

    static data_t parse_from_string(const std::string_view& view_val) {
        return parse_from_string(std::string{view_val.begin(), view_val.end()});
    }
    static data_t parse_from_string(std::string&& value);

    static std::ostream& make_indent(std::ostream& oss, int level, int initial_indent);
    void print_impl(std::ostream& oss, int level, int initial_indent, node& the_node);

public:
    void serialize(std::ostream& oss);
    void deserialize(std::istream& iss);
    std::ostream& print(std::ostream& oss, int initial_indent = 0);

    // The object doesn't require to directly declare/define constructors/destructors/
    // copy/move operators because all is generated correctly by a compiler, thanks to
    // m_root unique_ptr.
};
