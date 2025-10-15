#pragma once
#include "table.hpp"

using namespace std;

enum class ExecuteResult
{
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL
};

inline void print_row(const Row *row)
{
    cout << "(" << row->id << ", " << row->username << ", " << row->email << ")" << endl;
}

inline ExecuteResult execute_insert(Statement *statement, Table *table)
{
    if (table->num_rows >= (TABLE_MAX_PAGES * (PAGE_SIZE / ROW_SIZE)))
    {
        cout << "Error: Table full." << endl;
        return ExecuteResult::EXECUTE_TABLE_FULL;
    }

    void *slot = row_slot(table, table->num_rows);
    serialize_row(&statement->row_to_insert, slot);
    table->num_rows += 1;
    return ExecuteResult::EXECUTE_SUCCESS;
}

inline ExecuteResult execute_select(Statement *statement, Table *table)
{
    Row row;
    for (uint32_t i = 0; i < table->num_rows; ++i)
    {
        void *slot = row_slot(table, i);
        deserialize_row(slot, &row);
        print_row(&row);
    }
    return ExecuteResult::EXECUTE_SUCCESS;
}

inline ExecuteResult execute_statement(Statement *statement, Table *table)
{
    switch (statement->type)
    {
    case StatementType::STATEMENT_INSERT:
        return execute_insert(statement, table);
    case StatementType::STATEMENT_SELECT:
        return execute_select(statement, table);
    }
    return ExecuteResult::EXECUTE_SUCCESS;
}
