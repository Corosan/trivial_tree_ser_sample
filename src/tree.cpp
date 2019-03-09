#include "tree.h"

#include <stdexcept>
#include <iostream>
#include <locale>
#include <iterator>
#include <queue>
#include <algorithm>
#include <sstream>

namespace {

std::string trim(const std::string& value) {
    auto beg = find_if_not(value.begin(), value.end(),
        [l = std::locale()](auto c){ return std::isspace(c, l); });
    auto rbeg = find_if_not(value.rbegin(), std::string::const_reverse_iterator(beg),
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

tree::data_t tree::parse_from_string(const std::string& value) {
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
    return value;
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
                std::unique_ptr<node> child{new node{parse_from_string(token), {}}};
                queue.push(child.get());
                queue.front()->m_children.push_back(std::move(child));
            }
            queue.pop();
            parent_got = false;
        } else {
            auto data = parse_from_string(trim(line));
            if (queue.empty()) {
                m_root.reset(new node{std::move(data), {}});
                queue.push(m_root.get());
            } else if (queue.front()->m_data != data) {
                throw std::runtime_error(
                    "line " + std::to_string(line_n) + ": unexpected parent according to BFS, "
                    "expected: " + (std::ostringstream() << queue.front()->m_data).str());
            }
            parent_got = true;
        }
    }
}
