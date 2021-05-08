#include "join.h"
#include "ui_join.h"
#include "setting.h"

#include<QPushButton>
Join::Join(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Join)
{
    //预定义参数设置
    DEBUG_MODEL = true;
    AVATAR = "";
    STYLE_COLOR = "default";
    SERVER_TYPE = "hack.chat";
    TIME_OUT_SECOUND = 30000;
    SERVER_URL = "ws://123.56.105.26:6060/chat-ws";
    ui->setupUi(this);
//    SERVER_URL = "wss://hack.chat/chat-ws";

    initConfigFile();


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
//    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    loadStyleColor(STYLE_COLOR);

    //会话窗口设置
    ui->onLogin->hide();
    if(AVATAR.isNull() || AVATAR.isEmpty()) {
        ui->avatar->setStyleSheet("border-image: url(:/ico/unknown);");
    }else {
        ui->avatar->setStyleSheet("border-image: url(./icon/"+AVATAR+");");
        qDebug()<<"border-image: url(./icon/"+AVATAR+");";
    }

    if(!DEBUG_MODEL) {
        ui->textBrowser->hide();
    }

    //自动登陆：
    if(!set->value("auto_login").toString().isEmpty()) {
        if(set->value("auto_login").toString().toLower() == "true"){
            ui->auto_login->setChecked(true);
            ui->nick->setText(set->value("default_nick").toString());
            on_login_clicked();
        }
    }
}

Join::~Join()
{
    delete ui;
}
/**
 * @brief Join::initConfigFile 初始化配置文件
 */
void Join::initConfigFile()
{
    QString configFilePath = QCoreApplication::applicationDirPath()+"/config.ini";
    //初始化配置文件
    QFileInfo *configFileInfo = new QFileInfo(configFilePath);
    if(!configFileInfo->isFile()) {
        createNewConfig(configFileInfo);
        set = new QSettings(QCoreApplication::applicationDirPath()+"/config.ini", QSettings::IniFormat);
        return;
    }
    set = new QSettings(QCoreApplication::applicationDirPath()+"/config.ini", QSettings::IniFormat);
    if(!set->value("debug_model").toString().isEmpty()) {
        if(set->value("debug_model").toString().toLower()=="true"){
            DEBUG_MODEL = true;
        }else {
            DEBUG_MODEL = false;
        }
    }

    if(!set->value("nsever_url").toString().isEmpty()) {
        SERVER_URL = set->value("sever_url").toString();
    }

    if(!set->value("avatar").toString().isEmpty()) {
        AVATAR = set->value("avatar").toString();
    }

    if(!set->value("style_color").toString().isEmpty()) {
        QString color = set->value("style_color").toString().toLower();
        if(color=="default"||color == "darkblue" ||color == "black"||color == "pink"){
            STYLE_COLOR = set->value("style_color").toString();
        }
    }

    if(!set->value("server_type").toString().isEmpty()) {
        SERVER_TYPE = set->value("server_type").toString().toLower();
    }

    if(!set->value("default_room").toString().isEmpty()) {
        ui->rember_room->setChecked(true);
        ui->room->setText(set->value("default_room").toString());
    }
}

void Join::createNewConfig(QFileInfo *f)
{
    QFile *newConfig = new QFile(f->filePath());
    newConfig->open(QIODevice::ReadWrite|QIODevice::Text);
    QString newConfigFile = "[General]\nstyle_color=default\nsever_url="+SERVER_URL+"\navatar="+AVATAR+"\ntime_out="+QString::number(TIME_OUT_SECOUND)+"\nserver_type="+SERVER_TYPE+"\n";
    newConfig->write(newConfigFile.toUtf8());
    newConfig->close();
}

void Join::loadStyleColor()
{
    this->loadStyleColor(this->STYLE_COLOR);
}

/**
 * @brief Join::loadStyleColor 更新主题色
 * @param color 主题色
 */
void Join::loadStyleColor(QString color)
{
    if(color == "default"){
        color = "63, 188, 242";
    }else if (color == "darkblue") {
        color = "0, 85, 127";
    }else if (color == "black") {
        color = "0, 0, 0";
    }else if (color == "pink") {
        color = "251, 114, 153";
    }else {
        return;
    }

    ui->label->setStyleSheet("color: rgb("+color+", 100);");

    ui->nick->setStyleSheet("QLineEdit{\nbackground-color: rgb(255, 255, 255,0);\noutline: none;\nborder: none;\nborder-bottom: 2px solid rgb(100, 100, 100);\nselection-background-color: rgb("+color+");\n}\nQLineEdit:hover{\nborder-bottom: 2px solid rgb(20, 20, 20);\n}\nQLineEdit:focus{\nborder-bottom: 2px solid rgb("+color+");\n}");
    ui->room->setStyleSheet("QLineEdit{\nbackground-color: rgb(255, 255, 255,0);\noutline: none;\nborder: none;\nborder-bottom: 2px solid rgb(100, 100, 100);\nselection-background-color: rgb("+color+");\n}\nQLineEdit:hover{\nborder-bottom: 2px solid rgb(20, 20, 20);\n}\nQLineEdit:focus{\nborder-bottom: 2px solid rgb("+color+");\n}");

    if(color == "63, 188, 242"){
        ui->login->setStyleSheet("QPushButton{\nbackground-color: rgb(63, 188, 242);\ncolor: #ffffff;\nborder:0;\n}\nQPushButton:hover{\nbackground-color: rgb(123, 207, 242);\ncolor: #ffffff;\nborder:0;\n}\nQPushButton:pressed{\nbackground-color: rgb(33, 180, 242);\ncolor: #ffffff;\n}");
        color = "0, 85, 127";
    }else {
        ui->login->setStyleSheet("QPushButton{\nbackground-color: rgb("+color+");\ncolor: #ffffff;\nborder:0;\n}\nQPushButton:hover{\nbackground-color: rgb("+color+",200);\ncolor: #ffffff;\nborder:0;\n}\nQPushButton:pressed{\nbackground-color: rgb("+color+");\ncolor: #ffffff;\n}");
    }
    ui->back_1->setStyleSheet("background-color: rgb("+color+");");
    ui->icon_back->setStyleSheet("background-color: rgb("+color+");");
}

void Join::closeEvent(QCloseEvent *event)
{
    if(closeSocket) socket->abort();
//    if(ui->auto_login->isChecked()){
//        set->setValue("auto_login","true");
//    }else {
//        set->setValue("auto_login","false");
//    }
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
    if(ui->room->text().remove(" ").isEmpty() || ui->nick->text().remove(" ").isEmpty()) {
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

        //存储登陆信息
        if(ui->rember_room->isChecked()) {
            set->setValue("default_room",ui->room->text());
        }
        if(ui->auto_login->isChecked()){
            set->setValue("auto_login","true");
            set->setValue("default_nick",ui->nick->text());
        }else {
            set->setValue("auto_login","false");
        }

        Session *session = new Session();
        session->init(SERVER_URL,ui->nick->text(),AVATAR,ui->room->text(),STYLE_COLOR,SERVER_TYPE,message);
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

void Join::on_set_clicked()
{
    Setting *w = new Setting();
    w->initParentWidget(this,"join");
    w->show();
}

void Join::on_auto_login_clicked(bool checked)
{
    if(checked){
        ui->rember_room->setChecked(true);
    }
}
