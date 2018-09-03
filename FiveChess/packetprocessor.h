#ifndef PACKETPROCESSOR_H
#define PACKETPROCESSOR_H

#include "chatwidget.h"
#include "sideconfigbar.h"
#include "mainwindow.h"
#include "TopWidget.h"
#include "PlayerOnLineList.h"
#include "common.h"
#include "noticewidget.h"
#include "gamemodel.h"

#include <QObject>

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

struct ChessPos
{
    int order;
    int color;
    QPoint pos;
};

struct CliAccount
{
    CliAccount(){
        gamestatus = NONE;
        gametype = COMPUTER;
        ncolor = -1;
        islogon = false;
        bwithfriend = false;
        nchssorder = 0;
        fighttype = LOGON_OFFLINE;
    }

    std::vector<ChessPos> historyvec;
    std::vector<OnlinePlayer> playervec;
    QByteArray username;
    QByteArray password;
    QByteArray contername;
    GameStatus gamestatus;
    GameType   gametype;
    int        nchssorder;
    PacketType   fighttype;
    int   ncolor;
    bool  islogon;
    bool  bwithfriend;
};

class PacketProcessor : public QObject
{
    Q_OBJECT

public:
    ~PacketProcessor(){}
    static PacketProcessor* GetPacketProcessor();

    inline bool GetWithFriend(){ return m_account.bwithfriend;}

    void SetPieceColor(int ncolor);
    inline int  GetPieceColor(){return m_account.ncolor;}

    bool IsLogon(){return m_account.islogon;}
    inline bool IsPlaying(){if(m_account.gamestatus == PLAYING) return true; return false;}

    void SetGameStatus(GameStatus status);
    void SetGameType(GameType type);
    GameStatus GetGameStatus(){return m_gameModel->GetGameStatus();}

    void GameStartBtn();

    inline void AddNoticeMsg(QString str) {m_pNoticeidget->AddInfo(str);}

    inline void SetChatWig(ChatWidget *wig){m_chatWig = wig;}
    inline void SetPlaylistWig(PlayerOnLineList *wig){m_playlistWig = wig;}
    inline void SetSidebarWig(SideConfigBar *wig){m_sidebarWig = wig;}
    inline void SetMainWig(MainWindow *wig){m_mainWig = wig;}
    inline void SetTopWig(TopWidget *wig){m_topWig = wig;}
    inline void SetGameModel(GameModel *model){m_gameModel = model;}
    inline void SetPopDlg(PopDlg *wig){m_popDlg = wig;}
    inline void SetNoticeWidget(NoticeWidget *wig){m_pNoticeidget = wig;}

    void Logon(QString name, QString password);
    void Rigester(QString name, QString password);
    void PlayingWith(QByteArray arr);
    void RandomPlay();
    void SendMsg(QByteArray arr);
    void SendPos(QPoint pos);
    void RecvPos(char *pac, int &pos);
    void SendFightY();
    void SendFightN();
    void SendGameOut();

    void HandlePacket(QByteArray arr);
    void ShowPopDlg(int type);

    void ExitGame();

    void RecvGaming();

private:
    explicit PacketProcessor(QObject *parent = nullptr);
    void PlayGame();
    void SendRestart();
    void SetCurPlayersStatus(bool isplaying);
    void SetCurPlayersStatus(char *str1, char *str2, bool isplaying);
    void GameOffLine(char *pch, int pos);
    void AddPlayer(OnlinePlayer player);

signals:

public slots:

private:
    bool m_bSendColor;

    ChatWidget  *m_chatWig;
    PlayerOnLineList *m_playlistWig;
    SideConfigBar *m_sidebarWig;
    MainWindow    *m_mainWig;
    TopWidget     *m_topWig;
    PopDlg        *m_popDlg;
    NoticeWidget  *m_pNoticeidget;
    GameModel     *m_gameModel;

    CliAccount m_account;
    PacketType m_packtp;
};

#endif // PACKETPROCESSOR_H
