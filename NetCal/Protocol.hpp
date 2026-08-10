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
    void GetRequest(std::shared_ptr<Socket> &sock, InetAddr &client)
    {
        std::string buffer_queue; // 接收缓冲区
        while (true)
        {
            int n = sock->Recv(&buffer_queue);
            if (n > 0)
            {
                std::string json_package; // 一条有效报文

                // 1.解析客户端的报文
                while (Decode(buffer_queue, &json_package))
                {
                    // 2.得到一条有效报文后进行反序列化
                    Request rq;
                    bool ds_ok = rq.Deserialize(json_package);
                    if (!ds_ok)
                    {
                        continue; // 反序列失败跳过当前该报文
                    }

                    // 3.将反序列化后的数据交于应用层处理
                    Response rp = _func(rq);

                    // 4.将应用层处理完的结果序列化
                    std::string json_str = rp.Serialize();

                    // 5.封装报文
                    std::string message = Encode(json_str);

                    // 6.发送报文
                    sock->Send(message);
                };
            }
            else if (n == 0)
            {
                LOG(LogLevel::INFO) << "client:" << client.StringAddr() << " Quit!";
                break;
            }
            else
            {
                LOG(LogLevel::WARNING) << "client:" << client.StringAddr() << ", recv error";
                break;
            }
        }
    }

    // 获取应答报文
    bool GetResponse(std::shared_ptr<Socket> &client, std::string &resp_buff, Response *resp)
    {
        while (true)
        {
            int n = client->Recv(&resp_buff);
            if (n > 0)
            {
                std::string json_package; // 一条有效报文

                while (Decode(resp_buff, &json_package))
                {
                    resp->Deserialize(json_package);
                }
                return true;
            }
            else if (n == 0)
            {
                std::cout << "server quit " << std::endl;
                return false;
            }
            else
            {
                std::cout << "recv error" << std::endl;
                return false;
            }
        }
    }

    // 构建json请求报文
    std::string BuildRequestString(int &x, int &y, char &oper)
    {
        Request rq(x, y, oper);
        std::string json_rq = rq.Serialize();
        return Encode(json_rq);
    }

    ~Protocol()
    {
    }

private:
    func_t _func;
};