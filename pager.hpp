#pragma once
#include <fstream>
#include <cstdint>
#include <iostream>

using namespace std;

const uint32_t PAGE_SIZE = 4096;
const uint32_t TABLE_MAX_PAGES = 100;

class Pager
{
public:
    fstream file_stream;
    uint32_t file_length;
    uint32_t num_pages;
    void *pages[TABLE_MAX_PAGES];

    Pager(const string &filename);
    ~Pager();

    void *get_page(uint32_t page_num);
    void flush_page(uint32_t page_num, uint32_t size);
};
