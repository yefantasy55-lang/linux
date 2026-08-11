#include "Http.hpp"

void Login(HttpRequest &req, HttpResponse &resp)
{
    LOG(LogLevel::DEBUG) << req.Args() << ", 我们成功进入到了处理数据的逻辑";
    std::string text = "hello: " + req.Args();

    // 登录认证

    resp.SetCode(200);
    resp.SetHeader("Content-Type", "text/plain");
    resp.SetHeader("Content-Length", std::to_string(text.size()));
    resp.SetText(text);
}
void Register(HttpRequest &req, HttpResponse &resp)
{
    LOG(LogLevel::DEBUG) << req.Args() << ", 我们成功进入到了处理数据的逻辑";
    std::string text = "hello: " + req.Args();

    resp.SetCode(200);
    resp.SetHeader("Content-Type", "text/plain");
    resp.SetHeader("Content-Length", std::to_string(text.size()));
    resp.SetText(text);
}
void VipCheck(HttpRequest &req, HttpResponse &resp)
{
    LOG(LogLevel::DEBUG) << req.Args() << ", 我们成功进入到了处理数据的逻辑";
    std::string text = "hello: " + req.Args();
    resp.SetCode(200);
    resp.SetHeader("Content-Type", "text/plain");
    resp.SetHeader("Content-Length", std::to_string(text.size()));
    resp.SetText(text);
}

void Search(HttpRequest &req, HttpResponse &resp)
{
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " port" << std::endl;
        exit(USAGE_ERR);
    }
    uint16_t port = std::stoi(argv[1]);

    std::unique_ptr<Http> httpsvr = std::make_unique<Http>(port);

    httpsvr->RegisterService("/login", Login);
    httpsvr->RegisterService("/register", Register);
    httpsvr->RegisterService("/vip_check", VipCheck);
    httpsvr->RegisterService("/s", Search);

    httpsvr->Start();

    return 0;
}