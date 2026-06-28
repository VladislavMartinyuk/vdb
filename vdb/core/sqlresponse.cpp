#include "sqlresponse.h"

using namespace vdb;

void SqlResponse::push(SqlRow row)
{
    if (row.empty()) {
        return;
    }
    m_rows.push_back(std::move(row));
}

SqlRows SqlResponse::rows() const
{
    return m_rows;
}

void SqlResponse::clear()
{
    m_rows.clear();
}

bool SqlResponse::isEmpty() const
{
    return m_rows.empty();
}