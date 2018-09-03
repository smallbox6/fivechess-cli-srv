#include "gamemodel.h"
#include "packetprocessor.h"

#include <utility>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <QMessageBox>
#include <QSound>
#include <QDebug>

GameModel::GameModel()
{
    m_bTurn = false;
    m_nPieceColor = -1;
    m_nChessLines = 15;
    m_gameType = COMPUTER;
    m_gameStatus = NONE;
    m_tipType = NONEIMG;
    memset(m_chessBoard, 0, sizeof(m_chessBoard));
    PacketProcessor::GetPacketProcessor()->SetGameModel(this);
}

void GameModel::SetTipType(TipType tiptype)
{
    m_tipType = tiptype;
    m_pChssWig->UpdateBoard();
}


void GameModel::InitGame()
{
    memset(m_chessBoard, 0, sizeof(m_chessBoard));

    if (m_gameType == COMPUTER)
    {
        InitScoreVec();

        if(m_nPieceColor == 1)
            UpdateGameMap(7, 7);
    }
    m_pChssWig->UpdateBoard();
}

void GameModel::InitScoreVec()
{
    m_scoreMapVec.clear();
    for (int i = 0; i < m_nChessLines; i++)
    {
        std::vector<int> lineScores;
        for (int j = 0; j < m_nChessLines; j++)
            lineScores.push_back(0);
        m_scoreMapVec.push_back(lineScores);
    }
}

void GameModel::UpdateGameMap(int row, int col)
{
    if (m_bTurn)
        m_chessBoard[row][col] = m_nPieceColor;
    else
        m_chessBoard[row][col] = -m_nPieceColor;

    if(m_gameType == COMPUTER)
    {
        if(IsWin(row, col))
        {
            if(GetGameStatus() == WIN)
            {
                SetTipType(WINIMG);
                QSound::play(":/sound/win.wav");
            }
            else
            {
                SetTipType(LOSEIMG);
                QSound::play(":/sound/lose.wav");
            }
            QString str;
            if (m_chessBoard[row][col] == m_nPieceColor)
                str = QStringLiteral("恭喜你，赢得比赛！");
            else if (m_chessBoard[row][col] == -m_nPieceColor)
                str = QStringLiteral("不好意思，你输了！");
            PacketProcessor::GetPacketProcessor()->AddNoticeMsg(str);
            PacketProcessor::GetPacketProcessor()->SetGameStatus(m_gameStatus);
            m_pChssWig->UpdateBoard();
            return;
        }

        // 判断死局
        if (IsDeadGame())
        {
            SetGameStatus(DEAD);
            QSound::play(":/sound/lose.wav");
            PacketProcessor::GetPacketProcessor()->AddNoticeMsg(QStringLiteral("平局！"));
            PacketProcessor::GetPacketProcessor()->SetGameStatus(m_gameStatus);
            m_pChssWig->UpdateBoard();
            return;
        }
    }

    ChangeTurn();
}

void GameModel::ActionByCounter(int row, int col)
{
    UpdateGameMap(row, col);
    m_pChssWig->UpdateBoard();
}

void GameModel::ActionByHuman(int row, int col)
{
    UpdateGameMap(row, col);
    if(m_gameType == HUMAN && m_gameStatus == PLAYING)
        PacketProcessor::GetPacketProcessor()->SendPos(QPoint(row, col));
}

QPoint GameModel::ActionByAI()
{
    // 计算评分
    CalculateScore();

    // 从评分中找出最大分数的位置
    int maxScore = 0;
    std::vector<std::pair<int, int>> maxPoints;

    for (int row = 1; row < m_nChessLines; row++)
        for (int col = 1; col < m_nChessLines; col++)
        {
            if (m_chessBoard[row][col] == 0)
            {
                if (m_scoreMapVec[row][col] > maxScore)  // 找最大的数和坐标
                {
                    maxPoints.clear();
                    maxScore = m_scoreMapVec[row][col];
                    maxPoints.push_back(std::make_pair(row, col));
                }
                else if (m_scoreMapVec[row][col] == maxScore) // 如果有多个最大的数，都存起来
                    maxPoints.push_back(std::make_pair(row, col));
            }
        }

    // 随机落子，如果有多个点的话
    srand((unsigned)time(0));
    int index = rand() % maxPoints.size();

    std::pair<int, int> pointPair = maxPoints.at(index);
    m_curPos = {pointPair.first, pointPair.second}; // 记录落子点
    UpdateGameMap(pointPair.first, pointPair.second);
    return m_curPos;
}

// 最关键的计算评分函数
void GameModel::CalculateScore()
{
    int personNum = 0; // 玩家连成子的个数
    int botNum = 0; // AI连成子的个数
    int emptyNum = 0; // 各方向空白位的个数

    // 清空评分数组
    for (int i = 0; i < m_nChessLines; i++)
    {
        for (int j = 0; j < m_nChessLines; j++)
            m_scoreMapVec[i][j] = 0;
    }

    // 计分
    for (int row = 0; row < m_nChessLines; row++)
        for (int col = 0; col < m_nChessLines; col++)
        {
            // 空白点就算
            if (m_chessBoard[row][col] == 0)
            {
                // 遍历周围八个方向
                for (int y = -1; y <= 0; y++)
                    for (int x = -1; x <= 1; x++)
                    {
                        // 重置
                        personNum = 0;
                        botNum = 0;
                        emptyNum = 0;

                        // 原坐标不算
                        if (-1 == y || (0 == y && -1 == x))
                        {
                            int r = 0, c = 0;

                            // 对玩家白子评分（正反两个方向）
                            for (int i = 1; i <= 4; i++)
                            {
                                r = row + i * y;
                                c = col + i * x;
                                if((r > 0 && r < m_nChessLines && c > 0 && c < m_nChessLines) == false)
                                    break;
                                if (m_chessBoard[r][c] == m_nPieceColor) // 玩家的子
                                {
                                    personNum++;
                                }
                                else if (m_chessBoard[r][c] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else
                                    break;
                            }

                            for (int i = 1; i <= 4; i++)
                            {
                                r = row - i * y;
                                c = col - i * x;
                                if((r > 0 && r < m_nChessLines && c > 0 && c < m_nChessLines) == false)
                                    break;
                                if (m_chessBoard[r][c] == m_nPieceColor) // 玩家的子
                                {
                                    personNum++;
                                }
                                else if (m_chessBoard[r][c] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else
                                    break;
                            }

                            if (personNum == 1)                      // 杀二
                                m_scoreMapVec[row][col] += 50;
                            else if (personNum == 2)                 // 杀三
                            {
                                if (emptyNum == 1)
                                    m_scoreMapVec[row][col] += 100;
                                else if (emptyNum == 2)
                                    m_scoreMapVec[row][col] += 200;
                            }
                            else if (personNum == 3)                 // 杀四
                            {
                                if (emptyNum == 1)
                                    m_scoreMapVec[row][col] += 500;
                                else if (emptyNum == 2)
                                    m_scoreMapVec[row][col] += 1500;
                            }
                            else if (personNum == 4)                 // 杀五
                                m_scoreMapVec[row][col] += 12000;

                            // 进行一次清空
                            emptyNum = 0;

                            // 对AI黑子评分
                            for (int i = 1; i <= 4; i++)
                            {
                                r = row + i * y;
                                c = col + i * x;
                                if((r > 0 && r < m_nChessLines && c > 0 && c < m_nChessLines) == false)
                                    break;
                                if (m_chessBoard[r][c] == -m_nPieceColor)
                                {
                                    botNum++;
                                }
                                else if (m_chessBoard[r][c] == 0)
                                {
                                    emptyNum++;
                                    break;
                                }
                                else
                                    break;
                            }

                            for (int i = 1; i <= 4; i++)
                            {
                                r = row - i * y;
                                c = col - i * x;
                                if((r > 0 && r < m_nChessLines && c > 0 && c < m_nChessLines) == false)
                                    break;
                                if (m_chessBoard[r][c] == -m_nPieceColor)
                                {
                                    botNum++;
                                }
                                else if (m_chessBoard[r][c] == 0)
                                {
                                    emptyNum++;
                                    break;
                                }
                                else
                                    break;
                            }

                            if (botNum == 0)                      // 普通下子
                                m_scoreMapVec[row][col] += 5;
                            else if (botNum == 1)                 // 活二
                                m_scoreMapVec[row][col] += 50;
                            else if (botNum == 2)
                            {
                                if (emptyNum == 1)                // 死三
                                    m_scoreMapVec[row][col] += 80;
                                else if (emptyNum == 2)
                                    m_scoreMapVec[row][col] += 200;  // 活三
                            }
                            else if (botNum == 3)
                            {
                                if (emptyNum == 1)                // 死四
                                    m_scoreMapVec[row][col] += 800;
                                else if (emptyNum == 2)
                                    m_scoreMapVec[row][col] += 1200; // 活四
                            }
                            else if (botNum >= 4)
                                m_scoreMapVec[row][col] += 10000;   // 活五
                        }
                  }
            }
      }
}

// 往左5个，往右匹配4个子
bool GameModel::IsWin(int row, int col)
{
    int rw = 0, cl = 0;
    // k = 0
    for (int i = 0; i < 5; i++)
    {
        cl = col - i;
        if (cl >= 0 && cl + 4 < m_nChessLines &&
            m_chessBoard[row][cl] == m_chessBoard[row][cl + 1] &&
            m_chessBoard[row][cl] == m_chessBoard[row][cl + 2] &&
            m_chessBoard[row][cl] == m_chessBoard[row][cl + 3] &&
            m_chessBoard[row][cl] == m_chessBoard[row][cl + 4])
        {
            if(m_bTurn) m_gameStatus = WIN;
            else m_gameStatus = LOSE;
            m_bTurn = false;
            return true;
        }
    }

    // k = 90
    for (int i = 0; i < 5; i++)
    {
        rw = row - i;
        if (rw >= 0 && rw + 4 < m_nChessLines &&
            m_chessBoard[rw][col] == m_chessBoard[rw + 1][col] &&
            m_chessBoard[rw][col] == m_chessBoard[rw + 2][col] &&
            m_chessBoard[rw][col] == m_chessBoard[rw + 3][col] &&
            m_chessBoard[rw][col] == m_chessBoard[rw + 4][col])
        {
            if(m_bTurn) m_gameStatus = WIN;
            else m_gameStatus = LOSE;
            m_bTurn = false;
            return true;
        }
    }

    // k = 45
    for (int i = 0; i < 5; i++)
    {
        rw = row + i;
        cl = col - i;
        if (rw < m_nChessLines && rw - 4 >= 0 &&
            cl >= 0 && cl + 4 < m_nChessLines &&
            m_chessBoard[rw][cl] == m_chessBoard[rw - 1][cl + 1] &&
            m_chessBoard[rw][cl] == m_chessBoard[rw - 2][cl + 2] &&
            m_chessBoard[rw][cl] == m_chessBoard[rw - 3][cl + 3] &&
            m_chessBoard[rw][cl] == m_chessBoard[rw - 4][cl + 4])
        {
            if(m_bTurn) m_gameStatus = WIN;
            else m_gameStatus = LOSE;
            m_bTurn = false;
            return true;
        }
    }

    // k = 135
    for (int i = 0; i < 5; i++)
    {
        rw = row - i;
        cl = col - i;
        if (rw >= 0 && rw + 4 < m_nChessLines &&
            cl >= 0 && cl + 4 < m_nChessLines &&
            m_chessBoard[rw][cl] == m_chessBoard[rw + 1][cl + 1] &&
            m_chessBoard[rw][cl] == m_chessBoard[rw + 2][cl + 2] &&
            m_chessBoard[rw][cl] == m_chessBoard[rw + 3][cl + 3] &&
            m_chessBoard[rw][cl] == m_chessBoard[rw + 4][cl + 4])
        {
            if(m_bTurn) m_gameStatus = WIN;
            else m_gameStatus = LOSE;
            m_bTurn = false;
            return true;
        }
    }
    return false;
}

bool GameModel::IsDeadGame()
{
    for (int i = 1; i < m_nChessLines; i++)
        for (int j = 1; j < m_nChessLines; j++)
        {
            if (m_chessBoard[i][j] == 0 || m_chessBoard[i][j] == 0)
                return false;
        }
    m_bTurn = false;
    return true;
}


