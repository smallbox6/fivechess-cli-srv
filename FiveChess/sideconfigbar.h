#ifndef SIDECONFIGBAR_H
#define SIDECONFIGBAR_H

#include "common.h"

#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>

class SideConfigBar : public QWidget
{
    Q_OBJECT
public:
    explicit SideConfigBar(QWidget *parent = nullptr);
    virtual ~SideConfigBar();

    inline bool GetFightonline() {return m_bFightOnline;}
    inline bool GetPiecesColor() {return m_bPiecesColor;}

    bool IsShow(){return m_isShow;}
    void SetShowType(bool bhide) { m_isShow = bhide; slot_ShowSideBar();}

    void SetGameType(GameType type);
    void SetPiecesColor(int color);

    void SetRadiosEnable(bool active);

protected:
    void paintEvent(QPaintEvent *event);
    //void focusOutEvent (QFocusEvent *event);

signals:

public slots:
    void slot_ShowSideBar();
    void slot_PiecesBtnClk(int id);
    void slot_FightModeBtnClk(int id);

private:
    void HideBigWidget();
    void ShowBigWidget();

private:
    bool m_isShow;
    bool m_bFightOnline;
    bool m_bPiecesColor;

    QPushButton *m_pSideBtn;
    QCheckBox   *m_pBackGrndMsc;

    QLabel         *m_pPiecesLabel;
    QButtonGroup   *m_pPiecesGroup;
    QRadioButton   *m_pWhiteRadio;
    QRadioButton   *m_pBlackRadio;

    QLabel         *m_pFightModeLabel;
    QButtonGroup   *m_pFightModeGroup;
    QRadioButton   *m_pComRadio;
    QRadioButton   *m_pNetRadio;
};

#endif // SIDECONFIGBAR_H
