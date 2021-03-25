#ifndef JOIN_H
#define JOIN_H

#include "session.h"
#include <QWidget>
#include <QTimer>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCloseEvent>
#include <QMouseEvent>

namespace Ui {
class Join;
}

class Join : public QWidget
{
    Q_OBJECT

public:
    explicit Join(QWidget *parent = nullptr);
    ~Join();

private:
    Ui::Join *ui;

    QString SERVER_URL;
    QString AVATAR;
    bool DEBUG_MODEL;
    int TIME_OUT_SECOUND;
    QPoint m_mousePressPoint;
    bool closeSocket;
    QTimer *offline;
protected:
    void closeEvent(QCloseEvent *event);
    //bool eventFilter(QObject *target, QEvent *e);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private slots:
    void on_login_clicked();
    void socketConnected();
    void socketDisconnected();
    void socketTextMessageReceived(QString message);
    /**
     * 登陆失败，页面变化
     * @brief loginFail
     */
    void loginFail(QString reason);
    void timeOut();
};

#endif // JOIN_H
