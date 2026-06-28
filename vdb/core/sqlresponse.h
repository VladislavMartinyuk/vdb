#ifndef VDB_SQLRESPONSE_H
#define VDB_SQLRESPONSE_H

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace vdb {

class SqlVariant
{
public:
    SqlVariant() = default;
    explicit SqlVariant(std::variant<int, double, std::string, bool, long> value)
        : m_value(value)
    {}
    explicit SqlVariant(int value)
        : m_value(value)
    {}
    explicit SqlVariant(double value)
        : m_value(value)
    {}
    explicit SqlVariant(std::string value)
        : m_value(value)
    {}
    explicit SqlVariant(bool value)
        : m_value(value)
    {}
    explicit SqlVariant(long value)
        : m_value(value)
    {}

    int toInt() const { return std::get<int>(m_value); }
    double toDouble() const { return std::get<double>(m_value); }
    std::string toString() const { return std::get<std::string>(m_value); }
    bool toBool() const { return std::get<bool>(m_value); }
    long toLong() const { return std::get<long>(m_value); }

private:
    std::variant<int, double, std::string, bool, long> m_value;
};

using SqlRow = std::unordered_map<std::string, SqlVariant>;
using SqlRows = std::vector<SqlRow>;
enum class SqlVariantTypes { Integer, Double, String, Boolean, Long };

class SqlResponse
{
public:
    SqlResponse() = default;

    void push(SqlRow row);
    SqlRows rows() const;
    void clear();
    bool isEmpty() const;

private:
    SqlRows m_rows;
};

} // namespace vdb

#endif //VDB_SQLRESPONSE_H
