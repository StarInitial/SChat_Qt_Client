#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QTabBar>

namespace Ui {
class Setting;
}

class Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = nullptr);
    ~Setting();

    void setPage(int page);
    void clearColor();

private slots:
    void on_server_btn_clicked();

    void on_chat_btn_clicked();

    void on_safe_btn_clicked();

    void on_style_btn_clicked();

    void on_about_btn_clicked();

    void on_is_lock_stateChanged(int arg1);

    void on_horizontalSlider_sliderMoved(int position);

    void on_fontComboBox_currentIndexChanged(const QString &arg1);

    void on_c_1_clicked();

    void on_c_2_clicked();

    void on_c_4_clicked();

    void on_c_3_clicked();

    void on_ok_2_clicked();

    void on_ok_clicked();

private:
    Ui::Setting *ui;
};

#endif // SETTING_H
