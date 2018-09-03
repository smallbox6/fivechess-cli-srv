#ifndef NOTICEWIDGET_H
#define NOTICEWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QPixmap>
#include <QPushButton>

class NoticeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NoticeWidget(QWidget *parent = nullptr);
    ~NoticeWidget();

    void SetPiecePixmap(int ncolor);
    inline void AddInfo(QString str) {m_pNoticeText->append(str);}

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:
    void slot_StartGame();
    void slot_Exit();

private:
    QTextEdit *m_pNoticeText;
    bool m_bPieceColor;
    QPixmap m_pixmap;

    QPushButton *m_pOkbtn;
    QPushButton *m_pCancelBtn;
};

#endif // NOTICEWIDGET_H
