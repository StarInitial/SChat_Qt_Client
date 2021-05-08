#include "setting.h"
#include "ui_setting.h"
#include "about.h"
#include "join.h"
#include "session.h"

#include "QDebug"

Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    this->initUI();
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
    backUpdate();
}

void Setting::clearColor()
{
    ui->c_1->setStyleSheet(ui->c_1->styleSheet().remove("border-image: url(:/ico/color_check_in);"));
    ui->c_2->setStyleSheet(ui->c_2->styleSheet().remove("border-image: url(:/ico/color_check_in);"));
    ui->c_3->setStyleSheet(ui->c_3->styleSheet().remove("border-image: url(:/ico/color_check_in);"));
    ui->c_4->setStyleSheet(ui->c_4->styleSheet().remove("border-image: url(:/ico/color_check_in);"));
}

void Setting::initUI()
{
    ui->tabWidget->tabBar()->close();
    ui->lock_widget->hide();
    ui->auth_widget->hide();
    ui->auth_widget_2->hide();
}

void Setting::initParentWidget(QWidget *w, QString type)
{
    this->w_type = type;
    this->w = w;
}

int Setting::inSize(int size, int min, int max, int defaultSize)
{
    if(size >= min && size <= max) {
        return size;
    }
    return defaultSize;
}

void Setting::backUpdate()
{
    if(w_type == "join"){
        ((Join *)w)->initConfigFile();
        ((Join *)w)->loadStyleColor();
    }
}

void Setting::loadStyleColor(int r, int g, int b)
{

    QString style_btn = "QPushButton{\nbackground-color: rgba(225, 225, 225, 0);\ncolor: $[STYLE_COLOR]\nborder:0;\n}\nQPushButton:hover{\nbackground-color: rgb(225, 225, 225, 100);\n}\nQPushButton:pressed{\nbackground-color: rgb(225, 225, 225, 200);\n}";
    QString style_input_line = "QLineEdit{\nbackground-color: rgb(255, 255, 255,0);\n	color: rgb(0, 0, 0);\noutline: none;\nborder: none;\nborder-bottom: 2px solid rgb(100, 100, 100);\nselection-background-color: $[STYLE_COLOR]\n}\nQLineEdit:hover{\nborder-bottom: 2px solid rgb(20, 20, 20);\n}\nQLineEdit:focus{\nborder-bottom: 2px solid $[STYLE_COLOR]\n}";
    QString style_combo_box = "QComboBox{\nbackground-color: rgb(255, 255, 255,0);\n	color: rgb(0, 0, 0);\noutline: none;\nborder: none;\nborder-bottom: 2px solid rgb(100, 100, 100);\nselection-background-color: $[STYLE_COLOR]\n}\nQComboBox:hover{\nborder-bottom: 2px solid rgb(20, 20, 20);\n}\nQComboBox:focus{\nborder-bottom: 2px solid $[STYLE_COLOR]\n}";
    QString style_font_combo_box = "QFontComboBox{\nbackground-color: rgb(255, 255, 255,0);\ncolor: rgb(0, 0, 0);\noutline: none;\nborder: none;\nborder-bottom: 2px solid rgb(100, 100, 100);\nselection-background-color: $[STYLE_COLOR]\n}\nQFontComboBox:hover{\nborder-bottom: 2px solid rgb(20, 20, 20);\n}\nQFontComboBox:focus{\nborder-bottom: 2px solid $[STYLE_COLOR]\n}";

    //数值校验
    //默认颜色:rgb(63, 188, 242);
    r = inSize(r,0,255,63);
    g = inSize(g,0,255,188);
    b = inSize(b,0,255,242);

    QString colorRgbName = "rgb("+QString::number(r)+","+QString::number(g)+","+QString::number(b)+");";
    QString style_clor_name = "$[STYLE_COLOR]";

    //按键主题调整
    ui->server_btn->setStyleSheet(style_btn.replace(style_clor_name,colorRgbName));
    ui->chat_btn->setStyleSheet(style_btn.replace(style_clor_name,colorRgbName));
    ui->safe_btn->setStyleSheet(style_btn.replace(style_clor_name,colorRgbName));
    ui->style_btn->setStyleSheet(style_btn.replace(style_clor_name,colorRgbName));
    ui->ok->setStyleSheet(style_btn.replace(style_clor_name,colorRgbName));
    ui->change_path_3->setStyleSheet(style_btn.replace(style_clor_name,colorRgbName));
    ui->encryption_plus_config->setStyleSheet(style_btn.replace(style_clor_name,colorRgbName));
    ui->change_path->setStyleSheet(style_btn.replace(style_clor_name,colorRgbName));
    ui->about_btn->setStyleSheet(style_btn.replace(style_clor_name,colorRgbName));
    ui->about_btn_1->setStyleSheet(style_btn.replace(style_clor_name,colorRgbName));
    ui->about_btn_2->setStyleSheet(style_btn.replace(style_clor_name,colorRgbName));
    ui->about_btn_3->setStyleSheet(style_btn.replace(style_clor_name,colorRgbName));

    //label调整
    ui->l_4->setStyleSheet("border-radius: 25px;\ncolor: rgb(255, 255, 255);\nbackground-color: "+colorRgbName);
    ui->label_5->setStyleSheet("border-radius: 25px;\nbackground-color: "+colorRgbName);
    ui->label_19->setStyleSheet("color: "+colorRgbName);

    //输入框调整
    ui->server_ip->setStyleSheet(style_input_line.replace(style_clor_name,colorRgbName));
    ui->server_port->setStyleSheet(style_input_line.replace(style_clor_name,colorRgbName));
    ui->server_auth_c->setStyleSheet(style_input_line.replace(style_clor_name,colorRgbName));
    ui->server_auth_key->setStyleSheet(style_input_line.replace(style_clor_name,colorRgbName));
    ui->password->setStyleSheet(style_input_line.replace(style_clor_name,colorRgbName));
    ui->style_file_path->setStyleSheet(style_input_line.replace(style_clor_name,colorRgbName));
    ui->server_url->setStyleSheet(style_input_line.replace(style_clor_name,colorRgbName));
    ui->spinBox->setStyleSheet("QSpinBox{\nbackground-color: rgb(255, 255, 255,0);\n	color: rgb(0, 0, 0);\noutline: none;\nborder: none;\nborder-bottom: 2px solid rgb(100, 100, 100);\nselection-background-color: "+colorRgbName+"\n}\nQSpinBox:hover{\nborder-bottom: 2px solid rgb(20, 20, 20);\n}\nQSpinBox:focus{\nborder-bottom: 2px solid "+colorRgbName+"\n}");

    //选择框调整
    ui->fontComboBox->setStyleSheet(style_font_combo_box.replace(style_clor_name,colorRgbName));
    ui->style_combo->setStyleSheet(style_combo_box.replace(style_clor_name,colorRgbName));
    ui->style_combo_2->setStyleSheet(style_combo_box.replace(style_clor_name,colorRgbName));
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
    ui->c_1->setStyleSheet(ui->c_1->styleSheet().append("border-image: url(:/ico/color_check_in);"));
    loadStyleColor(63,188,242);
    ui->title->setStyleSheet("border-image: url(:/title/setting_title);");
}

void Setting::on_c_2_clicked()
{
    clearColor();
    ui->c_2->setStyleSheet(ui->c_2->styleSheet().append("border-image: url(:/ico/color_check_in);"));
    loadStyleColor(0,0,0);
    ui->title->setStyleSheet("border-image: url(:/title/setting_title_black);");
}

void Setting::on_c_4_clicked()
{
    clearColor();
    ui->c_4->setStyleSheet(ui->c_4->styleSheet().append("border-image: url(:/ico/color_check_in);"));
    loadStyleColor(251,114,153);
    ui->title->setStyleSheet("border-image: url(:/title/setting_title_pink);");
}

void Setting::on_c_3_clicked()
{
    clearColor();
    ui->c_3->setStyleSheet(ui->c_3->styleSheet().append("border-image: url(:/ico/color_check_in);"));
    loadStyleColor(0,85,127);
    ui->title->setStyleSheet("border-image: url(:/title/setting_title_darkblue);");
}

void Setting::on_about_btn_1_clicked()
{
    About *w = new About();
    w->show();
}

void Setting::on_radioButton_2_clicked(bool checked)
{
    if(checked){
        ui->auth_widget->show();
        ui->auth_widget_2->show();
    }
}

void Setting::on_radioButton_clicked(bool checked)
{
    if(checked){
        ui->auth_widget->hide();
        ui->auth_widget_2->hide();
    }
}
