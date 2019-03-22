#include "interception.h"
#include "ui_interception.h"

interception::interception(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::interception)
{
    ui->setupUi(this);
    lay = new QGridLayout;
    ui->scroll_inercept_content->setLayout(lay);

    connect(this, SIGNAL(show_info_at_messege(int)), this, SLOT(show_info_at_messege_on_index(int)) );
    this->setWindowTitle("Перехват сообщений");
    int level = 1;
    ui->button_decrypt->setEnabled(level>=4);
}

interception::~interception()
{
    delete ui;

    for (int i = 0; i < (int)buttons_of_messege.size(); i++)
        delete buttons_of_messege[i];

    clearLayout(lay);
    delete lay;
}

void interception:: clearLayout( QLayout* layout )
{
    QLayoutItem* item = Q_NULLPTR;
    while( (item = layout->takeAt(0)) )
    {
        if (item->layout())
        {
            clearLayout( item->layout() );
            delete item->layout();
        }
        else if ( item->widget() )
            delete item->widget();
        else if( item->spacerItem() )
            delete item->spacerItem();

        delete item;
    }
}

void interception::on_button_back_clicked()
{
    emit this->homecomig();
}

void interception::add_new_messege(QImage img, QImage orig_image,
                                   QString p_key, int p_key_size,
                                   QString s_key, int s_key_size,
                                   int i, int j, QString algoritm, QString code)
{
    inter_messege msg;
    msg.img = img;
    msg.original_image = orig_image;
    msg.p_key = p_key;
    msg.p_key_size = p_key_size;
    msg.s_key = s_key;
    msg.s_key_size = s_key_size;
    msg.i = i;
    msg.j = j;
    msg.algoritm = algoritm;
    msg.code = code;
    intercepted_messeges.push_back(msg);

    QPB_modify *pb = new QPB_modify;
    pb->i = intercepted_messeges.size() - 1;
    pb->setText("Msg from " + login_of_intercept + " №" + QString::number(pb->i));
    connect(pb, &QPushButton::clicked, [this, pb](){
        emit this->show_info_at_messege(pb->i);
    });

    lay->addWidget(pb, pb->i, 0);
}

void interception::show_info_at_messege_on_index(int i)
{
    inter_messege msg = intercepted_messeges[i];
    index_message = i;
    flag_now_using_message = true;

    ui->image_intercept->setPixmap(QPixmap::fromImage(msg.img).scaled(ui->image_intercept->size()));
    ui->lbl_algoritm_value->setText(msg.algoritm);
    ui->lbl_s_size_value->setText(QString::number(msg.s_key_size));
    ui->lbl_p_size_value->setText(QString::number(msg.p_key_size));
    ui->lbl_s_key_vlaue->setText(msg.s_key);
    ui->lbl_p_key_value->setText(msg.p_key);
    ui->lbl__i_value->setText(QString::number(msg.i));
    ui->lbl_j_value->setText(QString::number(msg.j));

    ui->image_intercept_dec->setPixmap(QPixmap::fromImage(QImage(":/images/question.png")));

    if (msg.algoritm == "RSA")
    {
        QMessageBox::information(this, "omg", "Этот игрок зашифровал сообщение с помощью алгоритма RSA, расшифровать не получится. "
                                              "Не самый умный ход с его стороны, не так ли?");
    }

    flag_decr = false;
}

void interception::on_button_to_send_messege_clicked()
{
    if (flag_now_using_message == false)
    {
        QMessageBox::information(this, "error", "Не выбрано сообщение");
        return;
    }

    if (flag_decr == false)
    {
        QMessageBox::information(this, "error", "Не удалось расшифровать сообщение");
        return;
    }

    QString code;
    QImage img;
    if (flag_good_decr == true)
    {
        img = intercepted_messeges[index_message].original_image;
        code = intercepted_messeges[index_message].code;
    }
    else
    {
        img = decr_img;
        code = "100_100";
    }



    emit this->go_to_crypto(img,
                            code,
                            intercepted_messeges[index_message].i,
                            intercepted_messeges[index_message].j);
}

void interception::set_rsa_info(QImage img, QString code)
{
    inter_messege msg;
    msg.img = img;
    msg.p_key = "none";
    msg.p_key_size = 0;
    msg.s_key = "none";
    msg.s_key_size = 0;
    msg.i = 0;
    msg.j = 0;
    msg.algoritm = "RSA";
    msg.code = code;
    intercepted_messeges.push_back(msg);

    QPB_modify *pb = new QPB_modify;
    pb->i = intercepted_messeges.size() - 1;
    pb->setText("Msg from " + login_of_intercept + " №" + QString::number(pb->i));
    connect(pb, &QPushButton::clicked, [this, pb](){
        emit this->show_info_at_messege(pb->i);
    });

    lay->addWidget(pb, pb->i, 0);
}

void interception::on_button_decrypt_clicked()
{
    if (flag_now_using_message == false)
    {
        QMessageBox::information(this, "error", "Не выбрано перехваченное сообщение");
        return;
    }

    flag_good_decr = false;
    inter_messege msg = intercepted_messeges[index_message];
    bool flag_must_be_undecrypt = true;     // Определяет, показать расшифрованное сообщение или нет
    QImage img = msg.img;
    if (msg.algoritm == "RSA")
    {
        QMessageBox::information(this, "error", "Не получилось расшифровать. "
                                                 "Использовался алгоритм RSA. "
                                                 "Будет произведена попытка расшифровать на случайном ключе");
        std::vector<int> data = image_to_vector(img);
        std::vector<int> crypted_data = crypt_rsa2(data, rand()%2500, rand()%5000);
        img = set_vector_at_image(img, crypted_data);

        flag_must_be_undecrypt = false;
    }

    if ((msg.p_key == "unknown") && (msg.algoritm != ""))
    {
        QMessageBox::information(this, "error", "Ключи неизвестны, использовался алгоритм RSA для передачи ключей"
                                                "Будет произведена попытка расшифровать на случайном ключе!");

        srand(msg.p_key_size+ msg.s_key_size);
        std::vector<int> pk;
        pk = pblok_key(msg.p_key_size + rand()%30, rand()%5000);
        std::vector<std::vector<int>> sk;
        sk = sblok_like_vigener_key(msg.s_key_size + rand()%30, rand());

        std::string str = msg.algoritm.toStdString();
        for (int i = 0; i < (int)str.size(); i++)
            if (str[i] == 'P')
                img = encrypt_image_p(img, pk);
            else
                img = encrypt_image_s(img, sk);

        flag_must_be_undecrypt = false;
    }

    if (flag_must_be_undecrypt)
    {
        img = msg.original_image;
        flag_good_decr = true;
    }

    decr_img = img;

    int wid = ui->image_intercept_dec->width();
    int hei = ui->image_intercept_dec->height();
    ui->image_intercept_dec->setPixmap(QPixmap::fromImage(img).scaled(wid, hei));

    flag_decr = true;
}

void interception::up_level(int level)
{
    ui->button_decrypt->setEnabled(level>=6);
}
