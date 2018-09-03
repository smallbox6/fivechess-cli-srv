#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "TopWidget.h"
#include "cheeseboardwidget.h"
#include "PlayerOnLineList.h"
#include "chatwidget.h"
#include "sideconfigbar.h"
#include "popdlg.h"
#include "noticewidget.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class BackGroundWidget : public QWidget
{
    Q_OBJECT

public:
    BackGroundWidget(QWidget *parent):QWidget(parent){}
    ~BackGroundWidget(){}

protected:
    void paintEvent(QPaintEvent *event);
};

//////////////////////////////////////////
class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    explicit MainWindow(QWidget *parent = 0);

public:
    ~MainWindow();

    static MainWindow* getMainWindow()
    {
        static MainWindow m;
        return &m;
    }

protected:
    bool eventFilter(QObject *, QEvent *);

private slots:
    void slot_CloseWindow();
    void slot_MinWindow();

private:
    TopWidget *m_pTopWidget;
    CheeseBoardWidget *m_pChessboard;
    BackGroundWidget *m_pBackgroundWidget;
    PlayerOnLineList *m_pPlayerOnLineList;
    ChatWidget       *m_pChatWidget;
    SideConfigBar   *m_pSideBar;
    PopDlg    *m_ppopDlg;
    NoticeWidget *m_pNoticeWidget;
};

#endif // MAINWINDOW_H
