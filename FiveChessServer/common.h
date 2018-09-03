#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <vector>
#include <queue>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <event2/bufferevent.h>  

void  ListIpAdress();

void  PutByte(char *pPacket, char value, int &nPos);
char  GetByte(char *pPacket, int &nPos);

void  PutWord(char *pPacket, short value, int &nPos);
short GetWord(char *pPacket, int &nPos);

void  PutDword(char *pPacket, int value, int &nPos);
int   GetDword(char *pPacket, int &nPos);

void  PutString(char *pPacket, const char *pBuffer, int &nPos);
void  GetString(char *pPacket, char *pBuffer, int &nPos);

void  PutPacketSize(char *pPacket, int nPos);

void AddUserToVec(char *name, char *password, bufferevent *bev);

void ReadConfigFromFile();

enum PacketType{
    CHAT = 1,
    LOGON,
    LOGNON_Y,
    LOGON_N,
    LOGON_NOTE,
    REGIST,
    REGIST_Y,
    REGIST_N,
    POS,
    ALIVE,
    DIRECT_FIGHT,
    DIRECT_FIGHT_Y,
    DIRECT_FIGHT_N,
    COUNTER_0,
    RESTART,
    RANDOM_FIGHT,
    RANDOM_FIGHT_Y,
    RANDOM_FIGHT_N,
    GAMING,
    GAMING_B,
    SEND_COLOR,
    RECV_COLOR,
    WINER,
    LOSER,
    GAMEOUT,
    PLAYER_FREE,
    LOGON_OFFLINE,
    GAME_OFFLINE
};

struct ServerConfig{
    //server
    int serverport;
    //mysql
    std::string server;
    std::string username;
    std::string password;
    std::string database;
    std::string tablename;
    int mysqlport;
};

struct Point
{
    int x;
    int y;
};

struct ChessPos
{
    int order;
    int color;
    Point pos;
};

struct UserAccount
{
    UserAccount(){
        color = 1;
        isplaying = false;
        bev = nullptr;
    }

    bufferevent *bev;
    std::string username;
    std::string password;
    int  color;
    bool  isplaying;
};

struct GamePair
{
    GamePair()
    {
        order = 0;
        ready = 0;
        black = nullptr;
        white = nullptr;
        memset(chessboard, 0, sizeof(chessboard));
    }
    UserAccount *black;
    UserAccount *white;
    char ready;
    int order;
    int chessboard[15][15];
    std::vector<ChessPos> historyvec;
};

struct WaitPair
{
    WaitPair()
    {
        black = nullptr;
        white = nullptr;
    }
    UserAccount *black;
    UserAccount *white;
};

struct TwoConnect
{
    TwoConnect(){
        color = -1;
        bdouble = false;
    }
    int color;
    bool bdouble;
    std::string sender;
    std::string counter;
};


struct ServerPack
{
    std::vector<UserAccount*> userfree; 
    std::vector<UserAccount*> usergaming;
    std::vector<UserAccount*> totaluser;
    //std::vector<UserAccount*> waituser;
    std::vector<GamePair*> gamepairs;
    std::vector<WaitPair> waitpairs;
    std::queue<TwoConnect> twoconnqueue;
};

#endif