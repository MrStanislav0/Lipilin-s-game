#include "home_window.h"
#include "ui_home_window.h"

#include <QMessageBox>
#include <ctime>
#include <cstdlib>
#include <QThread>
#include <QTimer>

home_window::home_window(QString login, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::home_window)
{
    ui->setupUi(this);
    connect(ui->button_send_messege,SIGNAL(clicked()),this, SIGNAL(change_wnd_to_swnd()));
    this->setWindowTitle("Мастер-класс Криптография");
    int level = 1;
    ui->button_overhear_messege->setEnabled(level>=2);
    ui->button_assimetry->setEnabled(level>=5);

    srand(time(NULL));//это для случайных чисел

    ui->lbl_login_value->setText(login);
    ui->lbl_level_value->setText("1");

    //add_new_player(login);

    ui->lbl_new_intercepted_msg->setPixmap(QPixmap::fromImage(QImage(":/images/new_messege.png")).scaled(
                                               ui->lbl_new_intercepted_msg->size()));
    ui->lbl_new_intercepted_msg->setVisible(false);

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(rotate_rulet()));
}

home_window::~home_window()
{
    delete ui;
}

void home_window:: set_lvl_label(QString lvl)
{
    ui->lbl_level_value->setText(lvl);
}

vector <vector<QImage>> cut_image(QImage &image, int n, int m)
{
    vector<vector<QImage>> cut;
    for (int i = 0; i<m; i++)
    {
        vector<QImage> str_image;
        for (int j = 0; j<n; j++)
        {
            QImage temp(100,100,image.format());
            int wid_temp = j*100+100;
            for (int wid = j*100; wid<wid_temp; wid++)
            {
                 int hei_temp = i*100+100;
                for (int hei = i*100; hei < hei_temp; hei++)
                {
                    QRgb t = image.pixel(wid,hei);
                    temp.setPixel(wid%100, hei%100, t);
                }
            }
            str_image.push_back(temp);
        }
        cut.push_back(str_image);
    }
  return cut;
}

void home_window::create_img_buttons(vector<vector<QImage>> cut,
                                     int n, int m, vector<vector<QString>> codes,
                                     vector<vector<QString>> mess, QIcon icon_test)
{
    for (int i = 0; i < n; i++)
    {
        vector<QPB_modify*> pb_vec;
        for (int j = 0; j < m; j++)
        {
            QPB_modify *pb = new QPB_modify;
            pb->i = i;
            pb->j = j;
            pb->rune_code = codes[i][j];
            pb->reverse_img = cut[i][j];
            pb->was_opening = false;
            pb->str = mess[i][j];

            QSize button_size(50,50);
            pb->setMaximumSize(button_size);
            pb->setMinimumSize(button_size);

            pb->setIcon(icon_test);
            QSize icon_size(40,40);
            pb->setIconSize(icon_size);

            connect(pb, &QPushButton::clicked, [this, pb]()
            {
                if (count_777 == 0)
                {
                    QMessageBox::information(this, "Азино 777", "Вращайте барабан!");
                    return;
                }
                count_777--;

                QPB_modify *pb2 = new QPB_modify;
                pb2->setIcon(QPixmap::fromImage(pb->reverse_img));
                pb2->setIconSize(pb->size());
                pb2->setMaximumSize(pb->size());

                pb2->str = pb->str;
                pb2->was_sending = false;
                pb2->was_opening = true;
                pb2->i = pb->i;
                pb2->j = pb->j;

                connect(pb2, &QPushButton::clicked, [this, pb2]()
                {
                    if (pb2->was_sending == false)
                        QMessageBox::information(this, "info", "Не удалось отправить сообщение в штаб");
                    else
                        QMessageBox::information(this, "info", pb2->str);
                });

                emit i_opend(pb->reverse_img, pb->i, pb->j, pb->rune_code);
                ui->componate_custom_buttons_img->addWidget(pb2, pb->i, pb->j);

                delete icons[pb2->i][pb2->j];
                icons[pb2->i][pb2->j] = pb2;
            });

            emit do_it(pb->i, pb->j);
            ui->componate_custom_buttons_img->addWidget(pb,i,j);
            pb_vec.push_back(pb);
        }
        icons.push_back(pb_vec);
    }
}

void home_window::add_new_player(QString new_player_login)
{
    ui->user_list->addItem(new_player_login);
}

void home_window::on_button_overhear_messege_clicked()
{
    ui->lbl_new_intercepted_msg->setVisible(false);
    emit this->show_intercept_wnd_please();
}

void home_window::on_button_inercept_clicked()
{    
    for (int i = 0; i < (int)ui->user_list->count(); i++)
        {
    if (ui->user_list->item(i)->isSelected())
            {
                QString intercepting_player = ui->user_list->item(i)->text();
                emit this->i_want_intercept(intercepting_player);
                ui->lbl_overhere_value->setText(intercepting_player);
                return;
            }
    }
    QMessageBox::information(this, "error", "Такого игрока нет");
}

void home_window::set_visibale_new_messege(bool vis)
{
    ui->lbl_new_intercepted_msg->setVisible(vis);
}

/* ------ */
void home_window::on_automat_clicked()
{
    ui->automat->setEnabled(false);
    int iRand = rand() % 2;
    rand_image(iRand);
    count_timer = 0;
    timer->start(450);
}

void home_window::rand_image(int c)
{
    QImage img;

    switch (c)
    {
    case 3:
    {
        int image_num = rand()%5;
        img=runes[image_num];
        ui->rand_image1->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image1->size()));
        ui->rand_image2->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image2->size()));
        ui->rand_image3->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image3->size()));
        break;
    }
    case 2:
    {
        int count_runes = 5;

        int runes2[] = { 0,1,2,3,4 };
        int new_runes[] = { 0,1 };

        for (int i = 0; i < 2; i++)
        {
            int Rand_index = rand() % count_runes;

            new_runes[i] = runes2[Rand_index];
            int last = runes2[count_runes - 1];
            runes2[Rand_index] = last;

            count_runes--;
        }

        int place = rand() % 3;

        switch (place)
        {
        case 0:
            //натянуть на 2 и 3
            img=runes[new_runes[0]];
            ui->rand_image2->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image2->size()));
            ui->rand_image3->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image3->size()));

            img=runes[new_runes[1]];
            ui->rand_image1->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image1->size()));
            break;
        case 1:
            //натянуть на 1 и 3
            img=runes[new_runes[0]];
            ui->rand_image1->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image1->size()));
            ui->rand_image3->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image3->size()));

            img=runes[new_runes[1]];
            ui->rand_image2->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image2->size()));
            break;
        case 2:
            //натянуть на 1 и 2
            img=runes[new_runes[0]];
            ui->rand_image1->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image1->size()));
            ui->rand_image2->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image2->size()));

            img=runes[new_runes[1]];
            ui->rand_image3->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image3->size()));
            break;
        }
        break;
    }
    case 1:
    {
        int count_runes = 5;

        int runes2[] = { 0,1,2,3,4 };
        int new_runes[] = { 0,1,2 };

        for (int i = 0; i < 3; i++)
        {
            int Rand_index = rand() % count_runes;

            new_runes[i] = runes2[Rand_index];
            int last = runes2[count_runes - 1];
            runes2[Rand_index] = last;

            count_runes--;
        }

        //сделать одно
        img=runes[new_runes[0]];
        ui->rand_image1->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image1->size()));
        //второе
        img=runes[new_runes[1]];
        ui->rand_image2->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image2->size()));
        //третье
        img=runes[new_runes[2]];
        ui->rand_image3->setPixmap(QPixmap::fromImage(img).scaled(ui->rand_image3->size()));
        break;
    }
    }
}

void home_window::i_find_image_bf(int i, int j)
{
    QPB_modify *pb = icons[i][j];
    if (pb->was_opening == false)
    {
        count_777++; // Компенсация перехваченного сообщения
        emit pb->clicked();
    }
}

/* ------ */

void home_window::on_button_assimetry_clicked()
{
    emit this->from_home_wnd_to_rsa_wnd();
}

void home_window::up_level(int level)
{

    ui->button_overhear_messege->setEnabled(level>=2);
    ui->button_assimetry->setEnabled(level>=7);
}

void home_window::set_button_was_sending(int i, int j)
{
   QPB_modify* pb = icons[i][j];
   pb->was_sending = true;

   ui->componate_custom_buttons_img->addWidget(pb, pb->i, pb->j);

   icons[pb->i][pb->j] = pb;
}

void home_window::rotate_rulet()
{
    int iRand = rand() % 2;
    rand_image(iRand);
    count_timer++;

    if (count_timer == 17)
    {
        timer->stop();
        ui->automat->setEnabled(true);
        count_timer = 0;

        int luck;
        if (flag_offline)
            luck = 99;
        else
            luck = rand() % 100;
        if (luck<82)
        {
            //3 случайные картинки, не совпадающие
            rand_image(1);
            QMessageBox::information(this,"LOSE", "К сожалению, вам не повезло. Попробуйте еще раз.");
        }
        else if (luck<92)
        {

            //открыть 2 одинаковые
            rand_image(2);
            count_777++;
            QMessageBox::information(this,"WIN", "Поздравляем! Вы можете открыть одно изображение!");
        }
        else
        {

            //открыть 3 одинаковые
            rand_image(3);
            count_777+=2;
            QMessageBox::information(this,"WIN", "Вы сказочный везунчик! Вы можете открыть целых два изображения!");
        }
    }
}

void home_window::delete_player(QString login)
{
    for (int i = 0; i < (int)ui->user_list->count(); i++)
    {
        if (login == ui->user_list->item(i)->text())
        {
             delete ui->user_list->takeItem(i);
             break;
        }
    }
}
