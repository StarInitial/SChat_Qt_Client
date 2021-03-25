#ifndef ABOUT_H
#define ABOUT_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class About;
}

class About : public QWidget
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    ~About();

private:
    QPoint m_mousePressPoint;
    Ui::About *ui;
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // ABOUT_H
