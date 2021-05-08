#ifndef SESSION_H
#define SESSION_H

#include <QMainWindow>

#include "bridge.h"
#include "join.h"
#include "about.h"
#include "userinfo.h"
#include <QFileInfo>
#include <QCloseEvent>
#include <QWebChannel>
#include <QWebSocket>
#include <QJsonArray>
#include <QListWidgetItem>
#include <QWebEngineSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class Session; }
QT_END_NAMESPACE

class User{
public:
    QString id;
    QString nick;
    QString icon;
    QString trip;
    QString utype;
    QString hash;
    QString sign;
    bool isMe;
};

extern QWebSocket *socket;

class Session : public QMainWindow
{
    Q_OBJECT

public:
    Session(QWidget *parent = nullptr);
    ~Session();

    void file();
    void init(QString server,QString nick,QString avatar,QString room,QString styleColor,QString serverType,QString loginMessage);
    QList<User> users;
    void at(QString nick,QString msg);
    void userInfoWidget(QString nick);
public slots:
    void initWeb();
    void sendMessage(QString message);
private:
    QString room;
    QString server;
    QString nick;
    QString avatar;
    QString serverType;
    QString styleColor;
protected:
    void closeEvent(QCloseEvent *event);
    void loadStyleColor(QString color);
    //userMessage(messageType.Other,'看见好处就要抢',"雷狮"/*,"../icon/leishi.jpg"*/);
    void systemMessage(QString message);
    void myMessage(QString message);
    void otherMessage(QString message,QString nick,QString avatar);
    void onlineSet(QString json);
    void addUserToList(QJsonObject json);
    void removeUserByNick(QString nick);
    User getUserByNick(QString nick);

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_lineEdit_textChanged(const QString &arg1);
    void on_actionempty_triggered();
    void on_listWidget_itemClicked(QListWidgetItem *item);

    void socketConnected();
    void socketDisconnected();
    void socketTextMessageReceived(QString message);

    void on_actionabout_triggered();

    void on_send_btn_clicked();

    void on_message_edit_returnPressed();

    void on_actionsystem_triggered();

    void on_actionreset_triggered();
    void on_actionaccount_triggered();

private:
    Ui::Session *ui;
    bridge *bridge;
};
#endif // SESSION_H
