#include <iostream>
#include "statement.hpp"
#include "executor.hpp"

using namespace std;

void print_prompt()
{
    cout << "minisql > ";
}

int main()
{
    Table *table = new Table("mydb.db");
    string input_line;

    while (true)
    {
        print_prompt();
        getline(cin, input_line);

        if (input_line.empty())
            continue;

        if (input_line[0] == '.')
        {
            if (input_line == ".exit")
            {
                for (uint32_t i = 0; i < TABLE_MAX_PAGES; ++i)
                {
                    if (table->pager->pages[i] != nullptr)
                    {
                        table->pager->flush_page(i, PAGE_SIZE);
                        free(table->pager->pages[i]);
                    }
                }
                delete table;
                cout << "Bye!" << endl;
                exit(0);
            }
            else
            {
                cout << "Unrecognized command: " << input_line << endl;
                continue;
            }
        }

        Statement statement;
        PrepareResult prep = prepare_statement(input_line, &statement);

        if (prep == PrepareResult::PREPARE_SUCCESS)
        {
            execute_statement(&statement, table);
            cout << "Executed." << endl;
        }
        else if (prep == PrepareResult::PREPARE_SYNTAX_ERROR)
        {
            cout << "Syntax error." << endl;
        }
        else
        {
            cout << "Unrecognized statement." << endl;
        }
    }

    return 0;
}
