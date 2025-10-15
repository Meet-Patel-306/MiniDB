#pragma once
#include "pager.hpp"
#include "statement.hpp"
#include <iostream>
using namespace std;

class Table
{
public:
    Pager *pager;
    uint32_t num_rows;
    const string filename;

    Table(const string &filename) : filename(filename)
    {
        pager = new Pager(filename);

        // Load header (first 8 bytes)
        if (pager->file_length >= sizeof(uint32_t))
        {
            pager->file_stream.seekg(0);
            pager->file_stream.read(reinterpret_cast<char *>(&num_rows), sizeof(uint32_t));
        }
        else
        {
            num_rows = 0;
        }

        cout << "Loaded " << num_rows << " existing rows from " << filename << endl;
    }

    ~Table()
    {
        // Write row count before closing
        pager->file_stream.seekp(0);
        pager->file_stream.write(reinterpret_cast<char *>(&num_rows), sizeof(uint32_t));
        pager->file_stream.flush();

        delete pager;
    }
};

// Function to find row memory location
void *row_slot(Table *table, uint32_t row_num)
{
    uint32_t header_size = sizeof(uint32_t); // Skip 4 bytes for num_rows
    uint32_t byte_offset = header_size + (row_num * ROW_SIZE);

    uint32_t page_num = byte_offset / PAGE_SIZE;
    void *page = table->pager->get_page(page_num);
    uint32_t page_offset = byte_offset % PAGE_SIZE;

    return (char *)page + page_offset;
}
