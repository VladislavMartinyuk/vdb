#ifndef VDB_SQLQUERYBUILDER_H
#define VDB_SQLQUERYBUILDER_H

#include <string>

namespace vdb {

class SqlQueryBuilder
{
public:
    SqlQueryBuilder() = default;

    SqlQueryBuilder& query(std::string_view query)
    {
        m_sql = query;
        return *this;
    }

    template<typename T>
    SqlQueryBuilder& bind(std::string_view prefix, T&& realValue)
    {
        std::size_t pos = 0;
        while ((pos = m_sql.find(prefix, pos)) != std::string::npos) {
            if constexpr (std::is_same_v<std::decay_t<T>, int>
                          || std::is_same_v<std::decay_t<T>, float>
                          || std::is_same_v<std::decay_t<T>, double>
                          || std::is_same_v<std::decay_t<T>, long>) {
                replace(pos, std::to_string(realValue));
            } else if constexpr (std::is_same_v<std::decay_t<T>, bool>) {
                std::string stringBool = realValue ? "true" : "false";
                replace(pos, stringBool);
            } else {
                replace(pos, realValue);
            }
        }
        return *this;
    }

    std::string build() const { return m_sql; }

private:
    std::string m_sql;

    void replace(std::size_t& pos, std::string_view str)
    {
        m_sql.replace(pos, str.length(), str);
        pos += str.length();
    }
};

}; // namespace vdb

#endif //VDB_SQLQUERYBUILDER_H
