#include "cheeseboardwidget.h"
#include "packetprocessor.h"
#include "common.h"

#include <math.h>

#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTimer>
#include <QSound>
#include <QCursor>

#define CHESS_ONE_SOUND ":/sound/chessone.wav"

CheeseBoardWidget::CheeseBoardWidget(QWidget *parent) : QWidget(parent)
{
    m_kBoardMargin = 20; // 棋盘边缘空隙
    m_kRadius = 10; // 棋子半径
    m_kMarkSize = 6; // 标记边长
    m_kBlockSize = 30; // 格子的大小
    m_kPosDelta = 15; // 鼠标点击的模糊距离上限
    m_kAIDelay = 700; // AI落子时间间隔
    m_kBoardSizeNum = 14;
    m_nChessFalls = 15;

    setFocusPolicy(Qt::ClickFocus);
    m_gameModel.SetChssBoardWig(this);
    setCursor(QCursor(QPixmap(":/Image/IMG/Cursor.png"), 0, 0));
    m_WigLen = m_kBoardMargin * 2 + m_kBlockSize * m_kBoardSizeNum;
    setMouseTracking(true);
}


CheeseBoardWidget::~CheeseBoardWidget()
{
}

void CheeseBoardWidget::SetOriginPos(int x0, int y0)
{
    setGeometry(x0, y0, m_WigLen, m_WigLen);
}

void CheeseBoardWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPixmap pic(":/Image/IMG/wood.jpg");
    painter.drawPixmap(0, 0, this->width(), this->height(), pic);

    for (int i = 0; i < m_nChessFalls; i++)
    {
        //-
        painter.drawLine(m_kBoardMargin + m_kBlockSize * i, m_kBoardMargin, m_kBoardMargin + m_kBlockSize * i, height() - m_kBoardMargin);
        //|
        painter.drawLine(m_kBoardMargin, m_kBoardMargin + m_kBlockSize * i, width() - m_kBoardMargin, m_kBoardMargin + m_kBlockSize * i);
    }

    QBrush brush;
    QPen pen;
    // 绘制落子标记
    if(m_gameModel.GetGameStatus() == PLAYING && m_gameModel.MyTurn())
    {
        brush.setStyle(Qt::SolidPattern);
        if (m_currentPos.x() >= 0 && m_currentPos.x() < m_nChessFalls &&
            m_currentPos.y() >= 0 && m_currentPos.y() < m_nChessFalls &&
            m_gameModel.At(m_currentPos.x(), m_currentPos.y()) == 0)
        {
            QPainterPath path;
            path.addEllipse(m_kBoardMargin + m_kBlockSize * m_currentPos.y() - m_kRadius,
                            m_kBoardMargin + m_kBlockSize * m_currentPos.x() - m_kRadius,
                            m_kRadius * 2, m_kRadius * 2);
            QPainterPathStroker stroker;
            stroker.setJoinStyle(Qt::RoundJoin);
            stroker.setDashPattern(Qt::DashLine);
            stroker.setWidth(1);
            QPainterPath outlinePath = stroker.createStroke(path);
            pen = painter.pen();
            painter.setPen(QColor(255, 0, 0));
            painter.drawPath(outlinePath);
        }
    }

    painter.setPen(pen);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawEllipse(m_kBoardMargin + m_kBlockSize * 7 - m_kMarkSize / 2, m_kBoardMargin + m_kBlockSize * 7 - m_kMarkSize / 2, m_kMarkSize, m_kMarkSize);
    painter.drawEllipse(m_kBoardMargin + m_kBlockSize * 3 - m_kMarkSize / 2, m_kBoardMargin + m_kBlockSize * 3 - m_kMarkSize / 2, m_kMarkSize, m_kMarkSize);
    painter.drawEllipse(m_kBoardMargin + m_kBlockSize * 11 - m_kMarkSize / 2, m_kBoardMargin + m_kBlockSize * 3 - m_kMarkSize / 2, m_kMarkSize, m_kMarkSize);
    painter.drawEllipse(m_kBoardMargin + m_kBlockSize * 3 - m_kMarkSize / 2, m_kBoardMargin + m_kBlockSize * 11 - m_kMarkSize / 2, m_kMarkSize, m_kMarkSize);
    painter.drawEllipse(m_kBoardMargin + m_kBlockSize * 11 - m_kMarkSize / 2, m_kBoardMargin + m_kBlockSize * 11 - m_kMarkSize / 2, m_kMarkSize, m_kMarkSize);

    // 绘制棋子
    for (int i = 0; i < m_nChessFalls; i++)
        for (int j = 0; j < m_nChessFalls; j++)
        {
            if (m_gameModel.At(i, j) == 1)
            {
                QPixmap pic(":/Image/IMG/white_pieces.png");
                painter.drawPixmap(m_kBoardMargin + m_kBlockSize * j - m_kRadius, m_kBoardMargin + m_kBlockSize * i - m_kRadius, m_kRadius * 2, m_kRadius * 2, pic);
            }
            else if (m_gameModel.At(i, j) == -1)
            {
                QPixmap pic(":/Image/IMG/black_pieces.png");
                painter.drawPixmap(m_kBoardMargin + m_kBlockSize * j - m_kRadius, m_kBoardMargin + m_kBlockSize * i - m_kRadius, m_kRadius * 2, m_kRadius * 2, pic);
            }
        }

    if(m_gameModel.GetTipType() != NONEIMG)
    {
        if(m_gameModel.GetTipType() == READYIMG)
            painter.drawPixmap(180, 180, 100, 80, QPixmap(":/Image/IMG/ready.png"));
        else if(m_gameModel.GetTipType() == WINIMG)
            painter.drawPixmap(180, 180, 100, 80, QPixmap(":/Image/IMG/win.png"));
        else if(m_gameModel.GetTipType() == LOSEIMG)
            painter.drawPixmap(180, 180, 100, 80, QPixmap(":/Image/IMG/lose.png"));
    }
}

void CheeseBoardWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_gameModel.MyTurn() == false && m_gameModel.GetGameStatus() != PLAYING)
        return;

    int x = event->x();
    int y = event->y();

    // 棋盘边缘不能落子
    if (x >= m_kBoardMargin && x <= width() - m_kBoardMargin &&
        y >= m_kBoardMargin && y <= height()- m_kBoardMargin)
    {
        // 获取最近的左上角的点
        int col = abs(x - m_kBoardMargin) / m_kBlockSize;
        int row = abs(y - m_kBoardMargin) / m_kBlockSize;

        int leftTopPosX = m_kBoardMargin + m_kBlockSize * col;
        int leftTopPosY = m_kBoardMargin + m_kBlockSize * row;

        m_currentPos = {-1, -1};

        if((x - leftTopPosX) < m_kPosDelta && (y - leftTopPosY) < m_kPosDelta)
        {
            m_currentPos = {row, col};
        }
        else if((x - leftTopPosX) > m_kPosDelta && (y - leftTopPosY) < m_kPosDelta)
        {
            m_currentPos = {row, col + 1};
        }
        else if((x - leftTopPosX) < m_kPosDelta && (y - leftTopPosY) > m_kPosDelta)
        {
            m_currentPos = {row + 1, col};
        }
        else
        {
            m_currentPos = {row + 1, col + 1};
        }
    }

    update();
}

void CheeseBoardWidget::mousePressEvent(QMouseEvent *event)
{
    if(m_gameModel.MyTurn() == true  && m_gameModel.GetGameStatus() == PLAYING)
    {
        // 人下棋
        if (m_gameModel.GetGameType() == COMPUTER)
        {
            ChessOneByPerson();
            // AI下棋
            if (m_gameModel.MyTurn() == false)
                ChessOneByAI();
        }
        else if(m_gameModel.GetGameType() == HUMAN)
            ChessOneByPerson();
    }
}

void CheeseBoardWidget::ChessOneByPerson()
{
    // 只有有效点击才下子，并且该处没有子
    if (CheckCurPntPositive() && m_gameModel.At(m_currentPos.x(), m_currentPos.y()) == 0)
    {
        m_gameModel.ActionByHuman(m_currentPos.x(), m_currentPos.y());
        QSound::play(CHESS_ONE_SOUND);
        update();
    }
}

void CheeseBoardWidget::ChessOneByAI()
{
    m_currentPos = m_gameModel.ActionByAI();
    QSound::play(CHESS_ONE_SOUND);
    update();
}

bool CheeseBoardWidget::isInChessBoard()
{
    if(m_currentPos.x() > 0 && m_currentPos.y() < m_kBoardSizeNum &&
       m_currentPos.y() > 0 && m_currentPos.y() < m_kBoardSizeNum )
        return true;
    return false;
}
