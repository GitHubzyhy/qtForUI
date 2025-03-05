#include "mywidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QEvent>

MyWidget::MyWidget(QWidget *parent)
    : FramelessWindow(parent)
{
    initForm();
    setTitlebarWidgets({titleWidget, lblIcon, lblTitle});
    setOnlyMoveByTitlebar(false); // 默认只允许拖动标题栏实现移动窗口
    // setResizeable(false);

    // 关联信号槽
    connect(btnClose, &QPushButton::clicked, [this]()
    {
        this->close();
    });

    if (getResizeable())
    {
        connect(btnMax, &QPushButton::clicked, [this]()
        {
            // 切换最大化/正常显示
            this->setWindowState(this->windowState()
                                 .testFlag(Qt::WindowNoState) ? Qt::WindowMaximized : Qt::WindowNoState);
        });
    }
    else
        btnMax->hide();

    connect(btnMin, &QPushButton::clicked, [this]()
    {
        // 注意不要这样写，否则最大化状态下点击最小化按钮，再次恢复时不是最大化状态而是normal状态
        // this->setWindowState(Qt::WindowMinimized);

        // 使用下面的与或非写法，不要改变原来的状态。
        this->setWindowState(windowState() & ~Qt::WindowActive | Qt::WindowMinimized);
    });
}

MyWidget::~MyWidget() {}

void MyWidget::initForm()
{
    // 1. 设置窗口
    // 1.1 设置最小宽高
    this->setMinimumWidth(600);
    this->setMinimumHeight(400);

    // 1.2 设置背景色
    this->setStyleSheet("background:#303030");
    // this->setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    // 1.3 窗体整体采用垂直布局
    QVBoxLayout *v1 = new QVBoxLayout(this);
    v1->setSpacing(0);
    v1->setContentsMargins(0, 0, 0, 0);

    // 1.4 并嵌套一层QWidget
    QWidget *frameWidget = new QWidget(this);
    v1->addWidget(frameWidget);//将widget添加到垂直布局里

    // 1.5 嵌套的QWidget同样采用垂直布局
    QVBoxLayout *layout = new QVBoxLayout(frameWidget);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    // 2. 标题栏
    // 2.1 设置固定高度、背景色
    titleWidget = new QWidget(this);
    titleWidget->setFixedHeight(48);
    titleWidget->setStyleSheet("background-color:rgb(60, 60, 60);");

    // 2.2 标题栏采用水平布局
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget); // 标题栏采用水平布局
    titleLayout->setContentsMargins(8, 8, 8, 8); // 这里必须设置一个间距
    titleLayout->setSpacing(0);

    // 2.3 把标题栏添加到整体窗体布局中。之后就可以向titleLayout中添加控件了
    layout->addWidget(titleWidget);

    // 2.4 添加图标
    lblIcon = new QLabel(this);
    lblIcon->setFixedSize(20, 20);
    lblIcon->setStyleSheet("background-image:url(:/res/title_icon_20.svg);border:none");
    titleLayout->addWidget(lblIcon);

    // 2.5 添加弹簧: 在图标和标题之间增加间距
    QSpacerItem *space = new QSpacerItem(6, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
    titleLayout->addSpacerItem(space);

    // 2.6 添加标题
    lblTitle = new QLabel(this);
    // lblTitle->setText("UI美化实战课程_演示01_无边框窗口");
    lblTitle->setText("完善的无边框窗口");
#if 0
    // 默认地标签在水平方向和垂直方向的策略都是 Preferred。
    // 因此会自动将右侧的最小化/最大化/关闭按钮顶到右侧，不需要增加弹簧
    // QSizePolicy(horizontalPolicy = QSizePolicy::Preferred, verticalPolicy = QSizePolicy::Preferred)
    qDebug() << lblTitle->sizePolicy();
    lblTitle->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    // QSizePolicy(horizontalPolicy = QSizePolicy::Fixed, verticalPolicy = QSizePolicy::Preferred)
    qDebug() << lblTitle->sizePolicy();
#endif
    lblTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lblTitle->setStyleSheet(R"(
            background-color: rgba(255, 255, 255, 0);
            font:18px "Microsoft YaHei";
            color:rgb(200,200,200);
            border:none
        )");
    titleLayout->addWidget(lblTitle);

    // 2.7 添加最小化按钮
    btnMin = new QPushButton(this);
    btnMin->setCursor(QCursor(Qt::PointingHandCursor));
    btnMin->setStyleSheet(R"(
            QPushButton {
                background-image:url(:/res/minimize_20.svg);
                border:none;
                background-repeat:none;
                background-position:center;
            }
            QPushButton:hover {
                background-color:rgb(86, 86, 86);
            }
        )");
    btnMin->setFixedSize(36, 32);
    titleLayout->addWidget(btnMin);

    // 2.8 添加最大化按钮
    btnMax = new QPushButton(this);
    btnMax->setCursor(QCursor(Qt::PointingHandCursor));
    btnMax->setStyleSheet(R"(
            QPushButton {
                background-image:url(:/res/maximize_20_normal.svg);
                border:none;
                background-repeat:none;
                background-position:center;
            }
            QPushButton:hover {
                background-color:rgb(86, 86, 86);
            }
        )");
    btnMax->setFixedSize(36, 32);
    titleLayout->addWidget(btnMax);

    // 2.9 添加关闭按钮
    btnClose = new QPushButton(this);
    btnClose->setCursor(QCursor(Qt::PointingHandCursor));
    btnClose->setStyleSheet(R"(
            QPushButton {
                background-image:url(:/res/close_20.svg);
                border:none;
                background-repeat:none;
                background-position:center;
            }
            QPushButton:hover {
                background-color:rgb(222, 6, 26);
            }
        )");
    btnClose->setFixedSize(36, 32);
    titleLayout->addWidget(btnClose);

    // 3. 主体
    // 3.1 设置背景色
    mainWidget = new QWidget(this);
    mainWidget->setStyleSheet("background:#303030"); // 设置背景颜色

    // 3.2 主体采用水平布局
    QHBoxLayout *mainLayout = new QHBoxLayout(mainWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 3.3 把主体添加到整体窗体布局中。之后就可以向mainLayout中添加控件了
    layout->addWidget(mainWidget); // 将主体添加到布局中

    // 3.4 添加确定和取消按钮
    QPushButton *btn1 = new QPushButton("确定");
    QPushButton *btn2 = new QPushButton("取消");

    mainLayout->addWidget(btn1);
    mainLayout->addWidget(btn2);

    QString style = R"(
        QPushButton {
            background-color: rgb(64, 64, 64);
            font:18px "Microsoft YaHei";
            color:rgb(200,200,200);
            border: 1px solid #707070;
            border-radius: 5px;
            padding: 5px;
        }
        QPushButton:hover {
            background-color: rgb(40, 40, 40);
        }
        QPushButton:pressed {
            background-color: rgb(64, 64, 64);
        }
     )";

    btn1->setStyleSheet(style);
    btn2->setStyleSheet(style);

    connect(btn1, &QPushButton::clicked, this, [ = ] { qDebug() << btn1->text(); });
    connect(btn2, &QPushButton::clicked, this, [ = ] { qDebug() << btn2->text(); });
}

void MyWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        if ( this->windowState().testFlag(Qt::WindowMaximized) )
        {
            btnMax->setStyleSheet(R"(
                QPushButton {
                    background-image:url(:/res/maximize_20_max.svg);
                    border:none;
                    background-repeat:none;
                    background-position:center;
                }
                QPushButton:hover {
                    background-color:rgb(86, 86, 86);
                }
            )");
        }
        else if ( this->windowState().testFlag(Qt::WindowNoState) )
        {
            btnMax->setStyleSheet(R"(
                QPushButton {
                    background-image:url(:/res/maximize_20_normal.svg);
                    border:none;
                    background-repeat:none;
                    background-position:center;
                }
                QPushButton:hover {
                    background-color:rgb(86, 86, 86);
                }
            )");
        }
    }
}
