#ifndef BRIDGE_H
#define BRIDGE_H

#include <QObject>
#include <QMessageBox>

class bridge : public QObject
{
    Q_OBJECT
public:
    explicit bridge(QObject *parent = nullptr);
    QWidget *parent;

signals:

public slots:
    void alert(QString session,int type);
    void init();
    void file();
    void saveHistory(QString history);
    void chatApis(int command,QString data);
};

#endif // BRIDGE_H
