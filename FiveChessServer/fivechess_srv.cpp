#include "common.h"
#include "mydatabase.h"
#include "packetsprocess.h"
#include "plahyagreethread.h"

#include <netinet/in.h>  
#include <sys/socket.h>  
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
  
#include <stdio.h>  
#include <string.h>  
#include <ctime>
#include <pthread.h>

#include <iostream>
#include <string>
  
#include <event2/event.h>  
#include <event2/listener.h>  
#include <event2/bufferevent.h>  
#include <event2/thread.h>  


ServerPack g_serverpack;
PacketType g_packtype;

FiveChessSQL g_sql;

ServerConfig g_conf;

pthread_t g_twoplayId;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mute = PTHREAD_MUTEX_INITIALIZER;


void listener_cb(evconnlistener *listener, evutil_socket_t fd,  
                 struct sockaddr *sock, int socklen, void *arg);
  
void socket_read_cb(bufferevent *bev, void *arg);  
void socket_event_cb(bufferevent *bev, short events, void *arg);  
  
int main()  
{   
    ReadConfigFromFile();

    g_sql.ConnectMySQL(g_conf.server.c_str(),
                       g_conf.username.c_str(), 
                       g_conf.password.c_str(),
                       nullptr, g_conf.mysqlport);
    g_sql.CreateDatabase(g_conf.database.c_str());
    char table[512];
    char *format = "CREATE TABLE IF NOT EXISTS %s(\
                    id INT NOT NULL,\
                    username VARCHAR(32) NOT NULL,\
                    password VARCHAR(32) NOT NULL,\
                    PRIMARY KEY(username));";
    snprintf(table, 512, format, g_conf.tablename.c_str());
    g_sql.CreatedbTable(table);
    g_sql.GetMaxId();

    ListIpAdress();

    std::cout << "Server Runs!\n\n";
    int ret = pthread_create(&g_twoplayId, nullptr, PlayAgreeThread, nullptr);
    if(ret != 0)
    {
        std::cout << " Creating thread failed. Error Code: \n" << strerror(ret);
        exit(-1);
    }

    struct sockaddr_in sin;  
    memset(&sin, 0, sizeof(struct sockaddr_in));  
    sin.sin_family = AF_INET;  
    sin.sin_port = htons(g_conf.serverport);  
  
    event_base *base = event_base_new();  
    evconnlistener *listener = evconnlistener_new_bind(base, listener_cb, base, LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,  
                                                       10, (struct sockaddr*)&sin, sizeof(struct sockaddr_in));  
  
    event_base_dispatch(base);  
  
    evconnlistener_free(listener);  
    event_base_free(base);  
  
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mute);

    return 0;  
}  

//一个新客户端连接上服务器了  
//当此函数被调用时，libevent已经帮我们accept了这个客户端。该客户端的
//文件描述符为fd
void listener_cb(evconnlistener *listener, evutil_socket_t fd,  
                 struct sockaddr *sock, int socklen, void *arg)  
{  
    printf("accept a client %d\n\n", fd);  
  
    event_base *base = (event_base*)arg;  
  
    //为这个客户端分配一个bufferevent  
    bufferevent *bev =  bufferevent_socket_new(base, fd,  
                                               BEV_OPT_CLOSE_ON_FREE);  
  
    bufferevent_setcb(bev, socket_read_cb, NULL, socket_event_cb, NULL);  
    bufferevent_enable(bev, EV_READ | EV_PERSIST);  
}  
  
void HandlePacket(bufferevent *bev, char *arr, int &pos)
{
    char name[32];
    char counter[32];
    char password[32];
    char chatstr[1200];
    char tempstr[1200];
    int x = 0, y = 0;
    int order = 0;
    int color = -1;
    int count = 4;
    WaitPair waitpair;
    GamePair *gamepair = nullptr;
    UserAccount *account = nullptr;
    TwoConnect twoconn;

    int type = GetDword(arr, pos);
    switch (type) {
    case CHAT:
        std::cout << "-----------chat-----------\n";
        GetString(arr, name, pos);
        GetString(arr, counter, pos);
        GetString(arr, chatstr, pos);
        std::cout << "sender: " << name << "\n";
        std::cout << "counter: " << counter << "\n";
        std::cout << "chatstr: " << chatstr<< "\n";

        for(int i = 0; i < g_serverpack.gamepairs.size(); i++)
        {
            if(g_serverpack.gamepairs[i]->black->username == counter)
                account = g_serverpack.gamepairs[i]->black;
            else if(g_serverpack.gamepairs[i]->white->username == counter)
                account = g_serverpack.gamepairs[i]->white;
            if(account)
            {
                PutDword(tempstr, CHAT, count);
                PutString(tempstr, name, count);
                PutString(tempstr, counter, count);
                PutString(tempstr, chatstr, count);
                PutPacketSize(tempstr, count);
                bufferevent_write(account->bev, tempstr, count);
                std::cout << "send msg success.\n";
                return;
            }
        }
        break;

    case LOGON:
        std::cout << "-----------Logon----------\n";
        GetString(arr, name, pos);
        GetString(arr, password, pos);
        std::cout << "name: " << name << "\npassword: " << password << "\n";

        for(int i = 0; i < g_serverpack.totaluser.size(); i++)
        {
            if(g_serverpack.totaluser[i]->username == name)
                goto EXIST;
        }
        if(g_sql.CheckUserfromDB(name, password) == true)
        {
            count = ResponLogon(name, tempstr, true);
            bufferevent_write(bev, tempstr, count);
            AddUserToVec(name, password, bev);
            NoticeLogon(name, strlen(name));
            return;
        }

    EXIST:
        count = ResponLogon(name, tempstr, false);
        bufferevent_write(bev, tempstr, count);
        break;

    case REGIST:
        std::cout << "---------Rigester---------\n";
        GetString(arr, name, pos);
        GetString(arr, password, pos);
        std::cout << "name: " << name << "\npassword: " << password << "\n";
        if(g_sql.IsUserExist(name) == true)
        {
            count = ResponRigester(name, tempstr, false);
            bufferevent_write(bev, tempstr, count);
            return;
        }

        g_sql.InsertUsertoDB(name, password);
        count = ResponRigester(name, tempstr, true);
        bufferevent_write(bev, tempstr, count);
        AddUserToVec(name, password, bev);
        NoticeLogon(name, strlen(name));
        break;

    case POS:
        std::cout << "-----pos-----\n";
        AnlylisPos(bev, arr, pos);
        break;

    case DIRECT_FIGHT:
        std::cout << "-----Directfight-----\n";
        GetString(arr, name, pos);
        std::cout << "sender name: " << name << "\n";
        color = GetDword(arr, pos);
        std::cout << "piece type: " << color << "\n";
        GetString(arr, counter, pos);
        std::cout << "counter name: " << counter << "\n";
        twoconn.color = color;
        twoconn.sender = name;
        twoconn.counter = counter;
        twoconn.bdouble = true;
        pthread_mutex_lock(&mute);
        g_serverpack.twoconnqueue.push(twoconn);
        pthread_mutex_unlock(&mute);
        pthread_cond_signal(&cond);
        break;

    case DIRECT_FIGHT_Y:
    case RANDOM_FIGHT_Y:
        std::cout << "-----PlayWith_y-----\n";
        FightYes(bev, arr, pos, type);
        break;

    case DIRECT_FIGHT_N:
    case RANDOM_FIGHT_N:
        std::cout << "-----Refused-----\n";
        GetString(arr, name, pos);
        std::cout << "counter refuse (name): " << name << "\n";
        PlayWith_n(bev);
        break;

    case RESTART:
        std::cout << "-----restart-----\n";
        GameRestart(bev);
        break;

    case RANDOM_FIGHT:
        std::cout << "-----RandomFight-----\n";
        GetString(arr, name, pos);
        std::cout << "sender name: " << name << "\n";
        color = GetDword(arr, pos);
        std::cout << "piece type: " << color << "\n";
        RandomFight(bev, name, color);
        break;

    case GAMEOUT:
        std::cout << "-----gameout-----\n";
        GetString(arr, name, pos);
        std::cout << "player exit: " << name << "\n";
        GameOut(bev, name);
        break;

    case SEND_COLOR:
        std::cout << "-----sendcolor-----\n";
        color = GetDword(arr, pos);
        std::cout << "Color: " << -color << "\n";
        HandColor(bev, -color, SEND_COLOR);
        break;

    case RECV_COLOR:
        std::cout << "-----recvcolor-----\n";
        color = GetDword(arr, pos);
        std::cout << "Color: " << color << "\n";
        HandColor(bev, color, RECV_COLOR);
        break;

    default:
        break;
    }
}

void socket_read_cb(bufferevent *bev, void *arg)  
{  
    char msg[4096];  
  
    size_t len = bufferevent_read(bev, msg, sizeof(msg));  
    msg[len] = '\0'; 

    int pos = 0;
    int size = GetDword(msg, pos);
    
    std::cout << "Packet Size: " <<  size << " ?== " << len << " )\n"; 
    
    HandlePacket(bev, msg, pos);
}  
  
void socket_event_cb(bufferevent *bev, short events, void *arg)  
{ 
    int ii = -1, pos = 4;
    bool isgaming = true;
    char str[128];
    std::string name, live;
    if (events & BEV_EVENT_EOF)  
        printf("connection closed\n");  
    else if (events & BEV_EVENT_ERROR)
        printf("some other error\n");  

    for(int i = 0; i < g_serverpack.userfree.size(); i++)
    {
        if(g_serverpack.userfree[i]->bev == bev)
        {
            isgaming = false;
            name = g_serverpack.userfree[i]->username;
            g_serverpack.userfree.erase(g_serverpack.userfree.begin() + i);
            break;
        }
    }

    if(isgaming == true)
    {
        int cnt = 0;
        for(int i = 0; i < g_serverpack.gamepairs.size(); i++)
        {
            if(g_serverpack.gamepairs[i]->black->bev == bev)
            {
                name = g_serverpack.gamepairs[i]->black->username;
                live = g_serverpack.gamepairs[i]->white->username;
                g_serverpack.gamepairs[i]->white->isplaying = false;
                g_serverpack.userfree.push_back(g_serverpack.gamepairs[i]->white);
                for(int j = 0; j < g_serverpack.usergaming.size(); j++)
                {
                    if(g_serverpack.usergaming[j] == g_serverpack.gamepairs[i]->black ||
                       g_serverpack.usergaming[j] == g_serverpack.gamepairs[i]->white)
                       {
                           g_serverpack.usergaming.erase(g_serverpack.usergaming.begin() + j);
                           j--;
                           cnt++;
                       }
                    if(cnt == 2)
                        break;
                }
                delete g_serverpack.gamepairs[i];
                g_serverpack.gamepairs.erase(g_serverpack.gamepairs.begin() + i);
                break;
            }
            else if(g_serverpack.gamepairs[i]->white->bev == bev)
            {
                name = g_serverpack.gamepairs[i]->white->username;
                live = g_serverpack.gamepairs[i]->black->username;
                g_serverpack.gamepairs[i]->black->isplaying = false;
                g_serverpack.userfree.push_back(g_serverpack.gamepairs[i]->black);
                for(int j = 0; j < g_serverpack.usergaming.size(); j++)
                {
                    if(g_serverpack.usergaming[j] == g_serverpack.gamepairs[i]->black ||
                       g_serverpack.usergaming[j] == g_serverpack.gamepairs[i]->white)
                       {
                           g_serverpack.usergaming.erase(g_serverpack.usergaming.begin() + j);
                           j--;
                           cnt++;
                       }
                    if(cnt == 2)
                        break;
                }
                delete g_serverpack.gamepairs[i];
                g_serverpack.gamepairs.erase(g_serverpack.gamepairs.begin() + i);
                break;
            }
        }
    }
    

    for(int i = 0; i < g_serverpack.totaluser.size(); i++)
    {
        if(g_serverpack.totaluser[i]->bev == bev)
        {
            delete g_serverpack.totaluser[i];
            g_serverpack.totaluser.erase(g_serverpack.totaluser.begin() + i);
            i--;
        }
        else
        {
            if(isgaming == false)
            {
                PutDword(str, LOGON_OFFLINE, pos);
                PutString(str, name.c_str(), pos);
                PutPacketSize(str, pos);
                bufferevent_write(g_serverpack.totaluser[i]->bev, str, pos);
            }
            else
            {
                PutDword(str, GAME_OFFLINE, pos);
                PutString(str, name.c_str(), pos);
                PutString(str, live.c_str(), pos);
                PutPacketSize(str, pos);
                bufferevent_write(g_serverpack.totaluser[i]->bev, str, pos);
            }
        }
    }
  
    //这将自动close套接字和free读写缓冲区  
    bufferevent_free(bev);  
}