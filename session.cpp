#include "session.h"
#include "ui_session.h"

#include <QDebug>

QWebSocket *socket = new QWebSocket();

Session::Session(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Session)
{
    ui->setupUi(this);

    /* 创建信号槽连接 */
    connect(socket,&QWebSocket::disconnected,this,&Session::socketDisconnected,Qt::AutoConnection);
    connect(socket,&QWebSocket::connected,this,&Session::socketConnected,Qt::AutoConnection);
    connect(socket,&QWebSocket::textMessageReceived,this,&Session::socketTextMessageReceived,Qt::AutoConnection);

    /* 加载Web框架资源 */
    QFileInfo *index = new QFileInfo("./html/index.html");
    ui->WebDisplay->setUrl(QUrl("file:///"+index->absoluteFilePath()));
//    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9223");
//    qDebug()<<"file:///"+index->absoluteFilePath();

    /* 创建Web连接桥 */
    bridge = new class bridge(this);
    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject("bridge", (QObject*)bridge);
    ui->WebDisplay->page()->setWebChannel(channel);
}

Session::~Session()
{
    delete ui;
}

void Session::file()
{
    ui->WebDisplay->page()->runJavaScript("newMessage(unescape('%5B%u6587%u4EF6%u4F20%u8F93%5D%20%u529F%u80FD%u5DF2%u9501%u5B9A'),0)");
}

void Session::init(QString server, QString nick, QString avatar,QString room,QString loginMessage)
{
    this->server = server;
    if(nick.contains("#")){
        this->nick = nick.mid(0,nick.indexOf("#"));
        qDebug()<<this->nick;
    }else {
        this->nick = nick;
    }
    this->avatar = avatar;
    this->room = room;

    this->setWindowTitle(tr("SChat") + " | "+ tr("频道:")+room);

    //登陆成功，解析登陆成功信息
    Q_UNUSED(loginMessage);
    onlineSet(loginMessage);
    on_lineEdit_textChanged("");
}

void Session::initWeb()
{
    if(avatar.isNull() || avatar.isEmpty()) {
        ui->WebDisplay->page()->runJavaScript("init('"+server+"','"+nick+"')");
    }else {
        ui->WebDisplay->page()->runJavaScript("init('"+server+"','"+nick+"','"+avatar+"')");
    }

//    ui->WebDisplay->page()->runJavaScript("userMessage(messageType.Other,unescape('%u770B%u89C1%u597D%u5904%u5C31%u8981%u62A2'),unescape('%u96F7%u72EE'),'../icon/leishi.jpg');");
//    QString user = "雷狮";
//    QString message = "测试消息";
//    ui->WebDisplay->page()->runJavaScript("userMessage(messageType.Other,'"+message+"','"+user+"','../icon/leishi.jpg');");
}

void Session::sendMessage(QString message)
{
    QJsonObject *sendmessage = new QJsonObject();
    sendmessage->insert("cmd","chat");
    sendmessage->insert("text",message);
    QJsonDocument jsonDoc;
    jsonDoc.setObject(*sendmessage);
    socket->sendTextMessage(jsonDoc.toJson());
}

void Session::closeEvent(QCloseEvent *event)
{
    socket->abort();
    ui->WebDisplay->close();
    event->accept();
}

void Session::systemMessage(QString message)
{
    ui->WebDisplay->page()->runJavaScript("systemMessage('"+message+"')");
}

void Session::myMessage(QString message)
{
    QString js = "userMessage(1,'"+message+"','"+nick+"'";
    if(!avatar.isNull() && !avatar.isEmpty()){
        js += ",'../icon/"+avatar+"'";
    }
    js += ");";
    ui->WebDisplay->page()->runJavaScript(js);
}

void Session::otherMessage(QString message, QString otherNick, QString otherAvatar)
{
    QString js = "userMessage(2,'"+message+"','"+otherNick+"'";
    if(!otherAvatar.isNull() && !otherAvatar.isEmpty()){
        js += ",'../icon/"+otherAvatar+"'";
    }
    js += ");";
    ui->WebDisplay->page()->runJavaScript(js);
}

void Session::onlineSet(QString json)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject response = jsonDoc.object();

    //验证数据合法性
    if(response["cmd"].toString() != "onlineSet") {
        return;
    }

    //开始解析全部信息
    QJsonArray users = response["users"].toArray();
    for(QJsonValueRef userRef:users) {
       QJsonObject user = userRef.toObject();
       qDebug()<<user;
       User *thisUser = new User();
       thisUser->id = QString::number(user.take("userid").toDouble());
       thisUser->nick = user["nick"].toString();
       thisUser->trip = user["trip"].toString();
       thisUser->utype = user["utype"].toString();
       thisUser->hash = user["hash"].toString();
       thisUser->isMe = user["isme"].toBool(false);
       this->users.append(*thisUser);
    }

    //刷新联系人页面
    on_lineEdit_textChanged("");
}

void Session::addUserToList(QJsonObject json)
{
    User *user = new User();
    user->nick = json["nick"].toString();
    user->trip = json["trip"].toString();
    user->utype = json["utype"].toString();
    user->hash = json["hash"].toString();
    user->id = json["userid"].toString();
    user->isMe = false;
    users.append(*user);
    on_lineEdit_textChanged("");
}

void Session::removeUserByNick(QString nick)
{
    for(int i = 0; i < users.length() ;i++){
        if(users.at(i).nick == nick){
            users.removeAt(i);
            on_lineEdit_textChanged("");
            return;
        }
    }
}

User Session::getUserByNick(QString nick)
{
    if(nick == nullptr) return User();
    for(int i = 0; i < users.length() ;i++){
        if(users.at(i).nick == nick){
            return users.at(i);
        }
    }
    return User();
}
void Session::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ui->WebDisplay->page()->runJavaScript("atOther('"+item->text()+"')");
}

void Session::socketConnected()
{
    systemMessage("成功与服务器建立连接");
}

void Session::socketDisconnected()
{
    systemMessage("丢失与服务器的连接...");
}

void Session::socketTextMessageReceived(QString message)
{
    QStringList cmdList;
    cmdList<<"chat"
          <<"info"
         <<"onlineSet"
        <<"onlineAdd"
       <<"onlineRemove";

    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject response = jsonDoc.object();

    switch (cmdList.indexOf(response["cmd"].toString())) {
    case 0:
        /*Chat*/
        if(response["nick"].toString() == nick) {
            myMessage(response["text"].toString());
        }else {
            otherMessage(response["text"].toString(),response["nick"].toString(),NULL);
        }
        return;
    case 1:
        /*Info*/
        systemMessage(response["text"].toString());
        return;
    case 2:
        /*OnlineSet*/
        onlineSet(message);
        return;
    case 3:
        /*onlineAdd*/
        systemMessage(tr("用户[")+response["nick"].toString()+tr("]加入了聊天室"));
        addUserToList(response);
        return;
    case 4:
        /*onlineRemove*/
        systemMessage(tr("用户[")+response["nick"].toString()+tr("]离开了聊天室"));
        removeUserByNick(response["nick"].toString());
        return;
    default:
        qDebug()<<message;
    }

}


void Session::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    //** 将被设计为双击打开个人详情页 **//
    UserInfo *info = new UserInfo();
    info->init(avatar,server,room,users.length());
    qDebug()<<"room"<<room<<"server"<<server;
    User user = getUserByNick(item->text());
    qDebug()<<user.id;
    info->initUser(user.nick,user.hash,user.id,user.utype);
    info->show();
}

void Session::on_lineEdit_textChanged(const QString &arg1)
{
    ui->listWidget->clear();
    for(User user:users) {
        if(user.nick.contains(arg1)){
            QListWidgetItem *item = new QListWidgetItem();
            item->setText(user.nick);
            if(user.icon != NULL) {
                item->setIcon(QIcon(user.icon));
            }else {
                item->setIcon(QIcon(":/ico/unknown"));
            }
            ui->listWidget->addItem(item);
        }
    }
}

void Session::on_actionempty_triggered()
{
    QMessageBox::StandardButton button;
    button=QMessageBox::question(this,tr("提示"),QString(tr("是否 [清空] 当前页面上的记录? \n未保存的记录无法找回")),QMessageBox::Yes|QMessageBox::No);
    if(button==QMessageBox::Yes){
        ui->WebDisplay->page()->runJavaScript("empty()");
    }
}

void Session::on_actionabout_triggered()
{
    //关于 "SChat"
}
