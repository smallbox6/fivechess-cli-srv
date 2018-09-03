#ifndef ONLINEPLAYERLIST_H
#define ONLINEPLAYERLIST_H

#include "common.h"

#include <QListWidget>
#include <QMenu>
#include <QMouseEvent>
#include <QLineEdit>
#include <QStyledItemDelegate>
#include <QIcon>
#include <QLabel>
#include <QPainter>

class PlayerOnLineList;

class MyListWidget : public QListWidget
{
    Q_OBJECT
public:
    MyListWidget(QWidget *parent);
    ~MyListWidget();

    inline void SetMatchValue(bool bmatch){m_bSendMatch = bmatch;}
    inline void SetParentWig(PlayerOnLineList *pa) { m_parentWig = pa;}

public slots:
    void slot_PlayingWith();
    void slot_RandomPlayingWith();

protected:
    void contextMenuEvent(QContextMenuEvent *event);
private:
    QAction *m_pRandomAtc;
    QAction *m_pInviteAtc;
    PlayerOnLineList *m_parentWig;
    bool m_bSendMatch;
};

//////////////////////////////////////////////////
class PlayerOnLineList: public QWidget
{
    Q_OBJECT
public:
    PlayerOnLineList(QWidget *parent = nullptr);
    void addItem(QByteArray name, bool bgameing);
    QByteArray GetCountName(int row);
    void UpdateItems(std::vector<OnlinePlayer> &players);
    inline void SetMatchValue(bool bmatch){m_pListWidget->SetMatchValue(bmatch);}

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:

private:
    MyListWidget *m_pListWidget;
    QVector<QByteArray> m_playerStr;
};

#endif // ONLINEPLAYERLIST_H
