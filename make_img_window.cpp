#include "make_img_window.h"
#include "ui_make_img_window.h"

make_img_window::make_img_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::make_img_window)
{
    ui->setupUi(this);
    RuneSelected = false;
    set_colors();
    set_runes();
    ui->edit_root_messege->setVisible(false);
    ui->lbl_root_show_messege->setVisible(false);
    this->setWindowTitle("Выбор картинки");
    QRegExp one_char("[А-Я]{0,1}");
    ui->edit_root_messege->setValidator(new QRegExpValidator(one_char, this));
}

make_img_window::~make_img_window()
{
    delete ui;
}

void make_img_window::set_colors()
{
    QHBoxLayout *hbox = new QHBoxLayout(this);
    int color_size = colors.size();
    for (int i = 0; i < color_size; i++)
    {
        QPB_modify *pb = new QPB_modify;
        QImage temp(100, 100, QImage::Format_RGB32);
        temp.fill(QColor(colors[i]));

        pb->j = i;

        pb->setMaximumSize(50, 50);
        pb->setMinimumSize(50, 50);
        pb->setIcon(QIcon(QPixmap::fromImage(temp)));
        pb->setIconSize(pb->size());
        connect(pb, &QPushButton::clicked, [this, pb](){
            if(!RuneSelected)
                QMessageBox::information(this, "Ошибка", "Выберите изображение справа");
            else
            {
                ic = pb->j;
                QImage temp = paint_picture(ui->lbl_created_img->pixmap()->toImage(), colors[pb->j]);
                ui->lbl_created_img->setPixmap(QPixmap::fromImage(temp));
            }
        });
        hbox->addWidget(pb);
    }

    ui->scroll_color_content->setLayout(hbox);
}

void make_img_window::set_runes()
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    int runes_size = runes.size();
    for (int i = 0; i < runes_size; i++)
    {
        QPB_modify *pb = new QPB_modify;

        pb->setMaximumSize(80, 80);
        pb->setMinimumSize(80, 80);
        pb->setIcon(QIcon(QPixmap::fromImage(QImage(runes[i]))));

        pb->i = i;

        pb->setIconSize(pb->size());
        connect(pb, &QPushButton::clicked, [this, pb](){
            ir = pb->i;
            ic = -1;
            flag_img_choosen = true;
            RuneSelected = true;
            ui->lbl_created_img->setPixmap(QPixmap::fromImage(QImage(runes[pb->i])).scaled(ui->lbl_created_img->size()));
        });

        vbox->addWidget(pb);
    }

    ui->scroll_rune_content->setLayout(vbox);
}

void make_img_window::on_button_cancel_clicked()
{
    emit this->reject();
    this->close();
}

void make_img_window::on_button_ok_clicked()
{
    QImage img = ui->lbl_created_img->pixmap()->toImage().scaled(100, 100);
    QString img_code = QString::number(ir) + '_' + QString::number(ic);

    if (flag_is_it_root)
        emit this->root_make_new_img(img, root_i, root_j, ui->edit_root_messege->text(), img_code);
    else
        emit this->i_make_img(img, img_code);

    this->close();
}

QImage make_img_window::paint_picture (QImage img, QRgb color)
{
    //позаимствовала и видоизменила функцию шифрования
    //пусть будет просто цвет, а в вызове функции укажем, какой именно элемент массива цветов надо брать
    //выпиливаем изначально нужный цвет
    QImage data = img;
    QRgb white = qRgb(255,255,255);

    int wid = data.width();
    int hei = data.height();

    int cicl_end = wid*hei;

    for (int i = 0; i < cicl_end; i++)
    {
        QRgb temp = data.pixel(i%wid, i/wid);

        if (temp!=white) //исключаем белый цвет (фон букв)
            data.setPixel(i%wid, i/wid, color);
    }

    return data;
}

void make_img_window::set_rune(QImage img, int i, int j, QString str)
{
    ui->lbl_created_img->setPixmap(QPixmap::fromImage(img).scaled(ui->lbl_created_img->size()));
    root_i = i;
    root_j = j;
    flag_is_it_root = true;
    flag_img_choosen = true;
    RuneSelected = true;
    ui->edit_root_messege->setText(str);

    ui->edit_root_messege->setVisible(true);
    ui->lbl_root_show_messege->setVisible(true);
}

 QImage make_img_window::paint_picture_at_code (int rune_ind, int color_ind)
 {
     QImage r = QImage(runes[rune_ind]);
     QRgb c = colors[color_ind];
     return paint_picture(r, c);
 }

