#ifndef USERINFO_H
#define USERINFO_H

#include "session.h"
#include <QWidget>

namespace Ui {
class UserInfo;
}

class UserInfo : public QWidget
{
    Q_OBJECT

public:
    explicit UserInfo(QWidget *parent = nullptr);
    ~UserInfo();

    bool init(QString avatar,QString server,QString room,int userNum);
    void initUser(QString nick,QString hash,QString id,QString utype);

private:
    Ui::UserInfo *ui;

    QString defaultValue(QString value,QString defaultValue);
};

#endif // USERINFO_H
