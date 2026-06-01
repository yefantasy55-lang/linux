#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include "Log.hpp"
#include "InetAddr.hpp"

const std::string defaultdict = "./dictionary.txt";
const std::string sep = ": "; // 默认词典分割符为:
using namespace yzq;

class Dict
{
public:
    Dict(const std::string &path = defaultdict)
        : _dict_path(path)
    {
    }

    // 把词典加载到内存中
    bool LoadDict()
    {
        std::ifstream in(_dict_path); // 创建文件流对象
        if (!in.is_open())
        {
            LOG(LogLevel::DEBUG) << "打开字典: " << _dict_path << " 错误";
            return false;
        }

        std::string line; // 按行读取
        while (std::getline(in, line))
        {
            // apple: 苹果
            auto pos = line.find(sep);    // 寻找分割符的位置
            if (pos == std::string::npos) // 找不到分隔符
            {
                LOG(LogLevel::WARNING) << "解析: " << line << " 失败";
                continue;
            }
            std::string english = line.substr(0, pos);           // 左半部分为英文
            std::string chinese = line.substr(pos + sep.size()); // 右半部分为中文
            if (english.empty() || chinese.empty())              // 中文或英文缺少跳过
            {
                LOG(LogLevel::WARNING) << "没有有效内容: " << line;
                continue;
            }
            _dict.insert(std::make_pair(english, chinese));
            LOG(LogLevel::DEBUG) << "加载: " << line;
        }

        return true;
    }

    std::string Translate(const std::string &word, InetAddr &client)
    {
        auto iter = _dict.find(word);
        if (iter == _dict.end()) // 找不到
        {
            LOG(LogLevel::DEBUG) << "进入到了翻译模块, [" << client.Ip() << " : " << client.Port() << "]# " << word << "->None";
            return "None";
        }   
        LOG(LogLevel::DEBUG) << "进入到了翻译模块, [" << client.Ip() << " : " << client.Port() << "]# " << word << "->" << iter->second;
        return iter->second;
    }

    ~Dict()
    {
    }

private:
    std::string _dict_path;                             // 路径+文件名
    std::unordered_map<std::string, std::string> _dict; // 映射的词典
};