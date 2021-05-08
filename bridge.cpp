#include "bridge.h"
#include "session.h"
#include <QDebug>
bridge::bridge(QObject *parent) : QObject(parent)
{
    this->parent = (QWidget*)parent;
}

void bridge::alert(QString session,int type)
{
    switch (type) {
    case 0:
        QMessageBox::information((QWidget*)parent,"提示",session);
        return;
    case 1:
        QMessageBox::warning((QWidget*)parent,"警告",session);
        return;
    case 2:
        QMessageBox::critical((QWidget*)parent,"警告",session);
    default:
        return;
    }
}

//初始化聊天界面
void bridge::init()
{
    ((Session*)parent)->initWeb();
}

void bridge::at(QString nick, QString msg)
{
    ((Session*)parent)->at(nick,msg);
}

void bridge::chatApis(int command, QString data)
{
    /*
     * SChat Web->Qt API
     * Now Command Well be notes here;
     * command -> means
     * <-1-> -> userInfoWidget
     *
     * <-default-> -> Api Call Error
     */
    switch (command) {
    case 1 :
        ((Session*)parent)->userInfoWidget(data);
        return;
    default:
        QMessageBox::critical(nullptr,tr("错误"),tr("API调用错误:\n[")+QString::number(command)+tr("]不是预定义的指令索引"));
        return;
    }

}
