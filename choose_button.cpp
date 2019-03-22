#include "choose_button.h"
#include "ui_choose_button.h"

choose_button::choose_button(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::choose_button)
{
    ui->setupUi(this);
    this->setWindowTitle("Выбор картинки");
}

choose_button::~choose_button()
{
    delete ui;
}

void choose_button::set_button(QPB_modify *pb, int i, int j)
{
    ui->buttons_grid->addWidget(pb, i, j);
    pb->setVisible(true);
}

void choose_button::open_button(QImage img, int i, int j, QString code)
{
    QPB_modify *pb = new QPB_modify;
    QSize button_size(50,50);
    pb->setMaximumSize(button_size);
    pb->setMinimumSize(button_size);
    pb->setIcon(QIcon(QPixmap::fromImage(img)));
    pb->setIconSize(pb->size());

    pb->reverse_img = img;
    pb->i = i;
    pb->j = j;
    pb->was_opening = true;
    pb->rune_code = code;

    connect(pb, &QPushButton::clicked, [this, pb](){
        if (pb->reverse_img.isNull())
            QMessageBox::information(this,"Ошибка", "Не выбрана картинка!");
        else
        {
            emit i_choose_img(pb->reverse_img, pb->i, pb->j, pb->rune_code);
            this->close();
        }
    });

    set_button(pb, i, j);
}

void choose_button::on_button_cancel_clicked()
{
    emit this->reject();
    this->close();
}
