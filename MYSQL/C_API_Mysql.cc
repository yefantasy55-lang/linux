#include <iostream>
#include <mysql/mysql.h>
#include <string>

const std::string host = "127.0.0.1";
const std::string user = "yzq";
const std::string passwd = "qwe123";
const std::string db = "test";
const unsigned int port = 3306;

int main()
{
    // printf("MySQL client version: %s\n", mysql_get_client_info());
    MYSQL *my = mysql_init(nullptr);
    if (my == nullptr)
    {
        std::cerr << "Init Mysql error" << std::endl;
    }

    MYSQL *conn = mysql_real_connect(my, host.c_str(), user.c_str(), passwd.c_str(),
                                     db.c_str(), port, nullptr, 0);

    if (conn == nullptr)
    {
        std::cerr << "Connect Mysql error" << std::endl;
    }

    std::string sql;
    std::getline(std::cin, sql);
    int n = mysql_query(my, sql.c_str());

    MYSQL_RES *res = mysql_store_result(my);

    int rows = mysql_num_rows(res);
    int fields = mysql_num_fields(res);

    std::cout << "行：" << rows << std::endl;
    std::cout << "列：" << fields << std::endl;

    MYSQL_FIELD *fields_array = mysql_fetch_field(res);

    // 属性
    for (int i = 0; i < fields; i++)
    {
        std::cout << fields_array[i].name << "\t";
    }
    std::cout << "\n";

    // 内容
    for (int i = 0; i < rows; i++)
    {
        MYSQL_ROW row = mysql_fetch_row(res);  // 会自动记录读取到哪里
        for (int j = 0; j < fields; j++)
        {
            std::cout << row[j] << "\t";
        }
        std::cout << "\n";
    }

    if (n == 0)
    {
        std::cout << sql << " success" << std::endl;
    }
    else
    {
        std::cout << sql << " failed" << std::endl;
    }

    mysql_close(my);
    return 0;
}