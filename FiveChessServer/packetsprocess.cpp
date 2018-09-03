#include "packetsprocess.h"
#include "mydatabase.h"

extern ServerPack g_serverpack;
extern PacketType g_packtype;

extern FiveChessSQL g_sql;

extern pthread_t g_twoplayId;
extern pthread_cond_t  cond;
extern pthread_mutex_t mute;

int ResponLogon(char *name, char *arr, bool bhas)
{
    int pos = 12, t = 4, cnt = 0;
    if(bhas)
    {
        PutDword(arr, LOGNON_Y, t);
        for(int i = 0; i < g_serverpack.totaluser.size(); i++)
        {
            if(g_serverpack.totaluser[i]->username != name)
            {
                PutDword(arr, g_serverpack.totaluser[i]->isplaying, pos);
                PutString(arr, g_serverpack.totaluser[i]->username.c_str(), pos);
                cnt++;
            }
        }
        PutDword(arr, cnt, t);
    }
    else {
        pos = 4;
        PutDword(arr, LOGON_N, pos);
    }
    PutPacketSize(arr, pos);
    return pos;
}

int ResponRigester(char *name, char *arr, bool bhas)
{
    int pos = 12, t = 4, cnt = 0;
    if(bhas)
    {
        PutDword(arr, LOGNON_Y, t);
        for(int i = 0; i < g_serverpack.totaluser.size(); i++)
        {
            if(g_serverpack.totaluser[i]->username != name)
            {
                PutDword(arr, g_serverpack.totaluser[i]->isplaying, pos);
                PutString(arr, g_serverpack.totaluser[i]->username.c_str(), pos);
                cnt++;
            }
        }
        PutDword(arr, cnt, t);
    }
    else {
        pos = 4;
        PutDword(arr, REGIST_N, pos);
    }
    PutPacketSize(arr, pos);
    return pos;
}

void GameOut(bufferevent *bev, char *name)
{
    int pos = 4, cnt = -1;
    char str[64];
    UserAccount *acc = nullptr;
    
    for(int i = 0; i < g_serverpack.gamepairs.size(); i++)
    {
        if(g_serverpack.gamepairs[i]->black->bev == bev)
        {
            acc = g_serverpack.gamepairs[i]->white;
            cnt = i;
            break;
        }
        else if(g_serverpack.gamepairs[i]->white->bev == bev)
        {
            acc = g_serverpack.gamepairs[i]->black;
            cnt = i;
            break;
        }
    }
    if(cnt >= 0 && acc)
    {
        g_serverpack.gamepairs[cnt]->white->isplaying = false;
        g_serverpack.gamepairs[cnt]->black->isplaying = false;
        g_serverpack.userfree.push_back(g_serverpack.gamepairs[cnt]->white);
        g_serverpack.userfree.push_back(g_serverpack.gamepairs[cnt]->black);

        PutDword(str, GAMEOUT, pos);
        PutString(str, name, pos);
        PutPacketSize(str, pos);
        bufferevent_write(acc->bev, str, pos);

        NoticeGameOut(g_serverpack.gamepairs[cnt]->white, g_serverpack.gamepairs[cnt]->black);

        for(int i = 0; i < g_serverpack.usergaming.size(); i++)
        {
            if(g_serverpack.gamepairs[cnt]->white == g_serverpack.usergaming[i])
                g_serverpack.usergaming.erase(g_serverpack.usergaming.begin() + i);
            else if(g_serverpack.gamepairs[cnt]->black == g_serverpack.usergaming[i])
                g_serverpack.usergaming.erase(g_serverpack.usergaming.begin() + i);
        }

        delete g_serverpack.gamepairs[cnt];
        g_serverpack.gamepairs.erase(g_serverpack.gamepairs.begin() + cnt);
    }
}

void FightYes(bufferevent *bev, char *pch, int pos, int type)
{
    char name[32], tstr1[32];
    int cnt = 4;
    GamePair *gamepair = nullptr;
    GetString(pch, name, pos);
    std::cout << "counter name: " << name << "\n";
    for(int i = 0; i < g_serverpack.waitpairs.size(); i++)
    {
        if(g_serverpack.waitpairs[i].white->bev == bev || 
            g_serverpack.waitpairs[i].black->bev == bev)
        {
            g_serverpack.waitpairs[i].white->isplaying = true;
            g_serverpack.waitpairs[i].black->isplaying = true;
            gamepair = new GamePair();
            gamepair->ready = 0;
            gamepair->white = g_serverpack.waitpairs[i].white;
            gamepair->black = g_serverpack.waitpairs[i].black;
            g_serverpack.gamepairs.push_back(gamepair);
            g_serverpack.waitpairs.erase(g_serverpack.waitpairs.begin() + i);
                
            PutDword(tstr1, GAMING, cnt);
            if(type == RANDOM_FIGHT_Y)
                PutString(tstr1, name, cnt);
            PutPacketSize(tstr1, cnt);
            bufferevent_write(gamepair->white->bev, tstr1, cnt);
            bufferevent_write(gamepair->black->bev, tstr1, cnt);
            NoticeGaming(gamepair->white, gamepair->black);

            break;
        }
    }
}

void TwoFight(TwoConnect &twoconn)
{
    char tempstr[128];
    int count = 4;
    WaitPair waitpair;
    UserAccount *acc = nullptr, *cter = nullptr;

    for(int i = 0; i < g_serverpack.userfree.size(); i++)
    {
        if(g_serverpack.userfree[i]->username == twoconn.counter)
        {
            cter = g_serverpack.userfree[i];
            if(twoconn.color == -1)
            {
                waitpair.white =  g_serverpack.userfree[i];
                waitpair.white->color = -twoconn.color;
            }
            else 
            {
                waitpair.black =  g_serverpack.userfree[i];
                waitpair.black->color = -twoconn.color;
            }
            g_serverpack.userfree.erase(g_serverpack.userfree.begin() + i);
            i--;
            continue;
        }
        if(g_serverpack.userfree[i]->username == twoconn.sender)
        {
            if(twoconn.color == -1)
            {
                waitpair.black =  g_serverpack.userfree[i];
                waitpair.black->color = twoconn.color;
            }
            else
            {
                waitpair.white =  g_serverpack.userfree[i];
                waitpair.white->color = twoconn.color;
            }
            acc = g_serverpack.userfree[i];
            g_serverpack.userfree.erase(g_serverpack.userfree.begin() + i);
            i--;
            continue;
        }
    }

    if(waitpair.black && waitpair.white)
    {
        if(twoconn.bdouble)
            PutDword(tempstr, DIRECT_FIGHT, count);
        else
            PutDword(tempstr, RANDOM_FIGHT, count);
        PutString(tempstr, twoconn.sender.c_str(), count);
        PutDword(tempstr, -twoconn.color, count);
        PutPacketSize(tempstr, count);
        bufferevent_write(cter->bev, tempstr, count);
        g_serverpack.waitpairs.push_back(waitpair);
    }
    else
    {
        if(waitpair.black)
            g_serverpack.userfree.push_back(waitpair.black);
        if(waitpair.white)
            g_serverpack.userfree.push_back(waitpair.white);

        if(acc)
        {
            count = 4;
            PutDword(tempstr, COUNTER_0, count);
            PutString(tempstr, "对不起，匹配玩家失败,请稍后重试", count);
            PutPacketSize(tempstr, count);
            bufferevent_write(acc->bev, tempstr, count);
        }
    }
}

void NoticeGaming(UserAccount* u1, UserAccount* u2)
{
    char tempstr[128];
    int count = 4;
    PutDword(tempstr, GAMING_B, count);
    PutString(tempstr, u1->username.c_str(), count);
    PutString(tempstr, u2->username.c_str(), count);
    PutPacketSize(tempstr, count);
    for(int i = 0; i < g_serverpack.totaluser.size(); i++)
        if(g_serverpack.totaluser[i]->bev != u1->bev || g_serverpack.totaluser[i]->bev != u2->bev)
        {
            bufferevent_write(g_serverpack.totaluser[i]->bev, tempstr, count);
        }
}

void NoticeGameOut(UserAccount* u1, UserAccount* u2)
{
    char tempstr[128];
    int count = 4;
    PutDword(tempstr, PLAYER_FREE, count);
    PutString(tempstr, u1->username.c_str(), count);
    PutString(tempstr, u2->username.c_str(), count);
    PutPacketSize(tempstr, count);
    for(int i = 0; i < g_serverpack.totaluser.size(); i++)
        if(g_serverpack.totaluser[i]->bev != u1->bev || g_serverpack.totaluser[i]->bev != u2->bev)
        {
            bufferevent_write(g_serverpack.totaluser[i]->bev, tempstr, count);
        }
}

void NoticeLogon(char *name, int n)
{
    char tempstr[64];
    int  count = 4;
    PutDword(tempstr, LOGON_NOTE, count);
    PutString(tempstr, name, count);
    PutPacketSize(tempstr, count);
    for(int i = 0; i < g_serverpack.totaluser.size(); i++)
        if(g_serverpack.totaluser[i]->username != name)
        {
            bufferevent_write(g_serverpack.totaluser[i]->bev, tempstr, count);
        }
}

void RandomFight(bufferevent *bev, char *name, int color)
{
    int count = 4, tp = 0;
    char str[64];
    TwoConnect twoconn;
    if(g_serverpack.userfree.size() <= 1)
    {
        PutDword(str, COUNTER_0, count);
        PutString(str, "对不起，匹配玩家失败,请稍后重试", count);
        PutPacketSize(str, count);
        bufferevent_write(bev, str, count);
        return;
    }

    srand((unsigned int)time(0));
    tp = rand() % g_serverpack.userfree.size();
    while(g_serverpack.userfree[tp]->username == name)
        tp = rand() % g_serverpack.userfree.size();

    twoconn.color = color;
    twoconn.sender = name;
    twoconn.counter = g_serverpack.userfree[tp]->username;

    pthread_mutex_lock(&mute);
    g_serverpack.twoconnqueue.push(twoconn);
    pthread_mutex_unlock(&mute);
    pthread_cond_signal(&cond);
}

void PlayWith_n(bufferevent *bev)
{
    int pos = 4;
    char str[64];
    UserAccount *acc = nullptr;
    for(int i = 0; i < g_serverpack.waitpairs.size(); i++)
    {
        if(g_serverpack.waitpairs[i].white->bev == bev)
            acc = g_serverpack.waitpairs[i].black;
        else if(g_serverpack.waitpairs[i].black->bev == bev)
            acc = g_serverpack.waitpairs[i].white;
        if(acc)
        {
            g_serverpack.userfree.push_back(g_serverpack.waitpairs[i].black);
            g_serverpack.userfree.push_back(g_serverpack.waitpairs[i].white);
            g_serverpack.waitpairs.erase(g_serverpack.waitpairs.begin() + i);
                
            PutDword(str, DIRECT_FIGHT_N, pos);
            PutPacketSize(str, pos);
            bufferevent_write(acc->bev, str, pos);
            return;
        }
    }
}

void GameRestart(bufferevent *bev)
{
    char pack[16];
    int pos = 4;
    GamePair *pair = nullptr;
    for(int i = 0; i < g_serverpack.gamepairs.size(); i++)
    {
        if(g_serverpack.gamepairs[i]->black->bev == bev)
        {
            if(g_serverpack.gamepairs[i]->ready == 0)
            {
                g_serverpack.gamepairs[i]->ready = 1;
                return;
            }
            else if(g_serverpack.gamepairs[i]->ready == 1)
            {
                g_serverpack.gamepairs[i]->ready = 2;
                pair = g_serverpack.gamepairs[i];
                break;
            }
        }
        else if(g_serverpack.gamepairs[i]->white->bev == bev)
        {
            if(g_serverpack.gamepairs[i]->ready == 0)
            {
                g_serverpack.gamepairs[i]->ready = 1;
                return;
            }
            else if(g_serverpack.gamepairs[i]->ready == 1)
            {
                g_serverpack.gamepairs[i]->ready = 2;
                pair = g_serverpack.gamepairs[i];
                break;
            }
        }
    }

    if(pair && pair->ready == 2)
    {
        memset(pair->chessboard, 0, sizeof(pair->chessboard));
        pair->historyvec.clear();
        pair->order = 0;
        pair->ready = 0;
        PutDword(pack, GAMING, pos);
        PutPacketSize(pack, pos);
        bufferevent_write(pair->black->bev, pack, pos);
        bufferevent_write(pair->white->bev, pack, pos);
    }
}

void AnlylisPos(bufferevent *bev, char *arr, int pos)
{
    int size;
    ChessPos point;
    char name[32], packet[50];
    int order, color, x, y;
    order = GetDword(arr, pos);
    GetString(arr, name, pos);
    color = GetDword(arr, pos);
    x = GetDword(arr, pos);
    y = GetDword(arr, pos);
    std::cout << "\norder: " << order << "\nname: " << name << "\ncolor: " << color << "\n";
    std::cout << "Pos(x, y): (" << x << ", " << y <<")\n";

    for(int i = 0; i < g_serverpack.gamepairs.size(); i++)
    {
        if(color == -1 && g_serverpack.gamepairs[i]->black->bev == bev)
        {
            if(order != g_serverpack.gamepairs[i]->order)
            {
                std::cout << "Cheese point wrong. gamepair.order = " << g_serverpack.gamepairs[i]->order << "\n";
            }
            point.order = order;
            point.color = color;
            point.pos.x = x;
            point.pos.y = y;
            g_serverpack.gamepairs[i]->historyvec.push_back(point);
            g_serverpack.gamepairs[i]->chessboard[x][y] = color;
            if(IsWin(g_serverpack.gamepairs[i]->chessboard, x, y))
            {
                std::cout << "Black is Win.\n";
                int cnt = 4;
                PutDword(packet, WINER, cnt);
                PutString(packet, g_serverpack.gamepairs[i]->black->username.c_str(), cnt);
                PutPacketSize(packet, cnt);
                bufferevent_write(g_serverpack.gamepairs[i]->black->bev, packet, cnt);

                size = MakePosPacket(packet, LOSER, order, name, color, {x, y});
                bufferevent_write(g_serverpack.gamepairs[i]->white->bev, packet, size);
            }
            else
            {
                size = MakePosPacket(packet, POS, order, name, color, {x, y});
                bufferevent_write(g_serverpack.gamepairs[i]->white->bev, packet, size);
                g_serverpack.gamepairs[i]->order ++;
            }
            
            return;
        }
        else if(color == 1 && g_serverpack.gamepairs[i]->white->bev == bev)
        {
            if(order != g_serverpack.gamepairs[i]->order)
            {
                std::cout << "Cheese point wrong. gamepair.order = " << g_serverpack.gamepairs[i]->order << "\n";
            }
            point.order = order;
            point.color = color;
            point.pos.x = x;
            point.pos.y = y;
            g_serverpack.gamepairs[i]->historyvec.push_back(point);
            g_serverpack.gamepairs[i]->chessboard[x][y] = color;
            if(IsWin(g_serverpack.gamepairs[i]->chessboard, x, y))
            {
                std::cout << "White is Win.\n";
                int cnt = 4;
                PutDword(packet, WINER, cnt);
                PutString(packet, g_serverpack.gamepairs[i]->white->username.c_str(), cnt);
                PutPacketSize(packet, cnt);
                bufferevent_write(g_serverpack.gamepairs[i]->white->bev, packet, cnt);

                size = MakePosPacket(packet, LOSER, order, name, color, {x, y});
                bufferevent_write(g_serverpack.gamepairs[i]->black->bev, packet, size);
            }
            else
            {
                size = MakePosPacket(packet, POS, order, name, color, {x, y});
                bufferevent_write(g_serverpack.gamepairs[i]->black->bev, packet, size);
                g_serverpack.gamepairs[i]->order ++;
            }
            return;
        }
    }
}

void HandColor(bufferevent *bev, int color, PacketType packtype)
{
    char str[32];
    int pos = 4, cnt = 0;
    UserAccount *ucc = nullptr, *tempucc = nullptr;
    for(int i = 0; i < g_serverpack.gamepairs.size(); i++)
    {
        if(g_serverpack.gamepairs[i]->black->bev == bev)
        {
            cnt = i;
            ucc = g_serverpack.gamepairs[i]->white;
            break;
        }
        else if(g_serverpack.gamepairs[i]->white->bev == bev)
        {
            cnt = i;
            ucc = g_serverpack.gamepairs[i]->black;
            break;
        }
    }
    if(ucc)
    {
        if(packtype == SEND_COLOR)
        {
            tempucc = g_serverpack.gamepairs[cnt]->black;
            g_serverpack.gamepairs[cnt]->black = g_serverpack.gamepairs[cnt]->white;
            g_serverpack.gamepairs[cnt]->white = tempucc;
        }

        if(packtype == SEND_COLOR)
            PutDword(str, SEND_COLOR, pos);
        else
            PutDword(str, RECV_COLOR, pos);
        PutDword(str, color, pos);
        PutPacketSize(str, pos);
        bufferevent_write(ucc->bev, str, pos);
    }
}

int MakePosPacket(char *pac, int type, int order, char *name, int color, Point pnt)
{
    int pos = 4;
    PutDword(pac, type, pos);
    PutDword(pac, order, pos);
    PutString(pac, name, pos);
    PutDword(pac, color, pos);
    PutDword(pac, pnt.x, pos);
    PutDword(pac, pnt.y, pos);
    PutPacketSize(pac, pos);
    return pos;
}

bool IsWin(int (*chssBoardPtr)[15], int row, int col)
{
    int rw = 0, cl = 0;
    // k = 0
    for (int i = 0; i < 5; i++)
    {
        cl = col - i;
        if (cl >= 0 && cl + 4 < 15 &&
            chssBoardPtr[row][cl] == chssBoardPtr[row][cl + 1] &&
            chssBoardPtr[row][cl] == chssBoardPtr[row][cl + 2] &&
            chssBoardPtr[row][cl] == chssBoardPtr[row][cl + 3] &&
            chssBoardPtr[row][cl] == chssBoardPtr[row][cl + 4])
            return true;
    }

    // k = 90
    for (int i = 0; i < 5; i++)
    {
        rw = row - i;
        if (rw >= 0 && rw + 4 < 15 &&
            chssBoardPtr[rw][col] == chssBoardPtr[rw + 1][col] &&
            chssBoardPtr[rw][col] == chssBoardPtr[rw + 2][col] &&
            chssBoardPtr[rw][col] == chssBoardPtr[rw + 3][col] &&
            chssBoardPtr[rw][col] == chssBoardPtr[rw + 4][col])
            return true;
    }

    // k = 45
    for (int i = 0; i < 5; i++)
    {
        rw = row + i;
        cl = col - i;
        if (rw < 15 && rw - 4 >= 0 &&
            cl >= 0 && cl + 4 < 15 &&
            chssBoardPtr[rw][cl] == chssBoardPtr[rw - 1][cl + 1] &&
            chssBoardPtr[rw][cl] == chssBoardPtr[rw - 2][cl + 2] &&
            chssBoardPtr[rw][cl] == chssBoardPtr[rw - 3][cl + 3] &&
            chssBoardPtr[rw][cl] == chssBoardPtr[rw - 4][cl + 4])
            return true;
    }

    // k = 135
    for (int i = 0; i < 5; i++)
    {
        rw = row - i;
        cl = col - i;
        if (rw >= 0 && rw + 4 < 15 &&
            cl >= 0 && cl + 4 < 15 &&
            chssBoardPtr[rw][cl] == chssBoardPtr[rw + 1][cl + 1] &&
            chssBoardPtr[rw][cl] == chssBoardPtr[rw + 2][cl + 2] &&
            chssBoardPtr[rw][cl] == chssBoardPtr[rw + 3][cl + 3] &&
            chssBoardPtr[rw][cl] == chssBoardPtr[rw + 4][cl + 4])
            return true;
    }
    return false;
}