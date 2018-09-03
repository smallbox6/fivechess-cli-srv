#include "mydatabase.h"

#include <stdlib.h>

FiveChessSQL::FiveChessSQL():
    m_errorNum(0), 
    m_pErrorInfo("Ok"),
    m_id(0)
{
    m_pSql = mysql_init(nullptr);
    if(m_pSql == nullptr)
        ErrorIntoMySQL();
}

FiveChessSQL::~FiveChessSQL()
{
    CloseMySQL();
}
 
//连接MySQL
bool FiveChessSQL::ConnectMySQL(const char* server, const char* username, const char* password, const char* database, int port)
{
    if(mysql_real_connect(m_pSql, server, username, password, database, port, 0, 0) != NULL)
    {
        std::cout << "\nConnected to MySQL successful! \n\n";
        std::cout << "MySQL Info: " << mysql_get_server_info(m_pSql) << "\n\n";
        std::cout << "Host_info: " << mysql_get_host_info(m_pSql) << "\n\n";
        return true;
    }
    else
        ErrorIntoMySQL();
    return false;
}

//判断数据库是否存在，不存在则创建数据库，并打开
bool FiveChessSQL::CreateDatabase(std::string dbname)
{
    std::string queryStr = "create database if not exists ";
    queryStr += dbname;
    if (0 == mysql_query(m_pSql, queryStr.c_str()))
    {
        queryStr = "use ";
        queryStr += dbname;
        if (0 == mysql_query(m_pSql, queryStr.c_str()))
        {
            return true;
        }
    }
    ErrorIntoMySQL();
    return false;
}

//判断数据库中是否存在相应表，不存在则创建表
bool FiveChessSQL::CreatedbTable(const std::string query)
{
    if (0 == mysql_query(m_pSql, query.c_str()))
    {
        return true;
    }
    ErrorIntoMySQL();
    return false;
}

std::string FiveChessSQL::LookforUserQuery(std::string name, std::string password)
{
    char str[512];
    char *format = "select username, password from user_table where username = '%s' and password = '%s'";
    snprintf(str, 512, format, name.c_str(), password.c_str());
    return str;
}
 
std::string FiveChessSQL::InsertUserQuery(std::string name, std::string password)
{
    char str[512];
    char *format = "insert user_table(id, username, password) values(%d, '%s', '%s')";
    snprintf(str, 512, format, m_id, name.c_str(), password.c_str());
    m_id++;
    return str;
}

int FiveChessSQL::GetMaxId()
{
    std::string query("select max(id) as max_id from user_table");
    if(0 != mysql_query(m_pSql, query.c_str()))
    {
        ErrorIntoMySQL();
        std::cout << "there is no rows selcted\n";
        return -1;
    }

    m_pSqlResult = mysql_store_result(m_pSql);
    int row = mysql_num_rows(m_pSqlResult);
    MYSQL_ROW line = mysql_fetch_row(m_pSqlResult);
    int field = mysql_num_fields(m_pSqlResult);

    if(row <= 0 || field <= 0)
    {
        m_id = 1;
        return  m_id;
    }

    m_id = atoi(line[field - 1]) + 1;

    return  m_id;  
}

//写入数据
bool FiveChessSQL::InsertRowtoDB(std::string query)
{
    if(0 == mysql_query(m_pSql, query.c_str()))
        return true;
    else
        ErrorIntoMySQL();
    return false;  
}

bool FiveChessSQL::InsertUsertoDB(std::string name, std::string password)
{
    std::string query = InsertUserQuery(name, password);
    if( true == InsertRowtoDB(query))
    {
        std::cout << "insert user success\n\n";
        return true;
    }
    else
    {
        std::cout << "insert user failed\n\n";
        return false;
    }
}

bool FiveChessSQL::CheckUserfromDB(std::string name, std::string password)
{
    std::string query;
    query = LookforUserQuery(name, password);

    std::vector<std::string> user;

    if(GetRowfromDB(query, user) == true)
    {
        if(user.size() == 2 && user[0] == name && user[1] == password)
        {
            std::cout << "user exist.\n\n";
            return true;
        }
        else{
            std::cout << "user does not exist.\n\n";
        return false;
        }
    }
}

bool FiveChessSQL::IsUserExist(std::string name)
{
    char str[512];
    char *format = "select username from user_table where username = '%s'";
    snprintf(str, 512, format, name.c_str());

    if(0 != mysql_query(m_pSql, str))
    {
        ErrorIntoMySQL();
        return false;
    }
 
    m_pSqlResult = mysql_store_result(m_pSql);
 
    int row = mysql_num_rows(m_pSqlResult);
    if(row <= 0)
        return false;
    return true;
}

//读取数据
bool FiveChessSQL::GetRowfromDB(std::string query, std::vector<std::string>& user)
{
    if(0 != mysql_query(m_pSql, query.c_str()))
    {
        ErrorIntoMySQL();
        return false;
    }
 
    m_pSqlResult = mysql_store_result(m_pSql);
 
    int row = mysql_num_rows(m_pSqlResult);
    int field = mysql_num_fields(m_pSqlResult);
 
    MYSQL_ROW line = NULL;
    line = mysql_fetch_row(m_pSqlResult);
 
    int j = 0;
    std::string temp;
    while(NULL != line)
    {  
        std::cout << "<row>: (";
        for(int i = 0; i < field; i++)
        {
            if(line[i])
            {
                temp = line[i];
                user.push_back(temp);
                std::cout << temp;
                if(i < field - 1)
                    std::cout << ",  ";
            }
        }
        line = mysql_fetch_row(m_pSqlResult);
        std::cout << ")" <<std::endl;
    }
    return true;
}
 
//错误信息
void FiveChessSQL::ErrorIntoMySQL()
{
    m_errorNum = mysql_errno(m_pSql);
    m_pErrorInfo = mysql_error(m_pSql);
    std::cout << "<FiveChessSQL Error>: errorNum: " << m_errorNum << ", errorInfo: " << m_pErrorInfo << std::endl;
}
 
//断开连接
void FiveChessSQL::CloseMySQL()
{
    mysql_close(m_pSql);
}