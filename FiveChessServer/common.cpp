#include "common.h"

#include <string.h>
#include <iostream>
#include <fstream>
  
#include <sys/socket.h>   
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>

extern ServerPack g_serverpack;

extern ServerConfig g_conf;

void PutByte(char *pPacket, char value, int &nPos)
{
    *(char*)(pPacket + nPos) = value;
    nPos = nPos + sizeof(char);
}

char GetByte(char *pPacket, int &nPos)
{
    char value = *(char*)(pPacket + nPos);
    nPos = nPos + sizeof(char);
    return value;
}

void PutWord(char *pPacket, short value, int &nPos)
{
    *(short*)(pPacket + nPos) = value;
    nPos = nPos + sizeof(short);
}

short GetWord(char *pPacket, int &nPos)
{
    short value = *(short*)(pPacket + nPos);
    nPos = nPos + sizeof(short);
    return value;
}

void PutDword(char *pPacket, int value, int &nPos)
{
    *(int*)(pPacket + nPos) = value;
    nPos = nPos + sizeof(int);
}

int GetDword(char *pPacket, int &nPos)
{
    int value = *(int*)(pPacket + nPos);
    nPos = nPos + sizeof(int);
    return value;
}

void PutString(char *pPacket, const char *pBuffer, int &nPos)
{
    *(int*)(pPacket + nPos) = strlen(pBuffer);
    nPos = nPos + sizeof(int);
    memcpy(pPacket + nPos, pBuffer, strlen(pBuffer));
    nPos = nPos + strlen(pBuffer);
}

void GetString(char *pPacket, char *pBuffer, int &nPos)
{
    int bufferLen = 0;
    bufferLen = *(int*) (pPacket + nPos);
    nPos = nPos + sizeof(int);
    memcpy(pBuffer, pPacket + nPos, bufferLen);
    *(pBuffer + bufferLen) = '\0';
    nPos = nPos + bufferLen;
}

void PutPacketSize(char *pPacket, int nPos)
{
    *(int *) pPacket = nPos;
}

void AddUserToVec(char *name, char *password, bufferevent *bev)
{
    UserAccount *account = new UserAccount();
    account->username = name;
    account->password = password;
    account->bev = bev;
    g_serverpack.userfree.push_back(account);
    g_serverpack.totaluser.push_back(account);
}

void ListIpAdress()
{
    int sockfd;
    struct ifconf ifconf;
    struct ifreq *ifreq;
    char buf[512];
  
    ifconf.ifc_len = 512;
    ifconf.ifc_buf = buf;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket\n" );
        exit(1);
    }

    ioctl(sockfd, SIOCGIFCONF, &ifconf); //获取所有接口信息

    //获取IP地址
    ifreq = (struct ifreq*)ifconf.ifc_buf;
    std::cout << "-----------------local address----------------\n";

    std::cout << "Get < " << ifconf.ifc_len / sizeof (struct ifreq) << " > ip address.\n";
    for (int i = (ifconf.ifc_len / sizeof (struct ifreq)); i > 0; i--)
    {
        if(ifreq->ifr_flags == AF_INET){ //ipv4
            std::cout << "eth name : " << ifreq->ifr_name << "\n";
            std::cout << "local addr = " << inet_ntoa( ((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr) << "\n";
            ifreq++;
        }
    }
    std::cout << "-------------------------------------------end\n\n";
}

void ReadConfigFromFile()
{
    char temp[128], *pch1 = nullptr, *pch2 = nullptr;
    std::string str;
    std::fstream infile("./config.cfg");
    if(infile.is_open())
    {
        infile.getline(temp, 128);
        while(!infile.eof())
        {
            if(temp[0] == '#')
            {
                infile.getline(temp, 128);
                continue;
            }
            pch1 = strtok(temp, ":");
            pch2 = strtok(nullptr, ":");
            if(pch1 == std::string("serverport"))
            {
                g_conf.serverport = atoi(pch2);
            }
            else if(pch1 == std::string("server"))
            {
                g_conf.server = pch2;
            }
            else if(pch1 == std::string("username"))
            {
                g_conf.username = pch2;
            }
            else if(pch1 == std::string("password"))
            {
                g_conf.password = pch2;
            }
            else if(pch1 == std::string("database"))
            {
                g_conf.database = pch2;
            }
            else if(pch1 == std::string("tablename"))
            {
                g_conf.tablename = pch2;
            }
            else if(pch1 == std::string("mysqlport"))
            {
                g_conf.mysqlport = atoi(pch2);
            }
            infile.getline(temp, 128);
        }

        std::cout << "---------------------config--------------------\n";
        std::cout << "serverport: " << g_conf.serverport << "\n";
        std::cout << "server: " << g_conf.server << "\n";
        std::cout << "username: " << g_conf.username << "\n";
        std::cout << "password: " << g_conf.password << "\n";
        std::cout << "database: " << g_conf.database << "\n";
        std::cout << "tablename: " << g_conf.tablename << "\n";
        std::cout << "mysqlport: " << g_conf.mysqlport << "\n";
        std::cout << "-------------------------------------------end\n";
    }
    else
    {
        infile.close();
        std::cout << "Open file: config.cfg failed\n\n";
        exit(-3);
    }
    infile.close();
}