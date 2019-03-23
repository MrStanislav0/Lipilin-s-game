#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fun.h"
#include <QThread>

#define PORT_SERVER 15809

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Мастер-класс Криптография");

     root_address = getIpRoot();
     if (root_address == "")
     {
         QMessageBox::information(this,"Error", "IP адрес рута не задан!");
     }

    ui->Login_button->setEnabled(false);
    QRegExp is_login_variable("[a-zA-Z0-9]{1,20}");
    ui->login_edit->setValidator(new QRegExpValidator(is_login_variable, this));
    connect(ui->login_edit,SIGNAL(textChanged(QString)),this,SLOT(ok_enabled()));

    socket = new QUdpSocket(this);
    if (!socket->bind(QHostAddress::AnyIPv4, PORT_SERVER))
    {
        QMessageBox::information(this,"Error", "Порт закрыт!");
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(NET_datagramm_analysis())); // ловим udаграммыp дейт и анализируем

    QMessageBox::information(this,"Welcome", "Добро пожаловать! Для старта игры введите логин!");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::rootwindow()
{
    flag_is_it_root = true;

    root_wnd = new root_window;
    make_wnd = new make_img_window;

    connect(root_wnd, SIGNAL(show_make_img_with_my_img(QImage, int, int, QString)), this, SLOT(show_make_wnd_to_root(QImage, int, int, QString)));
    connect(root_wnd, SIGNAL(get_rune(int, int)), this, SLOT(set_rune(int, int)));
    connect(make_wnd, SIGNAL(rejected()), this, SLOT(if_close_wnd_fo_root()));
    connect(make_wnd, SIGNAL(root_make_new_img(QImage, int, int, QString, QString)), this, SLOT(new_rune_created_root(QImage, int, int, QString, QString)));
    connect(root_wnd, SIGNAL(start()), this, SLOT(NET_send_info_for_start()));

    connect(root_wnd, SIGNAL(lvl_up()), this, SLOT(NET_send_lvl()));

    root_wnd->show();
    this->close();
}

void MainWindow::playerwindow()
{

    QMessageBox::information(this,"Правила", "Позволь объяснить тебе смысл игры. "
                                             "Ты - агент, которому нужно стать лучшим среди всех. "
                                             "Штаб послал всем агентам сообщение, в котором закодирована важная информация."
                                             "Твоя задача - как можно быстрее восстановить сообщение штаба. "
                                             "Но помни - ты здесь не один. Другие агенты тоже хотят победить. "
                                             "И путь к победе не всегда бывает честным...");

    QString name_login = ui->login_edit->text();

    home_wnd = new home_window(name_login);
    send_messege_wnd = new send_messege;
    choose_button_wnd = new choose_button;
    make_wnd = new make_img_window;
    intercept_wnd = new interception;
    rsa_wnd = new asimetry;

    connect(send_messege_wnd, SIGNAL(change_wnd_to_homewnd()), this, SLOT(sendmess_wnd__home_wnd()));
    connect(home_wnd, SIGNAL(change_wnd_to_swnd()),this ,SLOT(home_wnd__sendmess_wnd()));

    connect(home_wnd, SIGNAL(do_it(int, int)), this, SLOT(create_pb(int, int)));
    connect(home_wnd, SIGNAL(i_opend(QImage, int, int,QString)), this, SLOT(then_opend_img(QImage, int, int, QString)));
    connect(send_messege_wnd, SIGNAL(show_ch_buttons_sign()), this, SLOT(show_ch_wnd()));
    connect(choose_button_wnd, SIGNAL(i_choose_img(QImage, int, int, QString)),this ,SLOT(then_choosen_img(QImage, int, int, QString)));
    connect(choose_button_wnd, SIGNAL(rejected()), this, SLOT(if_close_wnd()));

    connect(send_messege_wnd, SIGNAL(show_make_img_wnd()),this, SLOT(show_make_wnd()));
    connect(make_wnd, SIGNAL(rejected()), this, SLOT(if_close_wnd()));
    connect(make_wnd, SIGNAL(i_make_img(QImage, QString)), this, SLOT(then_made_img(QImage, QString)));

    connect(send_messege_wnd, SIGNAL(player_send_messege(QString,QString,int,QString,int,int,int, QString)),
            this, SLOT(test_player_image(QString,QString,int,QString,int,int,int, QString)));

    connect(home_wnd, SIGNAL(show_intercept_wnd_please()), this, SLOT(show_intercept_window()));
    connect(intercept_wnd, SIGNAL(homecomig()), this, SLOT(back__homecoming()));
    connect(home_wnd, SIGNAL(i_want_intercept(QString)), this, SLOT(NET_send_players_inercept_login(QString)));

    connect(intercept_wnd, SIGNAL(go_to_crypto(QImage,QString,int,int)),
            this, SLOT(send_messege_wnd_on_intercept_value(QImage,QString,int,int)));

    connect(home_wnd, SIGNAL(from_home_wnd_to_rsa_wnd()), this, SLOT(show_rsa_wnd()));
    connect(rsa_wnd, SIGNAL(rejected()), this, SLOT(back_from_rsa_wnd()));
    connect(rsa_wnd, SIGNAL(from_rsa_to_home_wnd()), this, SLOT(back_from_rsa_wnd()));
    connect(rsa_wnd, SIGNAL(keys_was_generated(int,int,int)),this, SLOT(use_assimetry_crypto(int,int,int)));

    connect(send_messege_wnd, SIGNAL(send_rsa_messege(QString,int,int, int, int)),
            this, SLOT(NET_send_rsa_img(QString,int,int, int, int)));

    connect(send_messege_wnd, SIGNAL(using_ass_keys_or_not(bool)), this, SLOT(set_flag_assimetry(bool)));

    if (root_address == "127.0.0.1")
        solo();

    home_wnd->show();
    this->close();
}

void MainWindow::ok_enabled()
{
    ui->Login_button->setEnabled(ui->login_edit->hasAcceptableInput());
}

void MainWindow::on_Login_button_clicked()
{   
    if(ui->login_edit->text() == "root")
        rootwindow();
    else
    {
		QThread::msleep(200);
        QByteArray data;
        data.append("0r");
        data.append(ui->login_edit->text());
        socket->writeDatagram(data, QHostAddress(root_address), PORT_SERVER);
    }

}

void MainWindow::home_wnd__sendmess_wnd()
{
    home_wnd->close();
    send_messege_wnd->show();
}

void MainWindow::sendmess_wnd__home_wnd()
{
    send_messege_wnd->close();
    home_wnd->show();
}

void MainWindow::show_ch_wnd()
{
    send_messege_wnd->setEnabled(false);
    choose_button_wnd->show();
}

void MainWindow::create_pb(int i, int j)
{
    QPB_modify* pb = new QPB_modify;
    QSize button_size(50,50);
    pb->setMaximumSize(button_size);
    pb->setMinimumSize(button_size);

    pb->setIcon(icon_test);
    QSize icon_size(40,40);
    pb->setIconSize(icon_size);

    pb->rune_code = runes_code[i][j];

    connect(pb, &QPushButton::clicked, [this, pb](){
        if (pb->reverse_img.isNull())
            QMessageBox::information(this,"Ошибка", "Не выбрана картинка!");
        else
            emit choose_button_wnd->i_choose_img(pb->reverse_img, pb->i, pb->j, pb->rune_code);
    });

    choose_button_wnd->set_button(pb, i, j);
}

void MainWindow::then_choosen_img(QImage img, int i, int j, QString code)
{
    send_messege_wnd->user_choose_img(img, code);
    send_messege_wnd->set_position_of_img(i, j);

    send_messege_wnd->setEnabled(true);
    send_messege_wnd->show();
}

void MainWindow::then_opend_img(QImage img, int i, int j, QString code)
{
    choose_button_wnd->open_button(img, i, j, code);
}

void MainWindow::if_close_wnd()
{
    send_messege_wnd->setEnabled(true);
    send_messege_wnd->show();
}

void MainWindow::show_make_wnd()
{
    send_messege_wnd->setEnabled(false);
    make_wnd->show();
}

void MainWindow::set_rune(int rune, int color)
{
    root_wnd->set_temp_rune(make_wnd->paint_picture(make_wnd->get_rune(rune),
                                                   make_wnd->get_color(color)));
}

void MainWindow::show_make_wnd_to_root(QImage img, int i, int j, QString str)
{
    root_wnd->setEnabled(false);
    make_wnd->set_rune(img, i, j, str);
    make_wnd->show();
}

void MainWindow::if_close_wnd_fo_root()
{
    root_wnd->setEnabled(true);
}

void MainWindow::new_rune_created_root(QImage img ,int i, int j, QString str, QString img_code)
{
    QPB_modify pb;

    pb.i = i;
    pb.j = j;
    pb.str = str;
    pb.rune_code = img_code;

    pb.reverse_img = img;
    QSize button_size(50,50);
    pb.setMaximumSize(button_size);
    pb.setMinimumSize(button_size);

    pb.setIcon(QIcon(QPixmap::fromImage(img)));
    QSize icon_size(pb.size());
    pb.setIconSize(icon_size);

    connect(&pb, &QPushButton::clicked, [this, &pb](){
        emit root_wnd->show_make_img_with_my_img(pb.reverse_img, pb.i, pb.j, pb.str);
    });

    root_wnd->set_rune_at_GL(&pb, i, j);
}

void MainWindow::then_made_img(QImage img, QString code)
{
    send_messege_wnd->user_made_img(img, code);

    send_messege_wnd->setEnabled(true);
    send_messege_wnd->show();
}

void MainWindow::NET_datagramm_analysis()
{
    while(socket->hasPendingDatagrams())
    {
        QByteArray buffer; //Дейтаграмма
        buffer.resize(socket->pendingDatagramSize());
        QHostAddress sender; // IP отправителя
        quint16 senderPort; // Port

        socket->readDatagram(buffer.data(), buffer.size(),&sender, &senderPort);
        QString data(buffer);
        if ((int)data.size() == 0)
            continue;

        QString temp_data = data;

        if (temp_data.left(6) == "level:")
        {
            QString temp = "";
            temp_data.replace(0, 6, temp);

            send_messege_wnd->up_level(temp_data.toInt());
            home_wnd->set_lvl_label(temp_data);
            home_wnd->up_level(temp_data.toInt());
            intercept_wnd->up_level(temp_data.toInt());

            continue;
        }

        char who = QCharRef(data[0]).toLatin1();
        char action = QCharRef(data[1]).toLatin1();
        data.remove(0, 2); // Удаляются первые 2 символа т.к. они уже записаны в who, data

        //анализируем дейтаграмму
        switch (action)
        {
        case 'r':
            if (who == '0') // Отправитель player
                NET_registration_for_root(data ,sender);
            else
                NET_registration_for_player(data);
            break;

        case 'n':
            NET_add_player (data, sender);
            break;

        case 'S':
            NET_start_messeges_phase_1(data);
            break;

        case 'g':
            create_source_images();
            break;
        case 'i':
            if (who == '0')
                NET_players_intercept_for_root(data, sender);
            else
                NET_players_intercept_for_player(data);

            break;

        case 'a':
            NET_list_of_user_in_game(data, sender);
            break;

        case 'I':
            NET_add_intercepted_messege(data);
            break;

        case 'R':
            NET_set_rsa_intercept_mess(data);
            break;
        }
    }
}

void MainWindow::NET_registration_for_root(QString login, QHostAddress sender)
{
    /* Проверка на уникальность адреса */
    QMap<QString, QString>::iterator it;
    QString verdict = "true";

    for (it = user_list.begin(); it!=user_list.end(); it++)
    {
        if (it.value() == login)
        {
            verdict = "false";
            break;
        }
    }

    /* Отправка ответа. Идет в начале, т.к. NET_a_new_player_come отсылает созданному пользователю информацию
     * Он должен быть создан перед тем, как ему отправится информация
     */
    QThread::msleep(200);
    QByteArray Data;
    Data.append("1r");
    Data.append(verdict);
    socket->writeDatagram(Data, sender, PORT_SERVER);

    if (verdict == "false")
        return;

    QThread::msleep(200);
    /* В случае, если уникальный, занесение в БД и оповещение других пользователей */

	if (user_list[sender.toString()] != "")
    {
        root_wnd->delete_player(user_list[sender.toString()]);
    }
    user_list[sender.toString()] = login;

    NET_a_new_player_come(login, sender.toString());

    if ((root_address!="127.0.0.1") && (login != "root"))
        root_wnd->add_new_player(login);

    if (flag_is_it_root)
    {
        //root_wnd->add_new_player(login, sender);
        if (root_wnd->get_flag_game_on())
            NET_send_info_for_player(sender.toString(), messeges, img_code);
    }
}

void MainWindow::NET_registration_for_player(QString verdict)
{
    if (verdict == "true")
        playerwindow();
    else
        QMessageBox::information(this, "Oops", "Такой логин уже занят, попробуй другой");
}

void MainWindow::NET_a_new_player_come(QString new_player_login, QString sender)
{
    QMap<QString, QString>::iterator it;
    QString users = "1a";

    for (it = user_list.begin(); it!=user_list.end(); it++)
    {
        if ((it.value() == "") || (it.value() == new_player_login))
            continue;

        users = users + it.value() + ' ' + it.key() + ' ';

        QHostAddress temp_addres(it.key());
		QThread::msleep(200);
        QByteArray Data;
        Data.append("1n");
        Data.append(new_player_login);
        Data.append(" ");
        Data.append(sender);
        Data.append(" ");
        socket->writeDatagram(Data, temp_addres, PORT_SERVER);
    }
	QThread::msleep(200);
    QByteArray Data;
    Data.append(users);
    socket->writeDatagram(Data, QHostAddress(sender), PORT_SERVER);
}

void MainWindow::NET_add_new_player (QString login, QHostAddress sender)
{    
    home_wnd->add_new_player(login);
}

void MainWindow::NET_send_info_for_start()
{
    QMap<QString, QString>::iterator it;
    int n = root_wnd->get_n();
    int m = root_wnd->get_m();
    messeges = "";
    img_code = "";

    /* В сообщение добавляем n, m*/

    messeges = messeges + QString::number(n) + ' ';
    messeges = messeges + QString::number(m) + ' ';

    /* В начале получаем сообщение со строчками следующего вида:
     * "первое_слово второе_слово третье_слово ..."
     */

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
        {
            QString temp = root_wnd->get_messege_at_position(i, j);
            messeges = messeges + temp + ' ';
        }

    /* Форсмируется строка закодированных изображений */
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            img_code = img_code + root_wnd->get_rune_code_at_position(i, j) + ' ';

    /* Шлем данные */

    for (it = user_list.begin(); it!=user_list.end(); it++)
        NET_send_info_for_player(it.key(), messeges, img_code);
}

void MainWindow::NET_start_messeges_phase_1(QString messeges)
{
    QString temp_str;

    /* Вытаскиваем n */
    temp_str = cut_string_befor_simbol(messeges, ' ');
    img_count_n = temp_str.toInt();

    /* Вытаскиваем m */
    temp_str = cut_string_befor_simbol(messeges, ' ');
    img_count_m = temp_str.toInt();

    /* Вытаскиваем слова */
    for (int i = 0; i < img_count_n; i++)
    {
        vector<QString> temp_vec;
        for (int j = 0; j < img_count_m; j++)
        {
            temp_str = cut_string_befor_simbol(messeges, ' ');
            temp_vec.push_back(temp_str);
        }
        code_messege.push_back(temp_vec);
    }

    /* Генерируем изображения */
    for (int i = 0; i < img_count_n; i++)
    {
        vector<QString> temp_code_vec;
        for (int j = 0; j < img_count_m; j++)
        {
            temp_str = cut_string_befor_simbol(messeges, ' ');
            temp_code_vec.push_back(temp_str);
        }
        runes_code.push_back(temp_code_vec);
    }
}

int count_simbols_befor(QString data, char befor)
{
    bool do_it = true;
    int i = 0;
    int LenData = data.size();

    while (do_it)
    {
        if (LenData >= i)
        {
            return -1;
        }

        if (QCharRef(data[i]).toLatin1() != befor)
            i++;
        else
            do_it = !do_it;
    }

    return i;
}

QString cut_string_befor_simbol(QString &str, char befor)
{
    int temp;
    QString temp_str;

    temp_str = "";
    temp = count_simbols_befor(str, befor);

    if (temp == -1)
    {
        return temp_str;
    }

    temp_str = simbols_in_str_at_positions(str, 0, temp);
    str.remove(0, temp+1); // Компенсация пробела

    return temp_str;
}

QString simbols_in_str_at_positions(QString data, int position, int count)
{
    QString unswer = "";
    for (int i = position; i < position + count; i++)
    {
        unswer = unswer + data[i];
    }

    return unswer;
}

void MainWindow::test_player_image(QString code, QString p_key, int p_key_size, QString s_key, int s_key_size, int i, int j, QString algoritm)
{
    if ((i<=0) || (j<=0) || (i > img_count_n) || (j > img_count_m))
    {
        QMessageBox::information(this, "error", "Неправильыне координаты");
        return;
    }

    // В зависимости игра оффлайн или онлайн
    if ((i-1>= (int)runes_code.size()) || ((j-1) >=(int)runes_code[0].size()))
    {
        QMessageBox::information(this, "errr", "Не могу распознать изображение");
        return;
    }

    bool verdict = code == runes_code[i-1][j-1];
    if (root_address == "127.0.0.1")
        send_messege_wnd->players_img_verdict(verdict);
    else
        send_messege_wnd->players_img_verdict(verdict);

    if (verdict)
    {
        home_wnd->i_find_image_bf(i-1, j-1);
        home_wnd->set_button_was_sending(i-1, j-1);
    }

    for (int k = 0; k < int(me_overhere_addres_list.size()); k++)
        NET_send_intercepted_messege_for_player(me_overhere_addres_list[k],
                                                code, p_key, p_key_size,
                                                s_key, s_key_size, i, j, algoritm);
}

void MainWindow::show_intercept_window()
{
    home_wnd->close();
    intercept_wnd->show();
}

void MainWindow::back__homecoming()
{
    intercept_wnd->close();
    home_wnd->show();
}

void MainWindow::NET_send_players_inercept_login(QString login)
{
    if (i_overhear_login != "")
        NET_no_overhere_for_root(login);

    intercept_wnd->set_login_of_intercept(login);
	QThread::msleep(200);
    QByteArray Data;
    Data.append("0iyes ");
    Data.append(login);
    socket->writeDatagram(Data, QHostAddress(root_address), PORT_SERVER);
    i_overhear_login = login;
}

void MainWindow::NET_players_intercept_for_root(QString data, QHostAddress sender)
{
    QByteArray Data;
    Data.append("1i");
	
    QString action = cut_string_befor_simbol(data, ' ');
    Data.append(action);
    Data.append(' ');
	QThread::msleep(200);
    // Поиск в map по значению
    QHostAddress addres;
    QMap<QString, QString>::iterator it;
    for (it = user_list.begin(); it!=user_list.end(); it++)
    {
        if (it.value()==data)
        {
            addres = QHostAddress(it.key());
            Data.append(sender.toString());
            socket->writeDatagram(Data, addres, PORT_SERVER);
            break;
        }
    }
}

void MainWindow::NET_players_intercept_for_player(QString data)
{
    QString action = cut_string_befor_simbol(data, ' '); // Поиск да или нет
    if (action == "yes")
    {
        int i = me_overhere_addres_list.indexOf(data);
        if (i != -1)
        {
            me_overhere_addres_list.remove(i);
        }
        me_overhere_addres_list.push_back(data);
    }
    else
    {
        int i = me_overhere_addres_list.indexOf(data);
        if (i != -1)
        {
            me_overhere_addres_list.remove(i);
        }
    }
}

void MainWindow::NET_send_intercepted_messege_for_player (QString addres, QString code,
                                             QString p_key, int p_key_size,   
                                             QString s_key, int s_key_size, 
                                             int i, int j, QString algoritm)
{
    QThread::msleep(200);
	QByteArray Data;
    Data.append("1I");
    QString messege = "";

    messege = messege + p_key + ' ' + QString::number(p_key_size) + ' '
                      + s_key + ' ' + QString::number(s_key_size) + ' '
                      + QString::number(i) + ' ' + QString::number(j) + ' ' + algoritm + ' '
                      + code + ' ';

    if (flag_assimetry_done)
        messege = messege + '1' + ' ';
    else
        messege = messege + '0' + ' ';

    Data.append(messege);
    socket->writeDatagram(Data, QHostAddress(addres), PORT_SERVER);
}

void MainWindow::NET_no_overhere_for_root (QString login)
{
    QByteArray Data;
    Data.append("0ino ");
    Data.append(login);
    socket->writeDatagram(Data, QHostAddress(root_address), PORT_SERVER);
}

void MainWindow::NET_list_of_user_in_game(QString data, QHostAddress sender)
{
    while (data!="")
    {
        QString UserName = cut_string_befor_simbol(data, ' ');
        QString UserIp = cut_string_befor_simbol(data, ' ');

        user_list[UserIp] = UserName;

        NET_add_new_player(UserName, sender);
    }
}

void MainWindow::NET_send_info_for_player(QString address, QString &messeges, QString &codes)
{
    QHostAddress temp_addres(address);

    QString datagramm = messeges + codes;
    QThread::msleep(200);
    QByteArray first_data;
    first_data.append("1S");
    first_data.append(datagramm);
    socket->writeDatagram(first_data, temp_addres, PORT_SERVER);

    // В случае, если игра началась, а пользователь опоздал - сразу шлется старт
    if (root_wnd->get_flag_game_on())
    {
        QThread::msleep(200);
        QByteArray Data;
        Data.append("1g");
        socket->writeDatagram(Data, temp_addres, PORT_SERVER);

        QThread::msleep(200);
        int new_level = root_wnd->level;
        QString s = QString::number(new_level);
        QByteArray Data2;
        Data2.append("level:");
        Data2.append(s);
        socket->writeDatagram(Data2, temp_addres, PORT_SERVER);
    }

}

void MainWindow::NET_add_intercepted_messege(QString data)
{
    // Костыльненько, но должно работать
    QString p_key           = cut_string_befor_simbol(data, ' ');
    QString p_key_size_str  = cut_string_befor_simbol(data, ' ');
    QString s_key           = cut_string_befor_simbol(data, ' ');
    QString s_key_size_st   = cut_string_befor_simbol(data, ' ');
    QString i_str           = cut_string_befor_simbol(data, ' ');
    QString j_str           = cut_string_befor_simbol(data, ' ');
    QString algoritm        = cut_string_befor_simbol(data, ' ');
    QString code            = cut_string_befor_simbol(data, ' ');
    QString ass             = cut_string_befor_simbol(data, ' ');

    int p_key_size = p_key_size_str.toInt();
    int s_key_size = s_key_size_st.toInt();
    int i = i_str.toInt();
    int j = j_str.toInt();

    QImage image, crypt_image;
    int ic = cut_string_befor_simbol(code, '_').toInt();
    int jc = code.toInt();
    image = make_wnd->paint_picture_at_code(ic, jc);

    code = QString::number(ic) + '_' + QString::number(jc);
    crypt_image = send_messege_wnd->encrypt_img_to_intercept(image, p_key, p_key_size,
                                                              s_key, s_key_size,
                                                              algoritm);
    if (ass == "1")
    {
        p_key = "unknown";
        p_key_size = 0;
        s_key = "unknown";
        s_key_size = 0;
    }

    intercept_wnd->add_new_messege(crypt_image, image ,p_key,p_key_size,s_key,s_key_size,i, j, algoritm, code);
    home_wnd->set_visibale_new_messege(true);
}

void MainWindow::send_messege_wnd_on_intercept_value(QImage img, QString code, int i, int j)
{
    intercept_wnd->close();
    send_messege_wnd->show();
    send_messege_wnd->set_intercept_info(img, code, i, j);
}

void MainWindow::solo()
{
    img_count_n = 5;                          // Дефолтное значение
    img_count_m = 5;
    vector<vector<QImage>> source_img;        // Исходные изображения, которые прислыает root
    for (int i = 0; i < 16; i++)
    {
        vector <QString> temp_vec;
        vector <QImage> images;
        for (int j = 0; j < 6; j++)
        {
            QString code = QString::number(i) + '_' + QString::number(j);
            temp_vec.push_back(code);

            /* Создание и запись изображения */
            QImage img = make_wnd->paint_picture_at_code(i, j);
            images.push_back(img);
        }
        source_img.push_back(images);
        runes_code.push_back(temp_vec);
    }

    /* Вытаскивание закодированных строк */
    for (int i = 0; i < img_count_n; i++)
    {
        vector<QString> tmp;
        for (int j = 0; j < img_count_m; j++)
        {
            QString str = QString::number(i*img_count_m + j);
            tmp.push_back(str);
        }
        code_messege.push_back(tmp);
    }

    /* Старт игры */
    home_wnd->create_img_buttons(source_img, img_count_n, img_count_m, runes_code, code_messege, icon_test);
    send_messege_wnd->up_level(10);
    home_wnd->set_lvl_label("10");
    home_wnd->up_level(10);
    intercept_wnd->up_level(10);
    home_wnd->set_flag_offline(true);

}

void MainWindow::show_rsa_wnd()
{
    home_wnd->close();
    rsa_wnd->show();
}

void MainWindow::back_from_rsa_wnd()
{
    rsa_wnd->close();
    home_wnd->show();
}

void MainWindow::use_assimetry_crypto(int n,int e,int d)
{
    send_messege_wnd->set_ass_key(n, e, d);
}

void MainWindow::NET_send_rsa_img(QString code, int e, int n, int i, int j)
{
    if ((i<=0) || (j<=0) || (i > img_count_n) || (j > img_count_m))
    {
        QMessageBox::information(this, "error", "Неправильыне координаты");
        return;
    }

    // В зависимости игра оффлайн или онлайн
    bool verdict = code == runes_code[i-1][j-1];
    if (root_address == "127.0.0.1")
        send_messege_wnd->players_img_verdict(verdict);
    else
        send_messege_wnd->players_img_verdict(verdict);

    if (verdict)
        home_wnd->i_find_image_bf(i-1, j-1);

    QByteArray Data;
    Data.append("1R");
    QString message = "";
    message = code + ' ' + QString::number(e) + ' ' + QString::number(n) + ' ';
    Data.append(message);

    for (int k = 0; k < int(me_overhere_addres_list.size()); k++)
    {
		QThread::msleep(200);
        QString addres = me_overhere_addres_list[k];
        socket->writeDatagram(Data, QHostAddress(addres), PORT_SERVER);
    }
}

void MainWindow::NET_set_rsa_intercept_mess(QString data)
{
    QString code = cut_string_befor_simbol(data, ' ');
    QString kostill = code;
    QString r_id = cut_string_befor_simbol(code, '_');
    QString c_id = code;
    int r = r_id.toInt();
    int c = c_id.toInt();

    QString e_str = cut_string_befor_simbol(data, ' ');
    int e = e_str.toInt();

    QString n_str = cut_string_befor_simbol(data, ' ');
    int n = n_str.toInt();

    QImage img2 = make_wnd->paint_picture_at_code(r, c);
    vector<int> data2 = image_to_vector(img2);
    vector<int> crypt_data = crypt_rsa2(data2, e, n);
    img2 = set_vector_at_image(img2, crypt_data);

    intercept_wnd->set_rsa_info(img2, kostill);
    home_wnd->set_visibale_new_messege(true);
}

void MainWindow::NET_send_lvl()
{
   int new_level = root_wnd->level;
   QString s = QString::number(new_level);
    QMap<QString, QString>::iterator it;
   for (it = user_list.begin(); it!=user_list.end(); it++)
   {
       QHostAddress temp_addres(it.key());
		QThread::msleep(200);
       QByteArray Data;
       Data.append("level:");
       Data.append(s);
       socket->writeDatagram(Data, temp_addres, PORT_SERVER);
   }

}

void MainWindow::create_source_images()
{
    vector<vector<QImage>> source_images;
    for (int i = 0; i < img_count_n; i++)
    {
        vector<QImage> temp_vec;
        for (int j = 0; j < img_count_m; j++)
        {
            QString ic = runes_code[i][j];
            QString ir = cut_string_befor_simbol(ic, '_');
            int r = ir.toInt();
            int c = ic.toInt();
            QImage img = make_wnd->paint_picture_at_code(r, c);
            temp_vec.push_back(img);
        }
        source_images.push_back(temp_vec);
    }

    home_wnd->create_img_buttons(source_images, img_count_n, img_count_m, runes_code, code_messege, icon_test);

    home_wnd->up_level(1);
}

void MainWindow::NET_add_player (QString data, QHostAddress sender)
{
     QString UserName = cut_string_befor_simbol(data, ' ');
     QString UserIp = cut_string_befor_simbol(data, ' ');
    if (user_list[UserIp] != "")
    {
        int i = me_overhere_addres_list.indexOf(UserIp);
        if (i != -1)
        {
            me_overhere_addres_list.remove(i);
        }
        home_wnd->delete_player(user_list[UserIp]);
    }
    user_list[UserIp] = UserName;

    home_wnd->add_new_player(UserName);
}

QString MainWindow::getIpRoot()
{
    QString IpRoot = "";
    QFile file("kmzi_program.cfg");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return IpRoot;
    QTextStream in(&file);
    if (!in.atEnd())
    {
        IpRoot = in.readLine();
    }
    file.close();
    return IpRoot;
}
