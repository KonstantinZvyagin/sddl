/*
 * This file is part of SDDL.
 *
 * SDDL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SDDL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with SDDL.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef JSONWRITER_H
#define JSONWRITER_H

#include "global.h"
#include "basics.h"

#include "common.h"
#include "common_data_types.h"
#include "common_structs.h"

#include <string>
#include <iostream>
#include <fstream>

#include <vector>

class archive;
class archive_entry;
struct JSONConvertible;
class JSONFileWriteTask;

class JSONWriter
{
public:
    JSONWriter(JSON_OUTPUT_TYPE json_output_type, const std::string& json_path);
    ~JSONWriter ();

    void write(t_Adsb *adsb_ptr);
    void write(t_Mlat *mlat_ptr);
    void write(t_Rsrv *rsrv_ptr);
    void write(t_Rtgt *rtgt_ptr);
    void write(t_Strk *strk_ptr);

    void fileWritingDone () { file_write_in_progress_ = false ;}

private:
    JSON_OUTPUT_TYPE json_output_type_;
    std::string json_path_;

    bool json_file_open_ {false};
    std::ofstream json_file_;

    bool json_zip_file_open_ {false};
    struct archive* json_zip_file_ {nullptr};
    struct archive_entry* json_zip_file_entry_ {nullptr};

    size_t rec_num_cnt_ {0};
    std::vector <JSONConvertible*> data_;
    std::vector <nlohmann::json> json_data_;
    std::vector <std::string> text_data_;
    std::vector <std::vector<std::uint8_t>> binary_data_;

    bool file_write_in_progress_ {false};

    void writeData();

    void convertJSON2Text ();
    void convertJSON2CBOR ();
    void convertJSON2UBJSON ();
    void convertJSON2MsgPack ();

    void openJsonFile ();
    void writeTextToFile ();
    void writeBinaryToFile ();
    void closeJsonFile ();

    void openJsonZipFile ();
    void writeTextToZipFile ();
    void writeBinaryToZipFile ();
    void closeJsonZipFile ();
};

#endif // JSONWRITER_H
