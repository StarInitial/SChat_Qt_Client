#include "session.h"
#include "ui_session.h"
#include "setting.h"

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

    ui->WebDisplay->page()->settings()->setAttribute(QWebEngineSettings::ShowScrollBars,false);
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

void Session::init(QString server,QString nick,QString avatar,QString room,QString styleColor,QString serverType,QString loginMessage)
{
    if(!server.isNull()) {
        this->server = server;
    }
    if(!nick.isNull()) {
        if(nick.contains("#")){
            this->nick = nick.mid(0,nick.indexOf("#"));
            qDebug()<<this->nick;
        }else {
            this->nick = nick;
        }
    }
    this->avatar = avatar;
    if(!room.isNull()){
        this->room = room;
    }

    this->styleColor = styleColor;
    this->serverType = serverType;


    this->setWindowTitle(tr("SChat") + " | "+ tr("频道:")+room);

    //登陆成功，解析登陆成功信息
    Q_UNUSED(loginMessage);
    if(!loginMessage.isNull()){
        onlineSet(loginMessage);
    }
    on_lineEdit_textChanged("");

    loadStyleColor(this->styleColor);
}

void Session::at(QString nick, QString msg)
{
    Q_UNUSED(msg);
    QString message = ui->message_edit->text();
    if(message.contains("@"+nick)){
        return;
    }
    ui->message_edit->setText("@"+nick+" "+message);
}

void Session::initWeb()
{
    if(avatar.isNull() || avatar.isEmpty()) {
        ui->WebDisplay->page()->runJavaScript("init('"+server+"','"+nick+"','"+this->serverType+"','"+this->styleColor+"',\"14\",\"sans\")");
    }else {
        ui->WebDisplay->page()->runJavaScript("init('"+server+"','"+nick+"','"+this->serverType+"','"+this->styleColor+"','"+avatar+"')");
    }

    ui->WebDisplay->page()->runJavaScript("systemDate()");

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

void Session::loadStyleColor(QString color)
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

    if(color != "63, 188, 242"){
        ui->listWidget->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba("+color+", 255), stop:1 rgba("+color+", 200));\nborder:0;\ncolor: rgb(255, 255, 255);");
        ui->lineEdit->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba("+color+", 255), stop:1 rgba("+color+", 200));\nborder:0;\ncolor: rgb(255, 255, 255);");
    }
    ui->message_edit->setStyleSheet("QLineEdit{\nbackground-color: rgb(255, 255, 255,0);\ncolor: rgb(0, 0, 0);\noutline: none;\nborder: none;\nborder-bottom: 2px solid rgb(100, 100, 100);\nselection-background-color: rgb("+color+");\n}\nQLineEdit:hover{\nborder-bottom: 2px solid rgb(20, 20, 20);\n}\nQLineEdit:focus{\nborder-bottom: 2px solid rgb("+color+");\n}");
}

void Session::systemMessage(QString message)
{
    ui->WebDisplay->page()->runJavaScript("systemMessage('"+message+"')");
}

void Session::myMessage(QString message)
{
    QString js = "userMessage(1,\""+message.replace("\"","&quot;")+"\",'"+nick+"'";
    if(!avatar.isNull() && !avatar.isEmpty()){
        js += ",'../icon/"+avatar+"'";
    }
    js += ");";
    ui->WebDisplay->page()->runJavaScript(js);
}

void Session::otherMessage(QString message, QString otherNick, QString otherAvatar)
{
    QString js = "userMessage(2,'"+message.replace("\"","&quot;")+"','"+otherNick+"'";
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
       thisUser->id = QString::number(user.take("userid").toDouble(),'f',0);
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

void Session::userInfoWidget(QString nick)
{
    UserInfo *info = new UserInfo();
    User user = getUserByNick(nick);
    info->init(avatar,server,room,users.size());
    info->initUser(user.nick,user.hash,user.id,user.utype);
    info->show();
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
    systemMessage("丢失与服务器的连接 ...");
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
    userInfoWidget(item->text());
}

void Session::on_lineEdit_textChanged(const QString &arg1)
{
    ui->listWidget->clear();
    for(User user:users) {
        if(user.nick.contains(arg1)){
            QListWidgetItem *item = new QListWidgetItem();
            item->setText(user.nick);
            if(user.nick == this->nick){
                if(this->avatar.isEmpty()){
                    item->setIcon(QIcon(":/ico/unknown"));
                }else {
                    item->setIcon(QIcon("./icon/"+this->avatar));
                }
            }else if(user.icon != NULL) {
                item->setIcon(QIcon("./icon/"+user.icon));
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
    About *h=new About();
    //阻塞其他页面，防止多个关于界面
    h->setWindowModality(Qt::ApplicationModal);
    h->show();
}

void Session::on_send_btn_clicked()
{
    QString message = ui->message_edit->text();
    if(message.isEmpty()) {
        return;
    }
    sendMessage(message);

    ui->message_edit->clear();
}

void Session::on_message_edit_returnPressed()
{
    on_send_btn_clicked();
}

void Session::on_actionsystem_triggered()
{
    Setting *s = new Setting();
    s->show();
}

void Session::on_actionreset_triggered()
{
    Join *j = new Join();
    this->close();
    j->show();
}

void Session::on_actionaccount_triggered()
{
    if(serverType=="hack.chat"){
        QMessageBox::information(this,"Information","Hack.Chat Server no account control function.");
        return;
    }
}
