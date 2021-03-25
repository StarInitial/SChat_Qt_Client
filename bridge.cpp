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

//附件
void bridge::file()
{
    if(parent == nullptr) return;
    ((Session*)parent)->file();
}

void bridge::saveHistory(QString history)
{
    if(parent == nullptr) {
        QMessageBox::critical(nullptr,tr("错误"),tr("调用错误 [聊天记录] 存储失败"));
        return;
    }
    qDebug()<<history;
    Q_UNUSED(history);
}

void bridge::chatApis(int command, QString data)
{
    /*
     * SChat Web->Qt API
     * Now Command Well be notes here;
     * command -> means
     * <-1-> -> Send Message
     *
     * <-default-> -> Api Call Error
     */
    switch (command) {
    case 1:
        ((Session*)parent)->sendMessage(data);
        return;
    default:
        QMessageBox::critical(nullptr,tr("错误"),tr("API调用错误:\n[")+QString::number(command)+tr("]不是预定义的指令索引"));
        return;
    }

}
