#include "packetprocessor.h"
#include "network.h"
#include "common.h"
#include "logondialog.h"
#include "gamemodel.h"

#include <QSound>

static char countname[64];
static int  piececolor = -1;

PacketProcessor::PacketProcessor(QObject *parent):
    QObject(parent),
    m_bSendColor(false)
{
    m_chatWig = nullptr;
    m_playlistWig = nullptr;
    m_sidebarWig = nullptr;
    m_mainWig = nullptr;
}

PacketProcessor* PacketProcessor::GetPacketProcessor()
{
    static PacketProcessor *processor;
    if(processor == nullptr)
        processor = new PacketProcessor();
    else
        return processor;
}

void PacketProcessor::SetPieceColor(int ncolor)
{
    if(ncolor == m_account.ncolor || m_bSendColor == true)
        return;
    m_account.ncolor = ncolor;
    m_pNoticeidget->SetPiecePixmap(ncolor);
    if(m_account.bwithfriend == true && m_account.gamestatus != PLAYING)
    {
        char str[16];
        int pos = 4;
        PutDword(str, SEND_COLOR, pos);
        PutDword(str, ncolor, pos);
        PutPacketSize(str, pos);
        NetWork::GetNetWork()->WriteData(str, pos);
        m_bSendColor = true;
        qDebug() << "SetPieceColor(int ncolor)";
    }
}

void PacketProcessor::HandlePacket(QByteArray arr)
{
    int count = 0, size = 0, isplaying = 0;
    char tstr1[1024], tstr2[1024];
    OnlinePlayer onplayer;

    char *pch = arr.data();
    int packsize = GetDword(pch, count);

    int type = GetDword(pch, count);
    switch (type) {
    case CHAT:
        GetString(pch, tstr1, count);
        GetString(pch, tstr1, count);
        GetString(pch, tstr1, count);
        m_chatWig->SetMsg(tstr1, false);
        break;

    case LOGNON_Y:
        LogOnDialog::getLogOnDialog()->LogonSucces();
        m_topWig->LogOn(true);
        size = GetDword(pch, count);
        for(int i = 0; i < size; i++)
        {
            isplaying = GetDword(pch, count);
            GetString(pch, tstr1, count);
            onplayer.isplaying = isplaying;
            onplayer.name = tstr1;
            AddPlayer(onplayer);
        }
        onplayer.isplaying = false;
        onplayer.name = m_account.username;
        m_account.playervec.push_back(onplayer);
        m_account.islogon = true;
        m_playlistWig->UpdateItems(m_account.playervec);
        m_pNoticeidget->AddInfo(m_account.username + QStringLiteral("： 恭喜你登录成功！"));
        break;

    case LOGON_N:
        m_pNoticeidget->AddInfo(QStringLiteral("当前用户名尚未注册，请注册！"));
        m_account.username.clear();
        m_account.password.clear();
        break;

    case LOGON_NOTE:
        GetString(pch, tstr1, count);
        onplayer.isplaying = false;
        onplayer.name = tstr1;
        m_account.playervec.push_back(onplayer);
        m_playlistWig->UpdateItems(m_account.playervec);
        m_pNoticeidget->AddInfo(tstr1 + QStringLiteral(", 已上线！"));
        break;

    case REGIST_N:
        m_pNoticeidget->AddInfo(QStringLiteral("当前用户已存在!"));
        m_account.username.clear();
        m_account.password.clear();
        break;

    case POS:
        RecvPos(pch, count);
        break;

    case DIRECT_FIGHT:
        GetString(pch, countname, count);
        piececolor = GetDword(pch, count);
        m_account.fighttype = DIRECT_FIGHT;
        m_popDlg->ShowWidget(countname, 2);
        m_pNoticeidget->AddInfo(countname + QStringLiteral(": 邀请你一起游戏！"));
        break;

    case DIRECT_FIGHT_N:
        m_playlistWig->SetMatchValue(false);
        m_pNoticeidget->AddInfo(QStringLiteral("玩家拒绝了你的邀请，请重新匹配！"));
        m_popDlg->ShowWidget(tstr1, 0);
        break;

    case COUNTER_0:
        m_playlistWig->SetMatchValue(false);
        GetString(pch, tstr1, count);
        m_popDlg->ShowWidget(countname, -1);
        m_pNoticeidget->AddInfo(QStringLiteral("玩家们正在游戏，请稍后重试！"));
        break;

    case RANDOM_FIGHT:
        GetString(pch, countname, count);
        piececolor = GetDword(pch, count);
        m_account.fighttype = RANDOM_FIGHT;
        m_popDlg->ShowWidget(countname, 2);
        m_pNoticeidget->AddInfo(countname + QStringLiteral(": 邀请你一起游戏！"));
        break;

    case GAMING:
        qDebug() << "case GAMING: " << m_account.ncolor;
        if(m_account.fighttype == RANDOM_FIGHT &&
           m_account.contername.size() == 0)
        {
            GetString(pch, tstr1, count);
            m_account.contername = tstr1;
        }
        m_bSendColor = false;
        RecvGaming();
        PlayGame();
        break;

    case GAMING_B:
        GetString(pch, tstr1, count);
        GetString(pch, tstr2, count);
        SetCurPlayersStatus(tstr1, tstr2, true);
        m_playlistWig->UpdateItems(m_account.playervec);
        break;

    case WINER:
        GetString(pch, tstr1, count);
        m_account.gamestatus = WIN;
        m_gameModel->SetGameStatus(WIN);
        m_gameModel->SetTipType(WINIMG);
        m_gameModel->SetTurn(false);
        QSound::play(":/sound/win.wav");
        m_sidebarWig->SetRadiosEnable(true);
        m_pNoticeidget->AddInfo(tstr1 + QStringLiteral(": 恭喜你，赢得比赛！"));
        break;

    case LOSER:
        RecvPos(pch, count);
        m_account.gamestatus = LOSE;
        m_gameModel->SetGameStatus(LOSE);
        m_gameModel->SetTipType(LOSEIMG);
        m_gameModel->SetTurn(false);
        QSound::play(":/sound/lose.wav");
        m_sidebarWig->SetRadiosEnable(true);
        m_pNoticeidget->AddInfo(m_account.username + QStringLiteral(": 对不起，你输了！"));
        break;

    case GAMEOUT:
        m_account.gametype = COMPUTER;
        m_pNoticeidget->AddInfo(m_account.contername + QStringLiteral(": 退出游戏。已开启人机对战模式。"));
        m_gameModel->SetGameType(COMPUTER);
        m_gameModel->InitScoreVec();
        m_account.bwithfriend = false;
        m_sidebarWig->SetGameType(COMPUTER);
        m_chatWig->setWidgetEnable(false);
        m_playlistWig->SetMatchValue(false);
        if(m_gameModel->MyTurn() == false)
            m_gameModel->ActionByAI();
        for(int i = 0; i < m_account.playervec.size(); i++)
        {
            if(m_account.playervec[i].name == m_account.username ||
               m_account.playervec[i].name == m_account.contername)
                m_account.playervec[i].isplaying = false;
        }
        m_playlistWig->UpdateItems(m_account.playervec);
        m_account.contername.clear();
        break;

    case PLAYER_FREE:
        GetString(pch, tstr1, count);
        GetString(pch, tstr2, count);
        SetCurPlayersStatus(tstr1, tstr2, false);
        m_playlistWig->UpdateItems(m_account.playervec);
        m_account.contername.clear();
        break;

    case LOGON_OFFLINE:
        GetString(pch, tstr1, count);
        for(int i = 0; i < m_account.playervec.size(); i++)
        {
            if(m_account.playervec[i].name == tstr1)
            {
                m_account.playervec.erase(m_account.playervec.begin() + i);
                break;
            }
        }
        m_playlistWig->UpdateItems(m_account.playervec);
        break;

    case GAME_OFFLINE:
        GameOffLine(pch, count);
        break;

    case SEND_COLOR:
        m_bSendColor = false;
        type = GetDword(pch, count);
        m_account.ncolor = type;
        m_sidebarWig->SetPiecesColor(type);
        m_sidebarWig->SetRadiosEnable(false);
        m_pNoticeidget->SetPiecePixmap(type);
        m_gameModel->SetMyColor(type);
        m_gameModel->SetTurn(m_account.ncolor == -1 ? true : false);

        count = 4;
        PutDword(tstr1, RECV_COLOR, count);
        PutDword(tstr1, type, count);
        PutPacketSize(tstr1, count);
        NetWork::GetNetWork()->WriteData(tstr1, count);
        m_pNoticeidget->AddInfo(QStringLiteral("对方玩家协商棋子颜色。"));
        break;

    case RECV_COLOR:
        m_bSendColor = false;
        m_pNoticeidget->AddInfo(QStringLiteral("棋子颜色协商完毕，单击开始进行游戏。"));
        break;

    default:
        break;
    }
}

void PacketProcessor::SetGameStatus(GameStatus status)
{
    m_account.gamestatus = status;
    m_sidebarWig->SetRadiosEnable(true);
}

void PacketProcessor::AddPlayer(OnlinePlayer player)
{
    for(int i = 0; i < m_account.playervec.size(); i++)
    {
        if(m_account.playervec[i].name == player.name)
        {
            m_account.playervec[i].isplaying = player.isplaying;
            return;
        }
    }
    m_account.playervec.push_back(player);
}

void PacketProcessor::GameOffLine(char *pch, int pos)
{
    char pstr1[32], pstr2[32];
    int cnt = 0;

    GetString(pch, pstr1, pos);
    GetString(pch, pstr2, pos);

    for(int i = 0; i < m_account.playervec.size(); i++)
    {
        if(m_account.playervec[i].name == pstr1)
        {
            m_account.playervec.erase(m_account.playervec.begin() + i);
            i--;
            cnt++;
            continue;
        }
        else if(m_account.playervec[i].name == pstr2)
        {
            m_account.playervec[i].isplaying = false;
            cnt++;
            continue;
        }
        if(cnt == 2)
            break;
    }

    if(m_account.username == pstr2)
    {
        m_account.bwithfriend = false;
        m_chatWig->setWidgetEnable(false);
        m_playlistWig->SetMatchValue(false);
        m_account.contername.clear();

        if(m_gameModel->GetGameStatus() == PLAYING)
        {
            m_account.gametype = COMPUTER;
            m_gameModel->SetGameType(COMPUTER);
            m_gameModel->InitScoreVec();
            m_sidebarWig->SetGameType(COMPUTER);
            if(m_gameModel->MyTurn() == false)
                m_gameModel->ActionByAI();
            m_pNoticeidget->AddInfo(QStringLiteral("对方离开游戏,已开启人机对战模式。"));
        }
        else
        {
            m_pNoticeidget->AddInfo(pstr1 + QStringLiteral("对方离开游戏。"));
        }
    }
    m_playlistWig->UpdateItems(m_account.playervec);
}

void PacketProcessor::RecvGaming()
{
    int ii = -1;
    OnlinePlayer player;
    if(m_account.bwithfriend == false)
    {
        m_popDlg->ShowWidget(m_account.contername, 0);
        m_chatWig->setWidgetEnable(true);
        for(int i = 0; i < m_account.playervec.size(); i++)
        {
            if(m_account.playervec[i].name == m_account.username)
                m_account.playervec[i].isplaying = true;
            else if(m_account.playervec[i].name == m_account.contername)
            {
                m_account.playervec[i].isplaying = true;
                ii = i;
            }
        }
        m_account.bwithfriend = true;

        if(ii < 0)
        {
            player.isplaying = true;
            player.name = m_account.contername;
            m_account.playervec.push_back(player);
        }
    }

    m_playlistWig->UpdateItems(m_account.playervec);
}

void PacketProcessor::ExitGame()
{
    if(m_gameModel->GetGameType() == COMPUTER)
    {
        m_gameModel->SetGameStatus(NONE);
        m_gameModel->SetTurn(false);
        m_gameModel->InitGame();
    }
    else if(m_gameModel->GetGameType() == HUMAN)
    {
        SendGameOut();
        SetCurPlayersStatus(false);
        m_account.contername.clear();
        m_gameModel->SetGameStatus(NONE);
        m_gameModel->SetTurn(false);
        m_gameModel->InitGame();
        m_playlistWig->UpdateItems(m_account.playervec);
        m_chatWig->setWidgetEnable(false);
        m_playlistWig->SetMatchValue(false);
        m_account.bwithfriend = false;
    }
    m_sidebarWig->SetRadiosEnable(true);
}

void PacketProcessor::SetGameType(GameType type)
{
    m_account.gametype = type;
}

void PacketProcessor::GameStartBtn()
{
    if(m_account.gametype == HUMAN && m_account.bwithfriend == false)
    {
        m_pNoticeidget->AddInfo(QStringLiteral("对不起，你还未匹配队友。"));
        return;
    }
    else if(m_gameModel->GetGameStatus() == PLAYING)
    {
        m_pNoticeidget->AddInfo(QStringLiteral("你正在游戏中，请勿重复点击。"));
        return;
    }
    else if(m_account.gametype == COMPUTER)
        PlayGame();
    else if(m_bSendColor == true)
    {
        m_pNoticeidget->AddInfo(QStringLiteral("正在协商棋子颜色，请稍后。"));
        return;
    }
    else if(m_account.bwithfriend == true)
        SendRestart();
}

void PacketProcessor::SendRestart()
{
    char msg[16];
    int count = 4;

    if(NetWork::GetNetWork()->IsConnected() == false)
        if(NetWork::GetNetWork()->slot_Connect() == false)
            return;

    PutDword(msg, RESTART, count);
    PutPacketSize(msg, count);
    NetWork::GetNetWork()->WriteData(msg, count);

    m_gameModel->SetGameStatus(READY);
    m_gameModel->InitGame();
    m_gameModel->SetTipType(READYIMG);
    m_pNoticeidget->AddInfo(m_account.username + QStringLiteral(": 准备。"));
}

void PacketProcessor::PlayGame()
{
    m_account.historyvec.clear();
    m_account.nchssorder = 0;
    m_gameModel->SetMyColor(m_account.ncolor);
    m_gameModel->SetGameType(m_account.gametype);
    m_gameModel->SetTurn(m_account.ncolor == -1 ? true : false);
    m_gameModel->SetGameStatus(PLAYING);
    m_gameModel->SetTipType(NONEIMG);
    if(m_account.gametype == HUMAN)
    {
        m_pNoticeidget->AddInfo(QStringLiteral("联网对战游戏开始。"));
    }
    else
    {
        m_pNoticeidget->AddInfo(QStringLiteral("人机对战游戏开始。"));
    }
    m_sidebarWig->SetRadiosEnable(false);
    m_gameModel->InitGame();
}

void PacketProcessor::ShowPopDlg(int type)
{
    m_popDlg->ShowWidget(type);
}

void PacketProcessor::Logon(QString name, QString password)
{
    char msg[128];
    QByteArray arr;
    int count = 4;

    if(NetWork::GetNetWork()->IsConnected() == false)
        if(NetWork::GetNetWork()->slot_Connect() == false)
            return;
    m_account.username = name.toLatin1();
    m_account.password = password.toLatin1();
    PutDword(msg, LOGON, count);
    arr = name.toLatin1();
    PutString(msg, arr.data(), count);
    arr = password.toLatin1();
    PutString(msg, arr.data(), count);
    PutPacketSize(msg, count);
    NetWork::GetNetWork()->WriteData(msg, count);
}

void PacketProcessor::Rigester(QString name, QString password)
{
    char msg[128];
    QByteArray arr;
    int count = 4;

    if(NetWork::GetNetWork()->IsConnected() == false)
        if(NetWork::GetNetWork()->slot_Connect() == false)
            return;

    m_account.username = name.toLatin1();
    m_account.password = password.toLatin1();
    PutDword(msg, REGIST, count);
    arr = name.toLatin1();
    PutString(msg, arr.data(), count);
    arr = password.toLatin1();
    PutString(msg, arr.data(), count);
    PutPacketSize(msg, count);
    NetWork::GetNetWork()->WriteData(msg, count);
}

void PacketProcessor::PlayingWith(QByteArray arr)
{
    char msg[64];
    int count = 4;

    if(m_account.username.size() == 0)
        return;

    if(arr == m_account.username)
    {
        m_playlistWig->SetMatchValue(false);
        ShowPopDlg(0);
        return;
    }

    m_account.fighttype = DIRECT_FIGHT; ///////
    m_account.contername = arr;
    PutDword(msg, DIRECT_FIGHT, count);
    PutString(msg, m_account.username.data(), count);
    PutDword(msg, m_account.ncolor, count);
    PutString(msg, arr.data(), count);
    PutPacketSize(msg, count);
    NetWork::GetNetWork()->WriteData(msg, count);
    m_account.gametype = HUMAN;
    m_sidebarWig->SetGameType(HUMAN);
    ShowPopDlg(1);///////////////
    AddNoticeMsg(QStringLiteral("已向玩家 < ") + arr + QStringLiteral(" > 发出游戏邀请。"));
}

void PacketProcessor::RandomPlay()
{
    char msg[16];
    int count = 4;

    if(m_account.username.size() == 0)
        return;

    if(m_account.playervec.size() == 1)
    {
        m_playlistWig->SetMatchValue(false);
        ShowPopDlg(0);
        return;
    }

    m_account.fighttype = RANDOM_FIGHT; //////////////
    PutDword(msg, RANDOM_FIGHT, count);
    PutString(msg, m_account.username.data(), count);
    PutDword(msg, m_account.ncolor, count);
    PutPacketSize(msg, count);
    NetWork::GetNetWork()->WriteData(msg, count);
    m_account.gametype = HUMAN;
    m_sidebarWig->SetGameType(HUMAN);

    ShowPopDlg(1);/////////////
    AddNoticeMsg(QStringLiteral("已向服务器发出随机游戏请求。"));
}

void PacketProcessor::SendMsg(QByteArray arr)
{
    char msg[1024];
    int count = 4;

    if(m_account.username.size() == 0)
        return;

    PutDword(msg, CHAT, count);
    PutString(msg, m_account.username.data(), count);
    PutString(msg, m_account.contername.data(), count);
    PutString(msg, arr.data(), count);
    PutPacketSize(msg, count);
    NetWork::GetNetWork()->WriteData(msg, count);
}

void PacketProcessor::SendPos(QPoint pos)
{
    char msg[128];
    int count = 4;

    if(m_account.username.size() == 0)
        return;

    ChessPos temppos;
    temppos.order = m_account.nchssorder;
    temppos.pos = pos;
    temppos.color = m_account.ncolor;
    m_account.historyvec.push_back(temppos);
    PutDword(msg, POS, count);
    PutDword(msg, m_account.nchssorder, count);
    PutString(msg, m_account.username.data(), count);
    PutDword(msg, m_account.ncolor, count);
    PutDword(msg, pos.x(), count);
    PutDword(msg, pos.y(), count);
    PutPacketSize(msg, count);
    NetWork::GetNetWork()->WriteData(msg, count);
    m_account.nchssorder++;
}

void PacketProcessor::RecvPos(char *pac, int &pos)
{
    char str[32];
    int  order, color, x, y;

    order = GetDword(pac, pos);
    GetString(pac, str, pos);
    color = GetDword(pac, pos);
    x = GetDword(pac, pos);
    y = GetDword(pac, pos);

    if(m_account.nchssorder != order || color != -m_account.ncolor || m_account.contername != str)
    {
        qDebug() << "Recv point wrong.";
    }

    ChessPos temppos;
    temppos.order = m_account.nchssorder;
    temppos.pos.setX(x);
    temppos.pos.setY(y);
    temppos.color = m_account.ncolor;
    m_account.historyvec.push_back(temppos);
    m_gameModel->ActionByCounter(x, y);
    m_account.nchssorder ++;

    qDebug() << "RecvPos(char *pac, int &pos).";
}

void PacketProcessor::SendFightY()
{
    char msg[64];
    int count = 4;

    if(m_account.username.size() == 0)
        return;

    if(m_account.fighttype == RANDOM_FIGHT)
        PutDword(msg, RANDOM_FIGHT_Y, count);
    else
        PutDword(msg, DIRECT_FIGHT_Y, count);
    PutString(msg, m_account.username.data(), count);
    PutPacketSize(msg, count);
    NetWork::GetNetWork()->WriteData(msg, count);

    m_account.ncolor = piececolor;
    m_account.contername = countname;
    m_sidebarWig->SetPiecesColor(piececolor);
    m_sidebarWig->SetGameType(HUMAN);
    m_account.gametype = HUMAN;
    m_account.bwithfriend = true;
    m_playlistWig->SetMatchValue(true);
    m_chatWig->setWidgetEnable(true);
    m_pNoticeidget->SetPiecePixmap(m_account.ncolor);
    m_gameModel->InitGame();
    m_gameModel->SetGameStatus(PLAYING);
    m_account.gamestatus = PLAYING;

    SetCurPlayersStatus(true);
    m_playlistWig->UpdateItems(m_account.playervec);
}

void PacketProcessor::SetCurPlayersStatus(bool isplaying)
{
    int ii = -1;
    OnlinePlayer player;

    for(int i = 0; i < m_account.playervec.size(); i++)
    {
        if(m_account.playervec[i].name == m_account.username)
            m_account.playervec[i].isplaying = isplaying;
        else if(m_account.playervec[i].name == m_account.contername)
        {
            m_account.playervec[i].isplaying = isplaying;
            ii = i;
        }
    }
    if(ii < 0)
    {
        player.isplaying = isplaying;
        player.name = m_account.contername;
        m_account.playervec.push_back(player);
    }
}

void PacketProcessor::SetCurPlayersStatus(char *str1, char *str2, bool isplaying)
{
    int i1 = -1, i2 = -1;
    OnlinePlayer player;

    for(int i = 0; i < m_account.playervec.size(); i++)
    {
        if(m_account.playervec[i].name == str1)
        {
            i1 = i;
            m_account.playervec[i].isplaying = isplaying;
        }
        else if(m_account.playervec[i].name == str2)
        {
            m_account.playervec[i].isplaying = isplaying;
            i2 = i;
        }
    }

    if(i1 < 0)
    {
        player.isplaying = isplaying;
        player.name = str1;
        m_account.playervec.push_back(player);
    }

    if(i2 < 0)
    {
        player.isplaying = isplaying;
        player.name = str2;
        m_account.playervec.push_back(player);
    }
}

void PacketProcessor::SendFightN()
{
    char msg[64];
    int count = 4;

    if(m_account.username.size() == 0)
        return;

    if(m_account.fighttype == RANDOM_FIGHT)
        PutDword(msg, RANDOM_FIGHT_N, count);
    else
        PutDword(msg, DIRECT_FIGHT_N, count);
    PutString(msg, m_account.username.data(), count);
    PutPacketSize(msg, count);
    NetWork::GetNetWork()->WriteData(msg, count);
}

void PacketProcessor::SendGameOut()
{
    char msg[64];
    int count = 4;
    PutDword(msg, GAMEOUT, count);
    PutString(msg, m_account.username.data(), count);
    PutPacketSize(msg, count);
    NetWork::GetNetWork()->WriteData(msg, count);
}
