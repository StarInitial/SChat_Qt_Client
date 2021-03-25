#include "join.h"
#include "ui_join.h"

#include<QPushButton>
Join::Join(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Join)
{
    //预定义参数设置
    DEBUG_MODEL = false;
    AVATAR = "";
    TIME_OUT_SECOUND = 30000;
    SERVER_URL = "ws://123.56.105.26:6060/chat-ws";

//    SERVER_URL = "wss://hack.chat/chat-ws";


    //初始化参数
    socket = new QWebSocket();
    offline = new QTimer();
    closeSocket = true;


    //连接信号槽
    connect(socket,&QWebSocket::disconnected,this,&Join::socketDisconnected,Qt::AutoConnection);
    connect(socket,&QWebSocket::connected,this,&Join::socketConnected,Qt::AutoConnection);
    connect(socket,&QWebSocket::textMessageReceived,this,&Join::socketTextMessageReceived,Qt::AutoConnection);
    connect(offline,&QTimer::timeout,this,&Join::timeOut,Qt::AutoConnection);

    //窗口设置
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    ui->setupUi(this);

    //会话窗口设置
    ui->onLogin->hide();
    if(!AVATAR.isNull() && !AVATAR.isEmpty()) {
        ui->avatar->setStyleSheet("border-image: url(:/ico/unknown);");
    }
    if(!DEBUG_MODEL) {
        ui->textBrowser->hide();
    }
}

Join::~Join()
{
    delete ui;
}

void Join::closeEvent(QCloseEvent *event)
{
    if(closeSocket) socket->abort();
    event->accept();
}

void Join::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)//判断是否是左键点击
    {
        m_mousePressPoint = event->pos();
    }
}

void Join::mouseMoveEvent(QMouseEvent *event)
{
    QPoint tempPoint = event->pos();//获取事件发生位置
    if(ui->login->geometry().contains(tempPoint) || ui->horizontalLayout->geometry().contains(tempPoint)) {
        return;
    }
    QRect selfRect = ui->back_2->geometry();//获取窗口图标相对于父窗体的矩形
    int y = selfRect.y();//获取矩形的高度
    int tempHight = ui->back_2->height() + y;//获取标题栏最下线到父窗体最上线的距离（作为判断对比值）

    if (m_mousePressPoint.y() < tempHight)//对比，确定是否移动。
    {
        int tempX = tempPoint.x() - m_mousePressPoint.x();
        int tempY = tempPoint.y() - m_mousePressPoint.y();
        int nowX = this->x();
        int nowY = this->y();
        nowX += tempX;
        nowY += tempY;
        this->move(nowX, nowY);
    }
}

void Join::mouseReleaseEvent(QMouseEvent *event)
{
    m_mousePressPoint = event->pos();
}

void Join::on_login_clicked()
{
    if(ui->room->text().isEmpty() || ui->nick->text().isEmpty()) {
        QMessageBox::warning(this,"警告","[频道] & [昵称] 不可为空");
        return;
    }
    if(SERVER_URL.isNull() || (!SERVER_URL.startsWith("ws://")&&!SERVER_URL.startsWith("wss://"))){
        QMessageBox::critical(this,"Error","Server address config error");
        return;
    }
    ui->textBrowser->clear();
    ui->textBrowser->append(tr("服务器地址:")+SERVER_URL);
    ui->container->hide();
    ui->onLogin->show();
    socket->open(QUrl(SERVER_URL));
    //3s
    offline->start(TIME_OUT_SECOUND);
}

void Join::socketConnected()
{
    offline->stop();
    QString msg1 = "<成功连接到服务器>";
    ui->textBrowser->append(msg1);
    QJsonObject *request = new QJsonObject();
    request->insert("cmd","join");
    request->insert("channel",ui->room->text());
    request->insert("nick",ui->nick->text());
    QJsonDocument jsonDoc;
    jsonDoc.setObject(*request);
    ui->textBrowser->append(tr("发送加入聊天室请求..."));
    socket->sendTextMessage(jsonDoc.toJson());

}

void Join::socketDisconnected()
{

}

void Join::socketTextMessageReceived(QString message)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject loginRequest = jsonDoc.object();
    QString cmd = loginRequest["cmd"].toString();
    //登陆出现错误
    if(cmd == "warn") {
        loginFail(loginRequest["text"].toString());
        socket->abort();
        return;
    }
    //获取用户列表
    else if (cmd == "onlineSet") {
        //ui->textBrowser->append(message);
        Session *session = new Session();
        session->init(SERVER_URL,ui->nick->text(),AVATAR,ui->room->text(),message);
        session->show();
        closeSocket = false;
        socket->disconnect(this);
        this->close();
    }else{
        qDebug()<<message;
        loginFail("未知错误");
    }
}

void Join::loginFail(QString reason)
{
    QMessageBox::critical(this,"登陆失败",reason);
    ui->container->show();
    ui->onLogin->hide();
}

void Join::timeOut()
{
    offline->stop();
    loginFail("Server connection timed out, please check your network.");
}
