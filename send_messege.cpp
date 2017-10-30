#include "send_messege.h"
#include "ui_send_messege.h"

#include <QLayout>
#include <QLabel>

send_messege::send_messege(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::send_messege)
{
    ui->setupUi(this);

    Loaded_image.load(":/images/test.png");
    Encrypted_image = Loaded_image;
    int wid = ui->img_original->width();
    int hei = ui->img_original->height();
    ui->img_original->setPixmap(QPixmap::fromImage(Loaded_image.scaled(wid,hei)));
    ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image.scaled(wid,hei)));


    ui->lbl_algoritm_value->setText("");
    ui->button_crypto_p->setEnabled(!p_key.empty());
    ui->button_crypto_s->setEnabled(!s_key.empty());

    QRegExp is_value_variable("[a-zA-Z0-9а-яА-Я]{1,20}");
    ui->p_key_edit->setValidator(new QRegExpValidator(is_value_variable, this));
    ui->s_key_edit->setValidator(new QRegExpValidator(is_value_variable, this));

    connect(ui->button_send_messege, SIGNAL(clicked()), this, SIGNAL(change_wnd_to_homewnd()));
    connect(ui->button_choose_img, SIGNAL(clicked()), this, SIGNAL(show_ch_buttons_sign()));
}

send_messege::~send_messege()
{
    delete ui;
}

void send_messege::on_button_load_img_clicked()
{
        /**/
        QString img_original_puth = QFileDialog::getOpenFileName(
                    this,
                    tr("Выберите файл"),
                    "C:\\",
                    "All files (*.*);; Image file (*.bmp)"
                    );

        /* Новое изображение загружается
         * После загрузки устанавливаюстя два изображения - оригинальное и "изменяемое"
         * Переменные wid и hei обозначают соответственно ширину и высоту
         * Введены для большей понятности кода */

        Loaded_image.load(img_original_puth);
        flag_new_image = true;
        Encrypted_image = Loaded_image;
        int wid = ui->img_original->width();
        int hei = ui->img_original->height();
        ui->img_original->setPixmap(QPixmap::fromImage(Loaded_image.scaled(wid,hei)));

        int widch = ui->img_changed->width();
        int heich = ui->img_changed->height();
        ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image.scaled(widch,heich)));
}

void send_messege::on_button_p_key_generate_clicked()
{
    /* Проверка как при нажатии на S
     * В случае не выполнения данных условий - выводи окно ошибки и завершается метод
     * В случае успеха - выводит данные пользователю:label show_now_use...
     * Затем генерирует ключ перестановок, записывает его в атрибут p_key
     * Делает активной клавишу "зашифровать P" */


    /* Проверка на "шифрование различными ключами" */
    if ((Loaded_image != Encrypted_image) && (!p_key.empty()))
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"warning",
                              "Вы меняете ключ, при уже зашифрованной картинке "
                              "это приведет к тому, что изображение вернется в начальное состояние. "
                              "Вы уверены в своем решении?",
                              QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            Encrypted_image = Loaded_image;
            int lblwid = ui->img_changed->width();
            int lblhei = ui->img_changed->height();
            ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image).scaled(lblwid,lblhei));
            flag_new_image = true;
        }
        else
            return;
    }

    /* Проверка на "задан ли верно ключ" */
    if (ui->p_key_edit->text() == "")
    {
        QMessageBox::information(this,"Error","Не задан ключ P");
        return;
    }

    if (ui->p_key_size_edit_spbox->value()==0)
    {
        QMessageBox::information(this,"Error","Длина ключа P не может быть = 0");
        return;
    }

    /* Инициирование стартового значения генератора */
    string value_key = ui->p_key_edit->text().toStdString();
    int generator_start = 0;
    for (int i = 0; i < value_key.size(); i++)
        generator_start = generator_start + value_key[i];


    p_key = pblok_key(ui->p_key_size_edit_spbox->value(), generator_start);
    ui->button_crypto_p->setEnabled(true);

    QString str = QString("%1").arg(ui->p_key_size_edit_spbox->value());// Перевод int в строку
    ui->lbl_p_key_size->setText(str);
    ui->lbl_p_key_value->setText(ui->p_key_edit->text());
}

void send_messege::on_button_s_key_generate_clicked()
{
    /* При нажатии проверяет: 1) задано ли генерирующее слово
     *                        2) Длина = 0 или нет
     *
     * В случае не выполнения данных условий - выводи окно ошибки и завершается метод
     * Проверяет следующую ситуацию: сообщение уже зашифровано, пользователь пытается шифровать на другом ключе
#!   * #!!! Реализован Виженер, а не SBLOK
     * В случае успеха - выводит данные пользователю:label show_now_use...
     * Затем генерирует ключ перестановок, записывает его в атрибут p_key
     * Делает активной клавишу "Зашифровать S"  */

    /* Проверка на "шифрование различными ключами" */
    if ((Loaded_image != Encrypted_image) && (!s_key.empty()))
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"warning",
                              "Вы меняете ключ, при уже зашифрованной картинке "
                              "это приведет к тому, что изображение вернется в начальное состояние. "
                              "Вы уверены в своем решении?",
                              QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            Encrypted_image = Loaded_image;
            int lblwid = ui->img_changed->width();
            int lblhei = ui->img_changed->height();
            ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image).scaled(lblwid,lblhei));
            flag_new_image = true;
        }
        else
            return;
    }

    /* Проверка на "задан ли верно ключ" */
    if (ui->s_key_edit->text() == "")
    {
        QMessageBox::information(this,"Error","Не задан ключ S");
        return;
    }

    if (ui->s_key_size_edit_spbox->value()==0)
    {
        QMessageBox::information(this,"Error","Длина ключа S не может быть = 0");
        return;
    }

    /* Инициирование стартового значения генератора */
    string value_key = ui->s_key_edit->text().toStdString();
    int generator_start = 0;
    for (int i = 0; i < value_key.size(); i++)
        generator_start = generator_start + value_key[i];


    s_key = sblok_like_vigener_key(ui->s_key_size_edit_spbox->value(), generator_start);
    ui->button_crypto_s->setEnabled(true);

    QString str = QString("%1").arg(ui->s_key_size_edit_spbox->value());// Перевод int в строку
    ui->lbl_s_key_size->setText(str);
    ui->lbl_s_key_value->setText(ui->s_key_edit->text());
}

void send_messege::on_button_crypto_p_clicked()
{
    /* Проверки как при нажатии S
     * Проверка, на безграничность роста алгоритма
     * Шифрование
     * После обработки добавляет букву "P" в алгоритм
     * Устанавливает зашифрованную картинку на экран
    */

    if(Loaded_image.isNull())
    {
        QMessageBox::information(this,"Error","Вы не загрузили изображение.");
        return;
    }

    if (flag_new_image == true)
    {
        flag_new_image = false;
        ui->lbl_algoritm_value->setText("");
    }

    QString algoritm = ui->lbl_algoritm_value->text();
    if (algoritm.size() > algoritme_size)
    {
        QMessageBox::information(this,"Error","Слишком большой алгоритм");
        return;
    }
    algoritm += "P";
    ui->lbl_algoritm_value->setText(algoritm);

    Encrypted_image = encrypt_image_p(Encrypted_image, p_key);

    int lblwid = ui->img_changed->width();
    int lblhei = ui->img_changed->height();

    ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image).scaled(lblwid,lblhei));
}

void send_messege::on_button_crypto_s_clicked()
{
    /* Работает аналоично шифрованию P */
    if(Loaded_image.isNull())
    {
        QMessageBox::information(this,"Error","Вы не загрузили изображение.");
         return;
    }

    if (flag_new_image == true)
    {
        flag_new_image = false;
        ui->lbl_algoritm_value->setText("");
    }

    Encrypted_image = encrypt_image_s(Encrypted_image, s_key);

    QString algoritm = ui->lbl_algoritm_value->text();
    if (algoritm.size() > algoritme_size)
    {
        QMessageBox::information(this,"Error","Слишком большой алгоритм");
        return;
    }
    algoritm += "S";
    ui->lbl_algoritm_value->setText(algoritm);


    int lblwid = ui->img_changed->width();
    int lblhei = ui->img_changed->height();

    ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image).scaled(lblwid,lblhei));
}

void send_messege::on_button_algoritm_crypto_clicked()
{
    /* Считывает строку и шифрует, в зависимости от символа */

    if (ui->lbl_algoritm_value->text()=="")
    {
        QMessageBox::information(this,"good_luck","Вы пытаетесь зашифровать без алгоритма");
        return;
    }

    flag_new_image = false;
    Encrypted_image = Loaded_image;
    QString algoritme = ui->lbl_algoritm_value->text();
    for (int i = 0; i<algoritme.size(); i++)
    {
        if(algoritme[i]=='P')
            Encrypted_image = encrypt_image_p(Encrypted_image, p_key);
        else
            Encrypted_image = encrypt_image_s(Encrypted_image, s_key);
    }
    int lblwid = ui->img_changed->width();
    int lblhei = ui->img_changed->height();

    ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image).scaled(lblwid,lblhei));
}

void send_messege::on_button_crypto_cansel_clicked()
{
    if( flag_new_image == true)
    {
        QMessageBox::information(this, "error", "Вы не применили ни один шифр");
        return;
    }

    QString algoritme = ui->lbl_algoritm_value->text();
    if (algoritme == "")
    {
        QMessageBox::information(this,"Oops","Вы не применили ни один шифр");
        return;
    }

    if (algoritme[algoritme.size()-1] == 'P')
    {
        vector<int> revers = pblok_key_revers(p_key);
        Encrypted_image = encrypt_image_p(Encrypted_image, revers);
    }
    else
        Encrypted_image = decrypt_image_s(Encrypted_image, s_key);
    algoritme.chop(1);
    ui->lbl_algoritm_value->setText(algoritme);


    int lblwid = ui->img_changed->width();
    int lblhei = ui->img_changed->height();

    ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image).scaled(lblwid,lblhei));
}

void send_messege::on_button_algoritm_crypto_delete_clicked()
{
    if (Encrypted_image != Loaded_image)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"warning",
                              "Вы меняете алгоритм, при уже зашифрованной картинке "
                              "это приведет к тому, что изображение вернется в начальное состояние. "
                              "Вы уверены в своем решении?",
                              QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            Encrypted_image = Loaded_image;
            int lblwid = ui->img_changed->width();
            int lblhei = ui->img_changed->height();
            ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image).scaled(lblwid,lblhei));
            flag_new_image = true;
        }
        else
            return;
    }

    ui->lbl_algoritm_value->setText("");
}

QImage encrypt_image_p(QImage encrypted_image, vector<int> pb_key)
{
    // Инициируются переменные для удобства использования
    int wid = encrypted_image.width();
    int hei = encrypted_image.height();
    int p_size = pb_key.size();

    for (int i = 0; i < hei*wid;)
    {

        // Считывание и шифрование
        vector<QRgb> temp;
        for (int j=0; j<p_size; j++)
        {
            QRgb t;
            t=encrypted_image.pixel(i%wid, i/wid);
            temp.push_back(t);
            i++;
        }

        // Шифроание и запись зашифрованных пикселей
        if (temp.size()==p_size)
        {
            temp = pblok_use(temp, pb_key);
            for (int j = 0; j<p_size;j++)
            {
                int k=i-p_size+j;
                encrypted_image.setPixel(k%wid, k/wid, temp[j]);
            }
        }
    }
    return encrypted_image;
}

QImage encrypt_image_s(QImage encrypted_image, std::vector<vector<int>> sb_key)
{
    int wid = encrypted_image.width();
    int hei = encrypted_image.height();
    int blok_size = sb_key.size();

    // Считывание
    for (int i = 0; i < hei*wid;)
    {
        vector<QRgb> temp;
        for (int j=0; j<blok_size; j++)
        {
            QRgb t;
            t=encrypted_image.pixel(i%wid, i/wid);
            temp.push_back(t);
            i++;
        }

        // Шифроание и запись зашифрованных пикселей
        if (temp.size()==blok_size)
        {
            temp = sblok_like_vigener_use(temp, sb_key);
            for (int j = 0; j<blok_size;j++)
            {
                int k=i-blok_size+j;
                encrypted_image.setPixel(k%wid, k/wid, temp[j]);
            }
        }
    }
    return encrypted_image;
}

QImage decrypt_image_s(QImage encrypted_image, std::vector<vector<int>> sb_key)
{
    int wid = encrypted_image.width();
    int hei = encrypted_image.height();
    int blok_size = sb_key.size();

    // Считывание
    for (int i = 0; i < hei*wid;)
    {
        vector<QRgb> temp;
        for (int j=0; j<blok_size; j++)
        {
            QRgb t;
            t=encrypted_image.pixel(i%wid, i/wid);
            temp.push_back(t);
            i++;
        }

        // Шифроание и запись зашифрованных пикселей
        if (temp.size()==blok_size)
        {
            temp = sblok_like_vigener_reverse(temp, sb_key);
            for (int j = 0; j<blok_size;j++)
            {
                int k=i-blok_size+j;
                encrypted_image.setPixel(k%wid, k/wid, temp[j]);
            }
        }
    }
    return encrypted_image;
}

void send_messege::user_choose_img(QImage img)
{
    Loaded_image= img;
    flag_new_image = true;
    Encrypted_image = Loaded_image;
    int wid = ui->img_original->width();
    int hei = ui->img_original->height();
    ui->img_original->setPixmap(QPixmap::fromImage(Loaded_image.scaled(wid,hei)));

    int widch = ui->img_changed->width();
    int heich = ui->img_changed->height();
    ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image.scaled(widch,heich)));
}