#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include "PlayerOnLineList.h"

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

struct ChatInfo
{
    ChatInfo() {
        lineHeight = 30;
        lineLen = 180;
        height = 0;
        isSelf = true;
        charUrl = QString(":/Image/IMG/me.png");
    }
    QString  chatString;
    QString  charUrl;
    int height;
    int lineLen;
    int lineHeight;
    bool isSelf;
};

class ChatItem : public QWidget
{
    Q_OBJECT

public:
    ChatItem(ChatInfo info, QWidget *parent = nullptr);
    ~ChatItem();

    void SetChatString();

protected:
    void paintEvent(QPaintEvent *event);
    void initComponent();

private:
    ChatInfo m_chatInfo;
    QTextEdit *m_pTextEdit;
};

////////////////////////////////////
class ScrollChat : public QWidget
{
    Q_OBJECT
public:
    explicit ScrollChat(QWidget *parent = 0);
    ~ScrollChat();

    void AddChatInfo(ChatInfo info);

protected:

private:
    void initComponent();

signals:

public slots:

private:
    QVector<ChatInfo> m_chatInfoVec;
    QScrollArea* m_pScrollArea;
    QVBoxLayout* m_pVLayout;
    QWidget  *m_pWidget;
};


///////////////////////
class ChatWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChatWidget(QWidget *parent = nullptr);

    void setWidgetEnable(bool);
    inline ChatWidget *GetSelfPointer() {return this;}
    void SetMsg(QString msg, bool self = false);

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:
    void slot_SenddMsg();

private:
    ScrollChat       *m_pListWidget;
    QTextEdit        *m_pTextEdit;
    QPushButton      *m_pSendBtn;
};

#endif // CHATWIDGET_H
