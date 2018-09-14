/**
 * MIT License
 *
 * Copyright (c) 2018 Younguk Kim (bluekyu)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <string>

#include <boost/optional.hpp>

#include <filename.h>

namespace rpplugins {

class RPStatPlugin;

class FileDialog
{
public:
    enum class OperationFlag: int
    {
        open = 0,
        write
    };

public:
    FileDialog(RPStatPlugin& plugin, OperationFlag op_flag, const std::string& id = "FileDialog");

    bool draw();

    const boost::optional<Filename>& get_filename() const;

private:
    void open_warning_popup(const std::string& msg);
    void open_error_popup(const std::string& msg);

    void process_file_drop();
    void draw_warning_popup();
    void draw_error_popup();

    void accept();
    void reject();

    RPStatPlugin& plugin_;
    const std::string id_;
    const OperationFlag operation_flag_;
    std::string buffer_;

    bool closed_ = true;
    boost::optional<Filename> fname_;
    std::string popup_message_;
};

inline const boost::optional<Filename>& FileDialog::get_filename() const
{
    return fname_;
}

}
