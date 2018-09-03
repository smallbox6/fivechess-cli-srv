#include "common.h"

#include <string.h>
#include <stdlib.h>

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

void PutString(char *pPacket, char *pBuffer, int &nPos)
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

int RegisterPacket(char *name, char *password, char *pac)
{
    int pos = 4;
    PutString(pac, "Register", pos);
    PutString(pac, name, pos);
    PutString(pac, password, pos);
    PutPacketSize(pac, pos);
    return pos;
}

int LogonPacket(char *name, char *password, char *pac)
{
    int pos = 4;
    PutString(pac, "Logon", pos);
    PutString(pac, name, pos);
    PutString(pac, password, pos);
    PutPacketSize(pac, pos);
    return pos;
}

int ChatMsgPacket(char *name, char *msg, char *pac)
{
    int pos = 4;
    PutString(pac, "Msg", pos);
    PutString(pac, name, pos);
    PutString(pac, msg, pos);
    PutPacketSize(pac, pos);
    return pos;
}
