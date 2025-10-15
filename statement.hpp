#pragma once
#include <cstring>
#include <string>
#include <cstdint>
#include <sstream>
#include <iostream>

using namespace std;

const uint32_t COLUMN_USERNAME_SIZE = 32;
const uint32_t COLUMN_EMAIL_SIZE = 255;

struct Row
{
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
};

// Row size calculations
const uint32_t ID_SIZE = sizeof(uint32_t);
const uint32_t USERNAME_SIZE = COLUMN_USERNAME_SIZE + 1;
const uint32_t EMAIL_SIZE = COLUMN_EMAIL_SIZE + 1;

const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

enum class StatementType
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
};

struct Statement
{
    StatementType type;
    Row row_to_insert;
};

enum class PrepareResult
{
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
};

inline void serialize_row(const Row *source, void *destination)
{
    memcpy((char *)destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy((char *)destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
    memcpy((char *)destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

inline void deserialize_row(void *source, Row *destination)
{
    memcpy(&(destination->id), (char *)source + ID_OFFSET, ID_SIZE);
    memcpy(destination->username, (char *)source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(destination->email, (char *)source + EMAIL_OFFSET, EMAIL_SIZE);
}

inline PrepareResult prepare_statement(const string &input_line, Statement *statement)
{
    if (input_line.rfind("insert", 0) == 0)
    {
        statement->type = StatementType::STATEMENT_INSERT;
        stringstream ss(input_line);
        string command, username, email;
        ss >> command >> statement->row_to_insert.id >> username >> email;

        if (ss.fail() || username.empty() || email.empty())
        {
            return PrepareResult::PREPARE_SYNTAX_ERROR;
        }

        if (username.size() > COLUMN_USERNAME_SIZE || email.size() > COLUMN_EMAIL_SIZE)
        {
            cout << "Error: String too long." << endl;
            return PrepareResult::PREPARE_SYNTAX_ERROR;
        }

        strncpy(statement->row_to_insert.username, username.c_str(), COLUMN_USERNAME_SIZE);
        statement->row_to_insert.username[COLUMN_USERNAME_SIZE] = '\0';
        strncpy(statement->row_to_insert.email, email.c_str(), COLUMN_EMAIL_SIZE);
        statement->row_to_insert.email[COLUMN_EMAIL_SIZE] = '\0';

        return PrepareResult::PREPARE_SUCCESS;
    }

    if (input_line == "select")
    {
        statement->type = StatementType::STATEMENT_SELECT;
        return PrepareResult::PREPARE_SUCCESS;
    }

    return PrepareResult::PREPARE_UNRECOGNIZED_STATEMENT;
}
