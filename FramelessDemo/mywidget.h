#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include "FramelessWindow.h"

class QLabel;
class QPushButton;
class MyWidget : public FramelessWindow
{
    Q_OBJECT

public:
    MyWidget(QWidget *parent = nullptr);
    ~MyWidget();

private:
    void initForm();

    // 标题栏
    QWidget *titleWidget;
    QLabel *lblIcon;
    QLabel *lblTitle;
    QPushButton *btnClose;
    QPushButton *btnMax;
    QPushButton *btnMin;

    // 主体
    QWidget *mainWidget;

    // QWidget interface
protected:
    virtual void changeEvent(QEvent *) override;
};
#endif // MYWIDGET_H
