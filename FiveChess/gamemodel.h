#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include "common.h"
#include <vector>
#include <QPoint>

class CheeseBoardWidget;


class GameModel
{

public:
    GameModel();

    void InitGame();

    void   ActionByCounter(int row, int col);
    void   ActionByHuman(int row, int col);
    QPoint ActionByAI();


    bool IsWin(int row, int col);
    bool IsDeadGame();

    int& At(int row, int col){return m_chessBoard[row][col];}

    inline void SetChssBoardWig(CheeseBoardWidget *wig) {m_pChssWig = wig;}

    inline void ChangeTurn() {m_bTurn = !m_bTurn;}
    inline void SetTurn(bool turn) {m_bTurn = turn;}
    inline bool MyTurn() {return m_bTurn;}

    inline void SetGameStatus(GameStatus status) {m_gameStatus = status;}
    inline GameStatus GetGameStatus() {return m_gameStatus;}

    inline void SetGameType(GameType type) {m_gameType = type;}
    inline bool GetGameType() {return m_gameType;}

    inline int GetMyColor() {return m_nPieceColor;}
    inline int SetMyColor(int color) {return m_nPieceColor = color;}

    inline int  GetTipType() {return m_tipType;}
    void SetTipType(TipType tiptype);

    void InitScoreVec();

private:
    void CalculateScore();
    void UpdateGameMap(int row, int col);

private:
    CheeseBoardWidget *m_pChssWig;
    QPoint m_curPos;
    bool m_bTurn; // 标示下棋方
    int  m_nPieceColor;
    int  m_nChessLines;
    int  m_chessBoard[15][15];
    std::vector<std::vector<int>> m_scoreMapVec; // 存储各个点位的评分情况
    GameType   m_gameType; // 游戏模式
    GameStatus m_gameStatus; // 游戏状态
    TipType    m_tipType;
};

#endif // GAMEMODEL_H
