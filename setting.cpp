#include "setting.h"
#include "ui_setting.h"
#include "about.h"

#include "QDebug"

Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    ui->tabWidget->tabBar()->close();
    ui->lock_widget->hide();
    setPage(0);
}

Setting::~Setting()
{
    delete ui;
}

void Setting::setPage(int page)
{
    ui->tabWidget->setCurrentIndex(page);
}

void Setting::on_ok_clicked()
{
}

void Setting::clearColor()
{
    ui->c_1->setText("");
    ui->c_2->setText("");
    ui->c_3->setText("");
    ui->c_4->setText("");
}

void Setting::on_server_btn_clicked()
{
    setPage(0);
}

void Setting::on_chat_btn_clicked()
{
    setPage(1);
}

void Setting::on_safe_btn_clicked()
{
    setPage(2);
}

void Setting::on_style_btn_clicked()
{
    setPage(3);
}

void Setting::on_about_btn_clicked()
{
    setPage(4);
}

void Setting::on_is_lock_stateChanged(int arg1)
{
    switch (arg1) {
    case 0:
        ui->lock_widget->hide();
        return;
    case 2:
        ui->lock_widget->show();
        return;
    }
}

void Setting::on_horizontalSlider_sliderMoved(int position)
{
    QFont font = ui->l_4->font();
    font.setPointSize(position);
    ui->size->setText(QString::number(position));
    ui->l_4->setFont(font);
    ui->l_2->setFont(font);
}

void Setting::on_fontComboBox_currentIndexChanged(const QString &arg1)
{
    QFont font = ui->l_4->font();
    font.setFamily(arg1);
    ui->l_4->setFont(font);
    ui->l_2->setFont(font);
}

void Setting::on_c_1_clicked()
{
    clearColor();
    ui->c_1->setText("选中");
}

void Setting::on_c_2_clicked()
{
    clearColor();
    ui->c_2->setText("选中");
}

void Setting::on_c_4_clicked()
{
    clearColor();
    ui->c_4->setText("选中");
}

void Setting::on_c_3_clicked()
{
    clearColor();
    ui->c_3->setText("选中");
}

void Setting::on_ok_2_clicked()
{
    About *w = new About();
    w->show();
}
