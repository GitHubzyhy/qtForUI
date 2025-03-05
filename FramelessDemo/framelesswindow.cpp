#include "framelesswindow.h"

#include <QApplication>
#include <QMouseEvent>
#include <QDebug>

FramelessWindow::FramelessWindow(QWidget *parent)
    : QWidget{parent}
{
    this->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    leftPressed = false;
    leftPressedInTitle = false;

    this->installEventFilter(this);
    // 拖动缩放
    hoverPos = CENTER;
    padding = 4;

    onlyMoveByTitlebar = true;// 默认只允许拖动标题栏，来移动窗口
    canResize = true;// 默认允许窗口缩放
}

void FramelessWindow::setTitlebarWidgets(QVector<QWidget *> widgets)
{
    titlebarWidgets = widgets;
}

//用于设置鼠标追踪
void FramelessWindow::setAllWidgetMouseTracking(QWidget *widget)
{
    widget->setMouseTracking(true);

    //给子控件也设置鼠标追踪
    QObjectList list = widget->children();
    foreach (QObject *obj, list)
    {
        // qDebug() << obj->metaObject()->className();
        if ( obj->metaObject()->className() == QStringLiteral("QWidget") )
        {
            QWidget *w = (QWidget *)obj;
            w->setMouseTracking(true);
            setAllWidgetMouseTracking(w);//递归调用
        }
    }
}

//设置光标在边界的形状
void FramelessWindow::setCursorShape(const QPoint &point)
{
    QRect rect = this->rect();
    QPoint topLeft = mapToGlobal(rect.topLeft());
    QPoint bottomRight = mapToGlobal(rect.bottomRight());

    int x = point.x();
    int y = point.y();

    if ( x >= topLeft.x() && x <= topLeft.x() + padding && y >= topLeft.y() && y <= topLeft.y() + padding )
    {
        // 左上角
        hoverPos = TOP_LEFT;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else if ( x <= bottomRight.x() && x >= bottomRight.x() - padding && y <= bottomRight.y() && y >= bottomRight.y() - padding )
    {
        // 右下角
        hoverPos = BOTTOM_RIGHT;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    }
    else if ( x >= topLeft.x() && x <= topLeft.x() + padding && y <= bottomRight.y() && y >= bottomRight.y() - padding )
    {
        // 左下角
        hoverPos = BOTTOM_LEFT;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if ( x <= bottomRight.x() && x >= bottomRight.x() - padding && y >= topLeft.y() && y <= topLeft.y() + padding )
    {
        // 右上角
        hoverPos = TOP_RIGHT;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if ( x >= topLeft.x() && x <= topLeft.x() + padding )
    {
        // 左边
        hoverPos = LEFT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if ( x <= bottomRight.x() && x >= bottomRight.x() - padding )
    {
        // 右边
        hoverPos = RIGHT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if ( y >= topLeft.y() && y <= topLeft.y() + padding )
    {
        // 上边
        hoverPos = TOP;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else if ( y <= bottomRight.y() && y >= bottomRight.y() - padding )
    {
        // 下边
        hoverPos = BOTTOM;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else
    {
        // 中间
        hoverPos = CENTER;
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}

//设置标题栏之外是否可以拖动
void FramelessWindow::setOnlyMoveByTitlebar(bool b)
{
    onlyMoveByTitlebar = b;
}

//设置是否允许缩放
void FramelessWindow::setResizeable(bool b)
{
    canResize = b;
}

bool FramelessWindow::getResizeable()
{
    return canResize;
}

void FramelessWindow::mousePressEvent(QMouseEvent *event)
{
    // 如果不是左键，直接返回
    if (event->button() != Qt::LeftButton) return;

    leftPressed = true;

    wndPos = this->pos();// 记录当前窗体的位置，也就是窗体左上角的坐标
    pressPos = event->globalPos(); // 记录鼠标按下的位置

    // 判断左键按下的位置是否在标题栏中
    QWidget *pressedWidget = QApplication::widgetAt(pressPos);
    if ( pressedWidget )
    {
        foreach (QWidget *widget, titlebarWidgets)
        {
            if ( pressedWidget == widget )
            {
                leftPressedInTitle = true;
                break;
            }
        }
    }
}

void FramelessWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint globalPos = event->globalPos();

    // 1. 左键未按下
    if ( !leftPressed )
    {
        // 窗口不是最大化状态，则光标移动到边界时，要变成缩放的形状（窗口处于最大化状态时，就无须改变光标形状了）
        if ( this->windowState().testFlag(Qt::WindowNoState) && canResize)
            setCursorShape(globalPos);
        return;
    }

    if (hoverPos != CENTER)
    {
        // 2.1 在边界处按下
        QRect rect = this->rect();
        QPoint topLeft = mapToGlobal(rect.topLeft());
        QPoint bottomRight = mapToGlobal(rect.bottomRight());

        QRect rMove(topLeft, bottomRight);
        switch ( hoverPos )
        {
            case TOP:
                // 如果不加if判断，则窗口高度达到最小高度后，会被鼠标 "向下推走"
                if ( bottomRight.y() - globalPos.y() > this->minimumHeight() )
                    rMove.setY(globalPos.y());
                break;
            case BOTTOM:
                rMove.setHeight(globalPos.y() - topLeft.y());
                break;
            case LEFT:
                // 如果不加if判断，则窗口高度达到最小宽度后，会被鼠标 "向右推走"
                if ( bottomRight.x() - globalPos.x() > this->minimumWidth() )
                    rMove.setX(globalPos.x());
                break;
            case RIGHT:
                rMove.setWidth(globalPos.x() - topLeft.x());
                break;
            case TOP_LEFT:
                if ( bottomRight.y() - globalPos.y() > this->minimumHeight() )
                    rMove.setY(globalPos.y());
                if ( bottomRight.x() - globalPos.x() > this->minimumWidth() )
                    rMove.setX(globalPos.x());
                break;
            case TOP_RIGHT:
                if ( bottomRight.y() - globalPos.y() > this->minimumHeight() )
                    rMove.setY(globalPos.y());
                rMove.setWidth(globalPos.x() - topLeft.x());
                break;
            case BOTTOM_LEFT:
                rMove.setHeight(globalPos.y() - topLeft.y());
                if ( bottomRight.x() - globalPos.x() > this->minimumWidth() )
                    rMove.setX(globalPos.x());
                break;
            case BOTTOM_RIGHT:
                rMove.setHeight(globalPos.y() - topLeft.y());
                rMove.setWidth(globalPos.x() - topLeft.x());
                break;
            default:
                break;
        }

        this->setGeometry(rMove);//重新设置窗体的大小
    }
    else
    {
        // 2.2 在标题栏内按下
        if ( leftPressedInTitle )
        {
            if ( this->isMaximized() )//是否是最大化
            {
                // 窗口最大化时鼠标拖动标题栏需要完成两个操作：
                // a. 窗口恢复
                // b. 鼠标相对窗口的位置不变
                // 相对位置不变指的是：鼠标点击拖动窗口1/4处进行拖动，复原时鼠标依然位于窗口1/4处
                // 达到此效果，仅需更改窗口的位置即可

                // 计算全屏时，鼠标在 x 轴上，相对于屏幕宽度的百分比
                float width_ratio = float(globalPos.x()) / float(this->width());
                qDebug() << "ratio=" << width_ratio;
                qDebug() << "before width = " << this->width(); // 1920

                // a. 窗口恢复
                this->setWindowState(Qt::WindowNoState);
                QCoreApplication::processEvents(); // 强制处理事件循环
                qDebug() << "after width = " << this->width(); // 600

                // b. 鼠标相对窗口的相对位置不变
                // 和双击时一样，默认会回到最大化之前的位置，所以要修改窗口的位置
                int offset = this->width() * width_ratio; // 当前窗口相对于鼠标位置的偏移
                wndPos.setX(globalPos.x() - offset);//改变窗体左上角的坐标
                wndPos.setY(0);
            }
            else
                this->move(wndPos + (event->globalPos() - pressPos));
        }
        else
        {
            //在主体内按下
            if ( !onlyMoveByTitlebar && !this->isMaximized() )
                this->move(wndPos + (event->globalPos() - pressPos));
        }
    }
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    leftPressed = false;
    leftPressedInTitle = false;
}

// 双击标题栏放大
void FramelessWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if ( canResize )
    {
        QWidget *clickedWidget = QApplication::widgetAt(event->globalPos());
        // qDebug() << "clickedWidget: " << clickedWidget << ", " << event->globalPos();
        if ( clickedWidget )
        {
            foreach (QWidget *widget, titlebarWidgets)
            {
                if ( clickedWidget == widget )
                {
                    this->setWindowState(isMaximized() ? Qt::WindowNoState : Qt::WindowMaximized);
                    break;
                }
            }
        }
    }
}

/*  当前窗口绘制时，给窗口中的所有控件设置鼠标追踪。
    这样鼠标在移动到边界时，可以追踪得到，从而设置光标为缩放形状
*/
bool FramelessWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Paint)
    {
        static bool init = false;
        if (!init)
        {
            init = true;
            setAllWidgetMouseTracking(this);
        }
    }

    // 查看源码可知，父类的实现就是 return false，表示让事件接着传递，也就是传递给对应的控件
    return QWidget::eventFilter(watched, event);
}
