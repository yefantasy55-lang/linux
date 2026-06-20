#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <set>
#include "Command.hpp"
#include "InetAddr.hpp"
#include "Log.hpp"

using namespace yzq;

class Command
{
public:
    Command()
    {
        _WhiteListCommands.insert("ls");
        _WhiteListCommands.insert("pwd");
        _WhiteListCommands.insert("ls -l");
        _WhiteListCommands.insert("who");
        _WhiteListCommands.insert("whoami");
    }

    ~Command()
    {
    }

    bool IsSafeCommand(const std::string &cmd)
    {
        auto iter = _WhiteListCommands.find(cmd);
        return iter != _WhiteListCommands.end();
    }

    std::string Execute(const std::string &cmd, InetAddr &addr)
    {
        if (!IsSafeCommand(cmd))
        {
            return std::string("非法操作，令行禁止!");
        }

        std::string who = addr.StringAddr(); // 输入命令的用户
        FILE *fp = popen(cmd.c_str(), "r");  // 执行命令
        if (fp == nullptr)
        {
            return std::string("该命令不存在:" + cmd);
        }

        std::string res;
        char line[1024];
        while (fgets(line, sizeof(line), fp))
        {
            res += line; // 运行结果
        }

        std::string result = who + "execute done, result is: \n" + res;
        LOG(LogLevel::DEBUG) << result;
        return result;
    }

private:
    std::set<std::string> _WhiteListCommands; // 命令白名单
};