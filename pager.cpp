#include "pager.hpp"
#include <cstring>

Pager::Pager(const string &filename)
{
    file_stream.open(filename, ios::in | ios::out | ios::binary);
    if (!file_stream.is_open())
    {
        file_stream.open(filename, ios::out | ios::binary);
        file_stream.close();
        file_stream.open(filename, ios::in | ios::out | ios::binary);
    }

    file_stream.seekg(0, ios::end);
    file_length = file_stream.tellg();
    file_stream.seekg(0, ios::beg);
    num_pages = file_length / PAGE_SIZE;

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        pages[i] = nullptr;
    }
}

Pager::~Pager()
{
    for (uint32_t i = 0; i < num_pages; i++)
    {
        if (pages[i] != nullptr)
        {
            flush_page(i, PAGE_SIZE);
            free(pages[i]);
            pages[i] = nullptr;
        }
    }
    file_stream.close();
}

void *Pager::get_page(uint32_t page_num)
{
    if (page_num >= TABLE_MAX_PAGES)
    {
        cerr << "Page number out of bounds" << endl;
        exit(EXIT_FAILURE);
    }

    if (pages[page_num] == nullptr)
    {
        void *page = malloc(PAGE_SIZE);
        uint32_t num_pages_in_file = file_length / PAGE_SIZE;
        if (file_length % PAGE_SIZE)
            num_pages_in_file += 1;

        if (page_num < num_pages_in_file)
        {
            file_stream.seekg(page_num * PAGE_SIZE, ios::beg);
            file_stream.read((char *)page, PAGE_SIZE);
        }

        pages[page_num] = page;
        if (page_num >= num_pages)
            num_pages = page_num + 1;
    }

    return pages[page_num];
}

void Pager::flush_page(uint32_t page_num, uint32_t size)
{
    if (pages[page_num] == nullptr)
        return;
    file_stream.seekp(page_num * PAGE_SIZE, ios::beg);
    file_stream.write((char *)pages[page_num], size);
}
