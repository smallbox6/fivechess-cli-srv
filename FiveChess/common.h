#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <QByteArray>

void PutByte(char *pPacket, char value, int &nPos);
char GetByte(char *pPacket, int &nPos);

void  PutWord(char *pPacket, short value, int &nPos);
short GetWord(char *pPacket, int &nPos);

void PutDword(char *pPacket, int value, int &nPos);
int  GetDword(char *pPacket, int &nPos);

void PutString(char *pPacket, char *pBuffer, int &nPos);
void GetString(char *pPacket, char *pBuffer, int &nPos);

void PutPacketSize(char *pPacket, int nPos);

int RegisterPacket(char *name, char *password, char *pac);

int LogonPacket(char *name, char *password, char *pac);

int ChatMsgPacket(char *name, char *msg, char *pac);

struct OnlinePlayer
{
    OnlinePlayer()
    {
        isplaying = true;
    }
    QByteArray name;
    bool isplaying;
};

// 游戏类型，双人还是AI（目前固定让AI下黑子）
enum GameType
{
    HUMAN,
    COMPUTER
};

// 游戏状态
enum GameStatus
{
    NONE,
    READY,
    PLAYING,
    WIN,
    LOSE,
    DEAD
};

enum TipType
{
    NONEIMG,
    READYIMG,
    WINIMG,
    LOSEIMG
};

#endif // COMMON_H
