#ifndef PACHETSPROCESS_H_
#define PACHETSPROCESS_H_

#include "common.h"

void NoticeGaming(UserAccount* u1, UserAccount* u2);

void NoticeLogon(char *name, int n);

void NoticeGameOut(UserAccount* u1, UserAccount* u2);

int  ResponLogon(char *name, char *arr, bool bhas);

int  ResponRigester(char *name, char *arr, bool bhas);

void TwoFight(TwoConnect &twoconn);

void RandomFight(bufferevent *bev, char *name, int color);

void FightYes(bufferevent *bev, char *pch, int pos, int type);

void GameOut(bufferevent *bev, char *name);

void PlayWith_n(bufferevent *bev);

void GameRestart(bufferevent *bev);

int  MakePosPacket(char *pac, int type, int order, char *name, int color, Point pnt);

void AnlylisPos(bufferevent *bev, char *arr, int pos);

bool IsWin(int (*chssBoardPtr)[15], int row, int col);

void HandlePacket(bufferevent *bev, char *arr, int &pos);  

void HandColor(bufferevent *bev, int color, PacketType packtype);

#endif //PACHETSPROCESS_H_