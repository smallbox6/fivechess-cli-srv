#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QMouseEvent>

class PortraitWidget : public QWidget
{
    Q_OBJECT

public:
    PortraitWidget(QWidget *parent);
    ~PortraitWidget(){}
    void LogOn(bool blogon);

signals:
    void sig_Clicked();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    bool m_blogOn;
};

class TopWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TopWidget(QWidget *parent = 0);
    ~TopWidget();

    void LogOn(bool blogon);
    void SetWindowsSizeAndPos(int width, int height);

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent*event);
    void mousePressEvent(QMouseEvent*event);

signals:
    void sig_CloseWindow(bool);
    void sig_MinWindow();

private slots:
    void slot_CloseWindow();
    void slot_MinWindow();
    void slot_ShowWindow();

private:
    QPushButton *m_pCloseBtn;
    QPushButton *m_pMinBtn;
    QPushButton *m_pStartBtn;
    PortraitWidget *m_pPortaitBtn;
    //QCheckBox    *m_pCheckBox;
    bool m_isLogon;
    QPoint m_movePos;
};

#endif // FORM_H
