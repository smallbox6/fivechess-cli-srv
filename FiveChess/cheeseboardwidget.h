#ifndef CHEESEBOARDWIDGET_H
#define CHEESEBOARDWIDGET_H

#include "gamemodel.h"

#include <QWidget>


class CheeseBoardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CheeseBoardWidget(QWidget *parent = nullptr);
    ~CheeseBoardWidget();

    void SetOriginPos(int x0, int y0);

    void ChessOneByPerson(); //人执行
    void ChessOneByAI(); //AI下棋

    inline void UpdateBoard(){update();}

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    bool isInChessBoard();
    inline bool CheckCurPntPositive() {
        if(m_currentPos.x() != -1 && m_currentPos.y() != -1)
            return true;
        return false;
    }

private:
    GameModel m_gameModel; // 游戏指针
    GameType game_type; // 存储游戏类型
    QPoint m_currentPos;
    int m_WigLen;
    int m_kBoardMargin; // 棋盘边缘空隙
    int m_kRadius;     // 棋子半径
    int m_kMarkSize;    // 标记边长
    int m_kBlockSize;  // 格子的大小
    int m_kPosDelta;   // 鼠标点击的模糊距离上限
    int m_kAIDelay;   // AI落子时间间隔
    int m_kBoardSizeNum; // 棋盘尺寸
    int m_nChessFalls;
};

#endif // CHEESEBOARDWIDGET_H
