#pragma once
#include "Socket.hpp"
#include <iostream>
#include <memory>
#include <string>
#include "TcpServer.hpp"
#include <unordered_map>
#include "Log.hpp"
#include <sstream>
#include "Util.hpp"
#include <functional>

using namespace yzq;

const std::string gspace = " ";
const std::string glinespace = "\r\n"; // 行分隔符
const std::string glinesep = ": ";

const std::string webroot = "./wwwroot";
const std::string homepage = "index.html";
const std::string page_404 = "/404.html";

class HttpRequest
{
public:
    HttpRequest()
        : _blankline(glinespace), _is_interact(false)
    {
    }

    ~HttpRequest()
    {
    }
    // 解析请求行
    void ParseReqLine(std::string &reqline)
    {
        // GET /index.html HTTP/1.1
        std::stringstream ss(reqline);
        ss >> _method >> _uri >> _version;
    }

    std::string Serialize()
    {
        return std::string();
    }

    bool Deserialize(std::string &reqstr)
    {
        // GET /index.html HTTP/1.1
        std::string reqline;
        Util::ReadOneLine(reqstr, &reqline, gsep); // 读取请求行
        LOG(LogLevel::DEBUG) << reqline;

        // 反序列化
        ParseReqLine(reqline);
        if (_uri == "/")
        {
            _uri = webroot + _uri + homepage; // ./wwwroot/index.html
        }
        else
        {
            _uri = webroot + _uri; // ./wwwroot/a/b/c.html
        }

        // _uri: ./wwwroot/login?username=zhangsan&password=123456(GET提交)
        LOG(LogLevel::DEBUG) << "_method: " << _method;
        LOG(LogLevel::DEBUG) << "_uri: " << _uri;
        LOG(LogLevel::DEBUG) << "_version: " << _version;
        const std::string arg_sep = "?";
        auto pos = _uri.find(arg_sep);
        if (pos == std::string::npos) // 没有参数
        {
            return true;
        }
        else
        {
            _is_interact = true;
            _args = _uri.substr(pos + arg_sep.size());
            _uri = _uri.substr(0, pos);
            return true;
        }
    }

    std::string Uri()
    {
        return _uri;
    }

    std::string Args()
    {
        return _args;
    }

    bool isInteract()
    {
        return _is_interact;
    }

private:
    std::string _method;  // 请求方法
    std::string _uri;     // 资源路径
    std::string _version; // 版本号

    std::unordered_map<std::string, std::string> _headers; // 请求头
    std::string _blankline;                                // 空行
    std::string _text;                                     // 载荷（正文）

    std::string _args; // 用户提交的参数
    bool _is_interact; // 是否交互
};

class HttpResponse
{
public:
    HttpResponse()
        : _blankline(glinespace), _version("HTTP/1.1")
    {
    }

    ~HttpResponse()
    {
    }

    // 设置访问资源路径
    void SetTargetFile(const std::string &target)
    {
        _targetfile = target;
    }

    // 设置状态码
    void SetCode(int code)
    {
        _status_code = code;
        switch (_status_code)
        {
        case 200:
            _desc = "OK";
            break;

        case 301: // 永久重定向
            _desc = "Moved Permanently";

        case 302: // 临时重定向
            _desc = _desc = "See Other";

        case 404:
            _desc = "Not Found";
            break;

        default:
            break;
        }
    }

    // 设置正文
    void SetText(const std::string &text)
    {
        _text = text;
    }

    // 根据响应内容匹配响应类型
    std::string Uri2Suffix(const std::string &targetfile)
    {
        // ./wwwroot/index.html
        auto pos = targetfile.rfind(".");
        if (pos == std::string::npos)
        {
            return "text/html";
        }
        std::string suffix = targetfile.substr(pos);
        if (suffix == ".html" || suffix == ".htm")
            return "text/html";
        else if (suffix == ".jpg")
            return "image/jpeg";
        else if (suffix == ".png")
            return "image/png";
        else
            return "";
    }

    // 响应头
    void SetHeader(const std::string &key, const std::string &value)
    {
        auto pos = _headers.find(key);
        if (pos != _headers.end()) // 有则返回，无则插入
        {
            return;
        }
        _headers.insert(std::make_pair(key, value));
    }

    // 响应体
    bool MakeResponse()
    {
        if (_targetfile == "./wwwroot/favicon.ico") // 忽略图标请求
        {
            LOG(LogLevel::DEBUG) << "用户请求: " << _targetfile << "忽略它";
            return false;
        }
        bool res = Util::ReadFileContent(_targetfile, &_text);
        int filesize = 0; // 正文长度
        if (!res)
        {
            // version1(手动重定向)
            _text.clear();
            LOG(LogLevel::WARNING) << "client want get : " << _targetfile << " but not found";
            SetCode(404);
            _targetfile = webroot + page_404; // 重定向到404界面
            Util::ReadFileContent(_targetfile, &_text);
            std::string suffix = Uri2Suffix(_targetfile);
            filesize = Util::FileSize(_targetfile);
            SetHeader("Content-Type", suffix);
            SetHeader("Content-Lengt", std::to_string(filesize));

            // version2(利用状态码让浏览器自动重定向)
            // SetCode(301);
            // SetHeader("Location","http://123456:8080/404.html");
        }
        else
        {
            LOG(LogLevel::DEBUG) << "读取文件: " << _targetfile;
            SetCode(200);
            std::string suffix = Uri2Suffix(_targetfile);
            filesize = Util::FileSize(_targetfile);
            SetHeader("Content-Type", suffix);
            SetHeader("Content-Lengt", std::to_string(filesize));
        }
        return true;
    }

    std::string Serialize()
    {
        // 响应行
        std::string status_line = _version + gspace + std::to_string(_status_code) + gspace + _desc + glinespace;

        // 响应头
        std::string resp_header;
        for (auto &header : _headers)
        {
            resp_header += header.first + glinesep + header.second + glinespace;
        }

        return status_line + resp_header + _blankline + _text;
    }

public:
    std::string _version;
    int _status_code;  // 404
    std::string _desc; // "Not Found"

    std::unordered_map<std::string, std::string> _headers;
    std::string _blankline;
    std::string _text;

    // 其他属性
    std::string _targetfile;
};

using http_fun_t = std::function<void(HttpRequest &req, HttpResponse &resp)>; // 接口函数

class Http
{
public:
    Http(uint16_t port)
        : _tsvrp(std::make_unique<TcpServer>(port))
    {
    }

    // 注册方法
    void RegisterService(const std::string name, http_fun_t service)
    {
        std::string key = webroot + name;
        if (_route.find(key) != _route.end()) // 注册过直接返回
            return;
        _route.insert(std::make_pair(key, service));
    }

    void HandlerHttpRquest(std::shared_ptr<Socket> &sock, InetAddr &client)
    {
        // 接收请求
        std::string msg;
        int n = sock->Recv(&msg);

        if (n > 0)
        {
            // 解析请求
            HttpRequest req;
            req.Deserialize(msg);

            // 处理请求
            HttpResponse resp;
            if (req.isInteract())
            {
                if (_route.find(req.Uri()) == _route.end()) // 没有注册该方法重定向到404
                {
                    // TODO
                }
                else
                {
                    _route[req.Uri()](req, resp); // 调用注册的方法
                    std::string response_str = resp.Serialize();
                    sock->Send(response_str);
                }
            }
            else
            {
                resp.SetTargetFile(req.Uri());
                if (resp.MakeResponse())
                {
                    std::string response_str = resp.Serialize();
                    sock->Send(response_str);
                }
            }
        }
    }

    void Start()
    {
        _tsvrp->Start([this](std::shared_ptr<Socket> &sock, InetAddr &client) -> void
                      { HandlerHttpRquest(sock, client); });
    }

    ~Http()
    {
    }

private:
    std::unique_ptr<TcpServer> _tsvrp;
    std::unordered_map<std::string, http_fun_t> _route; // RESTful风格的接口函数
};