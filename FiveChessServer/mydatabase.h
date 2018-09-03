#ifndef _MYDATABASE_H_
#define _MYDATABASE_H_

#include <mysql/mysql.h>
#include <iostream>
#include <string>
#include <vector>
#include <string>

class FiveChessSQL
{

public: 
    FiveChessSQL();
    virtual ~FiveChessSQL();
 
    bool ConnectMySQL(const char* server, const char* username, const char* password, const char* database, int port);
    bool CreateDatabase(std::string dbname);
    bool CreatedbTable(const std::string query);
 
    std::string InsertUserQuery(std::string name, std::string password);
    bool InsertUsertoDB(std::string name, std::string password);
    bool InsertRowtoDB(std::string query);

    bool IsUserExist(std::string name);
    std::string LookforUserQuery(std::string name, std::string password);
    bool CheckUserfromDB(std::string name, std::string password);
    bool GetRowfromDB(std::string query, std::vector<std::string>& user);
    void CloseMySQL();
    
    int GetMaxId();

private:
    void ErrorIntoMySQL();            
 
private:
    int m_errorNum;                      
    const char* m_pErrorInfo; 

    int m_id;
    MYSQL *m_pSql;                      //MySQL对象，必备的一个数据结构
    MYSQL_RES *m_pSqlResult;            //用于存放结果 建议用char* 数组将此结果转存
};

#endif