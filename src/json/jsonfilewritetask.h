#ifndef JSONFILEWRITETASK_H
#define JSONFILEWRITETASK_H

#include <fstream>
#include <string>
#include <vector>
#include <tbb/tbb.h>
#include <archive.h>
#include <archive_entry.h>

#include "jsonwriter.h"

class JSONTextFileWriteTask : public tbb::task
{
public:
    JSONTextFileWriteTask (std::ofstream& json_file, std::vector <std::string>&& text, JSONWriter& json_writer)
        : json_file_(json_file), text_(text), json_writer_(json_writer)
    {    }

    /*override*/ tbb::task* execute()
    {
        for (const std::string str_it : text_)
            json_file_ << str_it;

        json_writer_.fileWritingDone();

        return nullptr; // or a pointer to a new task to be executed immediately
    }

private:
    std::ofstream& json_file_;
    std::vector <std::string> text_;
    JSONWriter& json_writer_;
};

class JSONBinaryFileWriteTask : public tbb::task
{
public:
    JSONBinaryFileWriteTask (std::ofstream& json_file, std::vector <std::vector<std::uint8_t>>&& data,
                             JSONWriter& json_writer)
        : json_file_(json_file), data_(data), json_writer_(json_writer)
    {}

    /*override*/ tbb::task* execute()
    {
        for (const std::vector<std::uint8_t>& bin_it : data_)
            json_file_.write (reinterpret_cast<const char*>(bin_it.data()), bin_it.size());

        json_writer_.fileWritingDone();

        return nullptr; // or a pointer to a new task to be executed immediately
    }

private:
    std::ofstream& json_file_;
    std::vector <std::vector<std::uint8_t>> data_;
    JSONWriter& json_writer_;
};


class JSONTextZipFileWriteTask : public tbb::task
{
public:
    JSONTextZipFileWriteTask (struct archive* json_zip_file, std::vector <std::string>&& text, JSONWriter& json_writer)
        : json_zip_file_(json_zip_file), text_(text), json_writer_(json_writer)
    {    }

    /*override*/ tbb::task* execute()
    {
        createEntry();

        for (const std::string str_it : text_)
            archive_write_data (json_zip_file_, str_it.c_str(), str_it.size());;

        json_writer_.fileWritingDone();

        endEntry();

        return nullptr; // or a pointer to a new task to be executed immediately
    }

private:
    struct archive* json_zip_file_;
    struct archive_entry* json_zip_file_entry_ {nullptr};

    static size_t entry_cnt_;

    std::vector <std::string> text_;
    JSONWriter& json_writer_;

    void createEntry()
    {
        size_t byte_size=0;

        for (auto& str : text_)
            byte_size += str.size();

        assert (byte_size);

        json_zip_file_entry_ = archive_entry_new();

        archive_entry_set_pathname(json_zip_file_entry_, ("entry"+std::to_string(entry_cnt_)+".json").c_str());
        archive_entry_set_size(json_zip_file_entry_, byte_size);
        archive_entry_set_filetype(json_zip_file_entry_, AE_IFREG);
        archive_entry_set_perm(json_zip_file_entry_, 0644);
        archive_write_header(json_zip_file_, json_zip_file_entry_);
        ++entry_cnt_;
    }

    void endEntry()
    {
        archive_entry_free(json_zip_file_entry_);
    }
};

class JSONBinaryZipFileWriteTask : public tbb::task
{
public:
    JSONBinaryZipFileWriteTask (struct archive* json_zip_file, std::vector <std::vector<std::uint8_t>>&& data,
                             JSONWriter& json_writer)
        : json_zip_file_(json_zip_file), data_(data), json_writer_(json_writer)
    {}

    /*override*/ tbb::task* execute()
    {
        for (const std::vector<std::uint8_t>& bin_it : data_)
            archive_write_data (json_zip_file_, reinterpret_cast<const void*>(bin_it.data()), bin_it.size());

        json_writer_.fileWritingDone();

        return nullptr; // or a pointer to a new task to be executed immediately
    }

private:
    struct archive* json_zip_file_;
    std::vector <std::vector<std::uint8_t>> data_;
    JSONWriter& json_writer_;
};

#endif // JSONFILEWRITETASK_H
