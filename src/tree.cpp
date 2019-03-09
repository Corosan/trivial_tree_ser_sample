#include "tree.h"

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <locale>
#include <iterator>
#include <queue>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace {

/**
 * Remove spaces from the beginning and the end of a string corresponding to
 * current locale. Work without any allocations
 */
std::string_view trim(const std::string_view& value) {
    auto beg = std::find_if_not(value.begin(), value.end(),
        [l = std::locale()](auto c){ return std::isspace(c, l); });
    auto rbeg = std::find_if_not(value.rbegin(), std::string_view::const_reverse_iterator(beg),
        [l = std::locale()](auto c){ return std::isspace(c, l); });

    return value.substr(beg - value.begin(), rbeg.base() - beg);
}

}

namespace std {

ostream& operator<<(ostream& os, const tree::data_t& data) {
    visit([&os](auto c) { os << c; }, data);
    return os;
}

}

tree::data_t tree::parse_from_string(std::string&& value) {
    try {
        std::size_t pos = 0;
        int v = std::stoi(value, &pos);
        if (pos == value.size())
            return v;
    } catch(...) {}
    try {
        std::size_t pos = 0;
        double v = std::stod(value, &pos);
        if (pos == value.size())
            return v;
    } catch(...) {}

    // RVO can't be used because returning type is different from the value's type.
    // Let's tell a compiler to move it into a newly constructed data_t to avoid
    // complaining in -Wall mode
    return std::move(value);
}

void tree::serialize(std::ostream& oss) {
    std::queue<node*> queue;

    if (m_root)
        queue.push(m_root.get());

    while (! queue.empty()) {
        const auto& the_node = *queue.front();
        oss << the_node.m_data << '\n';
        bool first = true;
        for (auto& ch : the_node.m_children) {
            if (! first)
                oss << ' ';
            first = false;
            oss << ch->m_data;
            if (! ch->m_children.empty())
                queue.push(ch.get());
        }
        queue.pop();
        oss << std::endl;
    }
}

void tree::deserialize(std::istream& iss) {
    std::string line;
    std::queue<node*> queue;
    bool parent_got = false;
   
    int line_n = 0; 
    while (getline(iss, line)) {
        ++line_n;

        if (parent_got) {
            std::istringstream ils(line);
            std::string token;
            while (getline(ils, token, ' ')) {
                if (token.empty())
                    continue;
                std::unique_ptr<node> child{new node{parse_from_string(std::move(token)), {}}};
                queue.push(child.get());
                queue.front()->m_children.push_back(std::move(child));
            }
            queue.pop();
            parent_got = false;
        } else {
            auto data = parse_from_string(trim(line));
            if (queue.empty()) {
                // Note that the method is safe for multiple calls while already having some loaded
                // tree. '.reset()' will destroy the whole old tree correctly.
                m_root.reset(new node{std::move(data), {}});
                queue.push(m_root.get());
            } else {
                for (; ! queue.empty(); queue.pop())
                    if (queue.front()->m_data == data)
                        break;
                if (queue.empty())
                    throw std::runtime_error(
                        "line " + std::to_string(line_n) + ": unexpected parent according to BFS");
            }
            parent_got = true;
        }
    }
}

std::ostream& tree::print(std::ostream& oss, int initial_indent) {
    if (! m_root) {
        if (initial_indent)
            oss << std::setw(initial_indent) << ' ' << "<empty>" << std::endl;
    }
    else {
        oss << std::left;
        print_impl(oss, 0, initial_indent, *m_root);
        oss.flush();
    }

    return oss;
}

std::ostream& tree::make_indent(std::ostream& oss, int level, int initial_indent) {
    if (initial_indent)
        oss << std::setw(initial_indent) << ' ';
    for (int l = 0; l < level - 1; ++l)
        oss << std::setw(s_indent_size) << '|';

    return oss;
}

void tree::print_impl(std::ostream& oss, int level, int initial_indent, node& the_node) {
    make_indent(oss, level, initial_indent);

    if (level)
        // setw() influences to the next printed entity only, but setfill() - not
        oss << std::setw(s_indent_size) << std::setfill('-') << '+' << std::setfill(' ');

    // It's better to use '\n' instead of std::endl every time when 'new-line' symbol is needed to
    // be inserted because it doesn't force a stream to be flushed too frequently. Flushing can
    // be executed at the end. This design decision can be changed if it's planned to print huge
    // structures with megabytes of data.
    oss << the_node.m_data << '\n';
    for (auto& ch : the_node.m_children)
        print_impl(oss, level + 1, initial_indent, *ch);

    if (! the_node.m_children.empty())
        make_indent(oss, level + 1, initial_indent) << '\n';
}
