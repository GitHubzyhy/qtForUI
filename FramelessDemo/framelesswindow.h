#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QWidget>

// 针对9个区域，定义了一个枚举变量；
enum Location
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    CENTER
};

class FramelessWindow : public QWidget
{
    Q_OBJECT
public:
    explicit FramelessWindow(QWidget *parent = nullptr);

protected:
    void setTitlebarWidgets(QVector<QWidget *> widgets);
    void setAllWidgetMouseTracking(QWidget *widget); // 用于设置鼠标追踪
    void setCursorShape(const QPoint &point); //设置光标在边界的形状

    void setOnlyMoveByTitlebar(bool b);//设置标题栏之外是否可以拖动
    void setResizeable(bool b);//禁止缩放
    bool getResizeable();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);// 双击标题栏放大
    bool eventFilter(QObject *watched, QEvent *event);

private:
    bool leftPressed; // 左键是否按下
    bool leftPressedInTitle; // 左键是否在标题栏按下
    QVector<QWidget *> titlebarWidgets; // 记录标题栏中的控件

    QPoint pressPos; // 鼠标点击的位置
    QPoint wndPos; // 当前窗体的位置，也就是窗口左上角的坐标

    int padding;
    Location hoverPos;

    bool onlyMoveByTitlebar;//标题栏之外是否可以拖动
    bool canResize;// 窗口可缩放
};

#endif // FRAMELESSWINDOW_H
