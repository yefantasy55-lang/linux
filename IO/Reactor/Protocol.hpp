#pragma once

#include <string>
#include <iostream>
#include <memory>
#include "Socket.hpp"
#include <json/json.h>

using namespace yzq;

// client -> server
class Request
{
public:
    Request()
    {
    }

    Request(int x, int y, char oper)
        : _x(x), _y(y), _oper(oper)
    {
    }

    std::string Serialize()
    {
        Json::Value root;
        root["x"] = _x;
        root["y"] = _y;
        root["oper"] = _oper;

        Json::FastWriter writer;
        std::string res = writer.write(root);
        return res;
    }

    bool Deserialize(std::string &in)
    {
        // {"x": 10, "y" : 20, "oper" : '+'}
        Json::Value root;
        Json::Reader reader;

        bool ok = reader.parse(in, root);
        if (ok)
        {
            _x = root["x"].asInt();
            _y = root["y"].asInt();
            _oper = root["oper"].asInt();
        }
        return ok;
    }

    int X()
    {
        return _x;
    }

    int Y()
    {
        return _y;
    }

    char Oper()
    {
        return _oper;
    }

    ~Request()
    {
    }

private:
    int _x;
    int _y;
    char _oper;
};

// server -> client
class Response
{
public:
    Response()
    {
    }

    Response(int res, int code)
        : _res(res), _code(code)
    {
    }

    std::string Serialize()
    {
        Json::Value root;
        root["res"] = _res;
        root["code"] = _code;

        Json::FastWriter writer;
        std::string s = writer.write(root);
        return s;
    }

    bool Deserialize(std::string &in)
    {
        Json::Value root;
        Json::Reader reader;

        bool ok = reader.parse(in, root);
        if (ok)
        {
            _res = root["res"].asInt();
            _code = root["code"].asInt();
        }
        return ok;
    }

    void SetResult(int res)
    {
        _res = res;
    }

    void SetCode(int code)
    {
        _code = code;
    }

    // 展示结果
    void ShowResult()
    {
        std::cout << "计算结果是: " << _res << "[" << _code << "]" << std::endl;
    }

    ~Response()
    {
    }

private:
    int _res;
    int _code; // success:0 , other err:0 1 2 ...
};

const std::string sep = "\r\n"; // 报文分隔符
using func_t = std::function<Response(Request &req)>;

// 协议处理数据
class Protocol
{
public:
    Protocol()
    {
    }

    Protocol(func_t func)
        : _func(func)
    {
    }

    std::string Encode(const std::string &jsonstr)
    {
        // 50\r\n{"x":10,"y":20,"oper":'+'}\r\n
        std::string len = std::to_string(jsonstr.size());
        return len + sep + jsonstr + sep; // 应用层封装报头
    }

    bool Decode(std::string &buffer, std::string *package)
    {
        // 50\r\n{"x":10,"y":20,"oper":'+'}\r\n
        ssize_t pos = buffer.find(sep);
        if (pos == std::string::npos)
        {
            return false; // 让调用方继续从内核中读取数据
        }
        std::string package_len_str = buffer.substr(0, pos);                        // 报文长度标志
        int package_len_int = std::stoi(package_len_str);                           // 有效报文长度
        int target_len = package_len_str.size() + package_len_int + sep.size() * 2; // 完整报文长度
        if (buffer.size() < target_len)                                             // 当缓冲区数据长度小于一条完整报文时返回
        {
            return false;
        }
        *package = buffer.substr(pos + sep.size(), package_len_int); // 取出一条有效报文
        buffer.erase(0, target_len);                                 // 移除一条完整报文
        return true;
    }

    // 处理请求数据
    std::string Execute(std::string &json_package)
    {

        LOG(LogLevel::DEBUG) << "进入到protocol Execute begnin";

        Request rq;
        rq.Deserialize(json_package);

        Response rp = _func(rq);

        std::string json_str = rp.Serialize();

        std::string message = Encode(json_str);

        LOG(LogLevel::DEBUG) << "进入到protocol Execute end";

        return message;
    }

    ~Protocol()
    {
    }

private:
    func_t _func;
};