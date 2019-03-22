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

    this->setWindowTitle("Отправка сообщений");

    ui->lbl_algoritm_value->setText("");

    QRegExp is_value_variable("[a-zA-Z0-9а-яА-Я]{1,20}");
    ui->p_key_edit->setValidator(new QRegExpValidator(is_value_variable, this));
    ui->s_key_edit->setValidator(new QRegExpValidator(is_value_variable, this));

    QRegExp is_coordinate_variable("[0-9]{0,5}");
    ui->edit_coordinate_i->setValidator((new QRegExpValidator(is_coordinate_variable, this)));
    ui->edit_coordinate_j->setValidator((new QRegExpValidator(is_coordinate_variable, this)));

    connect(ui->button_choose_img, SIGNAL(clicked()), this, SIGNAL(show_ch_buttons_sign()));
    connect(ui->button_make_img, SIGNAL(clicked()), this, SIGNAL(show_make_img_wnd()));

    ui->lbl_progress->setVisible(false);
    ui->progress_send->setVisible(false);
    int level = 1;
    ui->button_p_key_generate->setEnabled(level>=3);
    ui->button_s_key_generate->setEnabled(level>=4);
    ui->button_crypto_cansel->setEnabled(level>=4);
    ui->button_algoritm_crypto->setEnabled(level>=4);
    ui->button_algoritm_crypto_delete->setEnabled(level>=3);
    ui->button_crypt_rsa->setEnabled(level>=5);
    ui->button_make_img->setEnabled(level>=6);
    ui->button_crypto_p->setEnabled((level>=3)&&(!p_key.empty()));
    ui->button_crypto_s->setEnabled((level>=3)&&(!s_key.empty()));

    ui->check_box_crypt_keys->setEnabled(false);
}

send_messege::~send_messege()
{
    delete ui;
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
                              "Вы меняете ключ, при уже зашифрованной или перехваченной картинке. "
                              "Это приведет к тому, что изображение вернется в начальное состояние. "
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
    int value_key_size = value_key.size();
    for (int i = 0; i < value_key_size; i++)
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
     * В случае успеха - выводит данные пользователю:label show_now_use..
     * Затем генерирует ключ перестановок, записывает его в атрибут p_key
     * Делает активной клавишу "Зашифровать S"  */

    /* Проверка на "шифрование различными ключами" */
    if ((Loaded_image != Encrypted_image) && (!s_key.empty()))
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"warning",
                              "Вы меняете ключ, при уже зашифрованной или перехваченной картинке. "
                              "Это приведет к тому, что изображение вернется в начальное состояние. "
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
    int value_key_size = value_key.size();
    for (int i = 0; i < value_key_size; i++)
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

    if (flag_rsa_used)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"warning",
                              "Вы пытаетесь шифровать симметричными алгоритмами при "
                              "примененном ассиметричном шифре. "
                              "Это приведет к тому, что изображение вернется в начальное состояние. "
                              "Вы уверены в своем решении?",
                              QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            Encrypted_image = Loaded_image;
            int lblwid = ui->img_changed->width();
            int lblhei = ui->img_changed->height();
            ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image).scaled(lblwid,lblhei));
            flag_new_image = true;
            flag_rsa_used = false;
        }
        else
            return;
    }

    if (flag_new_image == true)
    {
        flag_new_image = false;
        ui->lbl_algoritm_value->setText("");
    }

    QString algoritm = ui->lbl_algoritm_value->text();
    int algoritm_size = algoritm.size();
    if (algoritm_size > algoritme_size)
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

    if (flag_rsa_used)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"warning",
                              "Вы пытаетесь шифровать симметричными алгоритмами при "
                              "примененном ассиметричном шифре. "
                              "Это приведет к тому, что изображение вернется в начальное состояние. "
                              "Вы уверены в своем решении?",
                              QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {

            Encrypted_image = Loaded_image;
            int lblwid = ui->img_changed->width();
            int lblhei = ui->img_changed->height();
            ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image).scaled(lblwid,lblhei));
            flag_new_image = true;
            flag_rsa_used = false;
        }
        else
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

    algoritm += "S";
    ui->lbl_algoritm_value->setText(algoritm);

    Encrypted_image = encrypt_image_s(Encrypted_image, s_key);
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

    Encrypted_image = Loaded_image;
    QString algoritme = ui->lbl_algoritm_value->text();
    if (algoritme == "RSA")
    {
        ui->button_cancel_rsa->clicked();
        return;
    }

    flag_new_image = false;

    int algoritm_size = algoritme.size();
    for (int i = 0; i<algoritm_size; i++)
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

    if (algoritme == "RSA")
    {
        QMessageBox::information(this, "oops", "Не применялись операции шифрования P и S");
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
    flag_rsa_used = false;
}

void send_messege::user_choose_img(QImage img, QString code)
{
    if (flag_rsa_used)
    {
        flag_rsa_used = false;
        ui->lbl_algoritm_value->setText("");
    }

    mc = 75;
    Loaded_image= img;
    flag_new_image = true;
    Encrypted_image = Loaded_image;
    now_using_rune_code = code;

    int wid = ui->img_original->width();
    int hei = ui->img_original->height();
    ui->img_original->setPixmap(QPixmap::fromImage(Loaded_image.scaled(wid,hei)));

    int widch = ui->img_changed->width();
    int heich = ui->img_changed->height();
    ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image.scaled(widch,heich)));
}

void send_messege::user_made_img(QImage img, QString code)
{
    if (flag_rsa_used)
    {
        flag_rsa_used = false;
        ui->lbl_algoritm_value->setText("");
    }

    mc = 100;
    Loaded_image= img;
    flag_new_image = true;
    Encrypted_image = Loaded_image;
    now_using_rune_code = code;

    int wid = ui->img_original->width();
    int hei = ui->img_original->height();
    ui->img_original->setPixmap(QPixmap::fromImage(Loaded_image.scaled(wid,hei)));

    int widch = ui->img_changed->width();
    int heich = ui->img_changed->height();
    ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image.scaled(widch,heich)));
}

void send_messege::set_position_of_img(int i, int j)
{
    ui->edit_coordinate_i->setText(QString::number(i+1)); // +1, чтобы пользователю было удобнее считать
    ui->edit_coordinate_j->setText(QString::number(j+1));
}

void send_messege::on_button_send_messege_clicked()
{
    /* Делает кнопки неактивными */
    this->setEnabled(false);

    /* Забивает время */
    ui->lbl_progress->setVisible(true);
    ui->progress_send->setVisible(true);

    for (int i = 0; i < 100; i++)
    {
        QEventLoop loop;
        QTimer::singleShot(mc, &loop, SLOT(quit()));
        loop.exec();
        loop.exit();
        ui->progress_send->setValue(i+1);
    }

    ui->lbl_progress->setVisible(false);
    ui->progress_send->setVisible(false);
    /* ------------------------------------------ */

    this->setEnabled(true);

    int i;
    QString i_str = ui->edit_coordinate_i->text();
    if (i_str == "")
        i = -1;
    else
        i = ui->edit_coordinate_i->text().toInt();

    int j;
    QString j_str = ui->edit_coordinate_j->text();
    if (j_str == "")
        j = -1;
    else
        j = ui->edit_coordinate_j->text().toInt();

    QString code = now_using_rune_code;
    if (code == "100_100")
    {
        QMessageBox::information(this, "???", "Вы пытаетесь подсунуть мне что-то невразумительное");
        return;
    }

    QString algoritm;
    if (flag_new_image)
        algoritm = "";
    else
        algoritm = ui->lbl_algoritm_value->text();

    if (flag_rsa_used)
    {
        emit this->send_rsa_messege(code, e, n, i, j);
        return;
    }

    QString p_key_str;
    p_key_str = ui->lbl_p_key_value->text();

    int p_key_size;
    QString p_key_size_str = ui->lbl_p_key_size->text();
    if (p_key_size_str == "none")
        p_key_size = 0;
    else
        p_key_size = p_key_size_str.toInt();

    QString s_key_str;
    s_key_str = ui->lbl_s_key_value->text();

    int s_key_size;
    QString s_key_size_str = ui->lbl_s_key_size->text();
    if (s_key_size_str == "none")
        s_key_size = 0;
    else
        s_key_size = s_key_size_str.toInt();

    emit this->player_send_messege(code, p_key_str, p_key_size, s_key_str, s_key_size, i, j, algoritm);
}

void send_messege::players_img_verdict(bool verdict)
{
    if (verdict == false)
        QMessageBox::information(this, "Марио, твоя принцесса в другом замке", "Это неверное изображение");
    else
        QMessageBox::information(this, "Nice", "Это верное изображение");

}

void send_messege::on_button_back_clicked()
{
   emit this->change_wnd_to_homewnd();
}

void send_messege::set_intercept_info(QImage img, QString code, int i, int j)
{
    flag_new_image = true;
    flag_rsa_used = false;
    mc = 45;

    Encrypted_image = img;
    Loaded_image = img;
    int widch = ui->img_changed->width();
    int heich = ui->img_changed->height();
    ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image.scaled(widch,heich)));

    int wid = ui->img_original->width();
    int hei = ui->img_original->height();
    ui->img_original->setPixmap((QPixmap::fromImage(Loaded_image.scaled(wid,hei))));

    now_using_rune_code = code;
    this->set_position_of_img(i-1, j-1);

    this->on_button_send_messege_clicked();
}

QImage send_messege::encrypt_img_to_intercept(QImage img, QString pkey, int pkey_size,
                                            QString skey, int skey_size, QString algoritm)
{
    int generator_start = 0;
    int pkey_size2 = pkey.size();
    for (int i = 0; i < pkey_size2; i++)
    {
        generator_start = generator_start + pkey[i].toLatin1();
    }
    vector<int> pbk = pblok_key(pkey_size, generator_start);

    generator_start = 0;
    int skey_size2 = skey.size();
    for (int i = 0; i < skey_size2; i++)
    {
        generator_start = generator_start + skey[i].toLatin1();
    }
    vector<vector<int>> sbk = sblok_like_vigener_key(skey_size, generator_start);

    int algoritm_size2 = algoritm.size();
    for (int i = 0; i<algoritm_size2; i++)
    {
        if(algoritm[i]=='P')
            img = encrypt_image_p(img, pbk);
        else
            img = encrypt_image_s(img, sbk);
    }

    return img;
}

void send_messege::on_button_crypt_rsa_clicked()
{
    if (!flag_rsa)
    {
        QMessageBox::information(this, "error", "Ключи RSA не сгенерированы. Вернитесь в домашнее окно");
        return;
    }

    if (!flag_new_image)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"warning",
                              "Вы применяете алгоритм шифрования RSA "
                              "это приведет к тому, что предыдущие изменения будут отменены "
                              "Вы уверены в своем решении?",
                              QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            Encrypted_image = Loaded_image;
            flag_new_image = true;

            int lblwid = ui->img_changed->width();
            int lblhei = ui->img_changed->height();
            ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image).scaled(lblwid,lblhei));
        }
        else
            return;
    }

    /* Сделать неактивным */
    this->setEnabled(false);

    /* Забить временем */
    ui->lbl_progress->setVisible(true);
    ui->progress_send->setVisible(true);
    for (int i = 0; i < 100; i++)
    {
        QEventLoop loop;
        QTimer::singleShot(300, &loop, SLOT(quit()));       // Время 300
        loop.exec();
        loop.exit();
        ui->progress_send->setValue(i+1);
    }

    ui->lbl_progress->setVisible(false);
    ui->progress_send->setVisible(false);

    this->setEnabled(true);

    vector <int> data = image_to_vector(Encrypted_image);
    vector<int> crypt_data = crypt_rsa2(data, e, n);
    Encrypted_image = set_vector_at_image(Encrypted_image, crypt_data);

    int lblwid = ui->img_changed->width();
    int lblhei = ui->img_changed->height();
    ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image).scaled(lblwid,lblhei));

    ui->lbl_algoritm_value->setText("RSA");

    flag_new_image = false;
    flag_rsa_used = true;
}

void send_messege::up_level(int level)
{
    ui->button_p_key_generate->setEnabled((level==3)||(level>=5));

    if (level == 4)
    {
        if (Encrypted_image != Loaded_image)
        {
            Encrypted_image = Loaded_image;
            int lblwid = ui->img_changed->width();
            int lblhei = ui->img_changed->height();
            ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image).scaled(lblwid,lblhei));
            flag_new_image = true;
            ui->lbl_algoritm_value->setText("");
        }
    }

    ui->button_s_key_generate->setEnabled(level>=4);
    ui->button_crypto_cansel->setEnabled(level>=3);
    ui->button_algoritm_crypto->setEnabled(level>=3);
    ui->button_algoritm_crypto_delete->setEnabled(level>=3);

    ui->button_crypt_rsa->setEnabled(level>=9);

    ui->button_choose_img->setEnabled(level>=1);

    ui->button_make_img->setEnabled(level>=3);

    ui->button_crypto_p->setEnabled(((level==3)||(level>=5))&&(!p_key.empty()));
    ui->button_crypto_s->setEnabled((level>=4)&&(!s_key.empty()));
}

void send_messege::on_button_cancel_rsa_clicked()
{
    if (!flag_rsa_used)
    {
        QMessageBox::information(this, "error", "Не применено шифрование RSA");
        return;
    }

    vector<int> data = image_to_vector(Encrypted_image);
    vector<int> decrypted_data = decrypt_rsa2(data, e, n);
    Encrypted_image = set_vector_at_image(Encrypted_image, decrypted_data);

    int lblwid = ui->img_changed->width();
    int lblhei = ui->img_changed->height();
    ui->img_changed->setPixmap(QPixmap::fromImage(Encrypted_image).scaled(lblwid,lblhei));

    ui->lbl_algoritm_value->setText("");

    flag_new_image = true;
    flag_rsa_used = false;
    mc = 75;
}

void send_messege::on_check_box_crypt_keys_clicked()
{
    emit this->using_ass_keys_or_not(ui->check_box_crypt_keys->checkState());
}

void send_messege::set_ass_key(int nn, int ee, int dd)
{
    n = nn;
    e = ee;
    d = dd;
    flag_rsa = true;
    ui->check_box_crypt_keys->setEnabled(true);
}
