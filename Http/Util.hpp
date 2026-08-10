#pragma once

#include <iostream>
#include <fstream>
#include <string>

class Util
{
public:
    // 读取完整载荷（正文）
    // version1 ------ 处理二进制文件时有bug，且长文本会频繁扩容，影响效率
    // static bool ReadFileContent(const std::string &filename, std::string *out)
    // {
    //     std::ifstream in(filename);
    //     if (!in.is_open()) // 打开失败
    //     {
    //         return false;
    //     }
    //     std::string line;
    //     while (std::getline(in, line)) // 逐行读取
    //     {
    //         *out += line;
    //     }
    //     in.close();
    //     return true;
    // }

    // version2
    static bool ReadFileContent(const std::string &filename, std::string *out)
    {
        int filesize = FileSize(filename);
        if (filesize > 0)
        {
            std::ifstream in(filename);
            if (!in.is_open()) // 打开失败
            {
                return false;
            }
            out->resize(filesize); // 一次性扩容
            in.read(&(*out)[0], filesize);
            in.close();
            return true;
        }
        else
        {
            return false;
        }
    }

    // 读取指定单行内容
    static bool ReadOneLine(std::string &bigstr, std::string *out, const std::string &sep)
    {
        auto pos = bigstr.find(sep);
        if (pos == std::string::npos)
        {
            return false;
        }
        *out = bigstr.substr(0, pos);
        bigstr.erase(0, pos + sep.size());
        return true;
    }

    // 返回文件大小
    static int FileSize(const std::string &filename)
    {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open())
        {
            return -1;
        }
        in.seekg(0, in.end);
        return in.tellg();
    }
};