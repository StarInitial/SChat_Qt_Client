#include "userinfo.h"
#include "ui_userinfo.h"

UserInfo::UserInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserInfo)
{
    ui->setupUi(this);
}

UserInfo::~UserInfo()
{
    delete ui;
}

bool UserInfo::init( QString avatar, QString server, QString room, int userNum)
{
    if(!avatar.isNull()&&!avatar.isEmpty()){
        ui->icon->setStyleSheet("border-image: url(./icon/"+avatar+");");
    }
    ui->server->setText(defaultValue(server,"[未知]"));
    ui->room->setText(defaultValue(room,"[未知]"));
    ui->online_num->setText(QString::number(userNum));

    return true;
}

QString UserInfo::defaultValue(QString value, QString defaultValue)
{
    if(value.isNull() || value.isEmpty()){
        return defaultValue;
    }
    return value;
}

void UserInfo::initUser(QString nick, QString hash, QString id, QString utype)
{
    this->setWindowTitle(tr("SChat | 用户:")+nick);
    ui->nick_0->setText(nick);
    ui->nick_1->setText(nick);
    ui->id->setText(id);
    ui->hash->setText(hash);
    ui->utype->setText("["+utype+"]");
}
