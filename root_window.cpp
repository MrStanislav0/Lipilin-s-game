#include "root_window.h"
#include "ui_root_window.h"

root_window::root_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::root_window)
{
    ui->setupUi(this);

    QRegExp is_int("[0-9]{0,3}");
    ui->edit_n->setValidator(new QRegExpValidator(is_int, this));
    ui->edit_m->setValidator(new QRegExpValidator(is_int, this));

    p_colors = pblok_key(colors_size, time(0));

    QRegExp reg("[А-Я]{0,70}");
    ui->edit_text->setValidator(new QRegExpValidator(reg, this));

    QRegExp reg_nm("[0-9]{0,1}");
    ui->edit_m->setValidator(new QRegExpValidator(reg_nm, this));
    ui->edit_n->setValidator(new QRegExpValidator(reg_nm, this));

    /* Заглушка против неправомерных едйствий рута */
    ui->button_start->setEnabled(false);
    ui->button_level_down->setEnabled(false);
    ui->button_level_up->setEnabled(false);

    this->setWindowTitle("Мастер-класс Криптография");


    // Инициализация соответствия букв и рун (слева буквы, справа руны)

    letters["А"]=1;
    letters["Б"]=2;
    letters["В"]=0;
    letters["Г"]=9;
    letters["Д"]=5;
    letters["Е"]=6;
    letters["Ж"]=7;
    letters["З"]=3;
    letters["И"]=8;
    letters["Й"]=8;
    letters["К"]=9;
    letters["Л"]=10;
    letters["М"]=11;
    letters["Н"]=12;
    letters["О"]=13;
    letters["П"]=14;
    letters["Р"]=15;
    letters["С"]=3;
    letters["Т"]=16;
    letters["У"]=17;
    letters["Ф"]=0;
    letters["Х"]=7;
    letters["Ц"]=4;
    letters["Ч"]=4;
    letters["Ш"]=14;
    letters["Щ"]=16;
    letters["Ъ"]=15;
    letters["Ы"]=5;
    letters["Ь"]=2;
    letters["Э"]=6;
    letters["Ю"]=17;
    letters["Я"]=1;

    connect(ui->button_level_up, SIGNAL(clicked()), this, SLOT(Lvl_up_all()));
    connect(ui->button_level_down, SIGNAL(clicked()), this, SLOT(Lvl_down_all()));
}

void root_window::Lvl_up_all()
{
    if (level==max_level)
    {
        ui->level->setText(QString::number(level) + " Это максимальный уровень");
        return;
    }

    level++;
    ui->level->setText(QString::number(level));
    emit this->lvl_up();

}

void root_window::Lvl_down_all()
{
    if (level==1)
    {
        ui->level->setText(QString::number(level) + " Это минимальный уровень");
        return;
    }

    level--;
    ui->level->setText(QString::number(level));
    emit this->lvl_up();

}

root_window::~root_window()
{
    delete ui;
}

void root_window::on_button_apply_clicked()
{
    if ((ui->edit_m->text() == "") || (ui->edit_n->text() == ""))
    {
        QMessageBox::information(this, "Error", "Вы не указали один из параметров n, m");
        return;
    }

    n = ui->edit_n->text().toInt();
    m = ui->edit_m->text().toInt();

    if (n*m % 7 != 0)
    {
        QMessageBox::information(this, "Error", "n*m должно быть кратно 7");
        return;
    }

    if (ui->lbl_count_of_char_value->text().toInt()!= n*m)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "error", "Количество букв в тексте и рун не совпадает или не кратно количеству"
                                                     "цветов (7). Дополнить или обрезать текст?",
                              QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
            pad_message();
        else
            return;
    }

    // Удаление элементов, при задании новых n, m
    QLayoutItem *ch;
    while ((ch = ui->componate_alphabet_buttons->takeAt(0))!= 0)
    {
            ui->componate_alphabet_buttons->removeItem(ch);
            ch->widget()->deleteLater();
            delete ch;
    }

    create_images();
    ui->button_start->setEnabled(true);
}

void root_window::create_images()
{
    /* Очистка от предыдущей информации */
    pb_runes.clear();
    QLayoutItem *ch;
    while ((ch = ui->componate_alphabet_buttons->takeAt(0))!= 0)
    {
        ui->componate_alphabet_buttons->removeItem(ch);
        ch->widget()->deleteLater();
       delete ch;
    }

    mix_message();
    /* Генерация рун */
    for (int i = 0; i < n; i++)
    {
        std::vector<QPB_modify*> temp_vec;
        for (int j = 0; j < m; j++)
        {
            // задается руна, соответствующая букве, с определенным цветом
            int let = letters[messege[j + i*m]];
            int col = p_colors[(j + i*m)%colors_size];
            emit this->get_rune(let, col);

            QPB_modify *pb = new QPB_modify;
            pb->i = i;
            pb->j = j;
            pb->str = messege[j + i*m];

            pb->reverse_img = temp_rune;
            pb->rune_code = QString::number(let) + '_' + QString::number(col);

            QSize button_size(50,50);
            pb->setMaximumSize(button_size);
            pb->setMinimumSize(button_size);

            pb->setIcon(QIcon(QPixmap::fromImage(temp_rune)));
            QSize icon_size(pb->size());
            pb->setIconSize(icon_size);

            connect(pb, &QPushButton::clicked, [this, pb](){
                // Предыдущая реализация
                //emit this->show_make_img_with_my_img(pb->reverse_img, pb->i, pb->j, pb->str);
                QMessageBox::information(this, "", pb->str);
            });
            temp_vec.push_back(pb);
        }
        pb_runes.push_back(temp_vec);
    }

    /* Установка рун */
    for (int i = 0; i < (int)pb_runes.size(); i++)
        for (int j = 0; j < (int)pb_runes[i].size(); j++)
            ui->componate_alphabet_buttons->addWidget(pb_runes[i][j], i, j);
}

void root_window::on_button_default_clicked()
{
    n = 6;
    m = 7;
    messege = default_text;

    ui->edit_m->setText(QString::number(m));
    ui->edit_n->setText(QString::number(n));

    QString str = "";
    for (int i = 0; i < (int)messege.size(); i++)
        str = str + messege[i];

    ui->lbl_now_using_text->setText(str);
    ui->edit_text->setText(str);
    ui->lbl_count_of_char_value->setText(QString::number(messege.size()));
    ui->button_start->setEnabled(true);

    create_images();
}

void root_window::on_button_start_clicked()
{
    flag_gamego_on = true;
    emit this->start();
    QMessageBox::information(this,"Старт", "Игра началась.");

    ui->button_level_up->setEnabled(true);
    ui->button_level_down->setEnabled(true);
    ui->button_apply->setEnabled(false);
    ui->button_default->setEnabled(false);
    ui->button_start->setEnabled(false);
}

void root_window::set_rune_at_GL(QPB_modify *pb, int i, int j)
{
    ui->componate_alphabet_buttons->addWidget(pb, i, j);
    pb_runes[i][j] = pb;
}

QString root_window::get_rune_code_at_position(int i, int j)
{
   QString code;
   code = pb_runes[i][j]->rune_code;
   return code;
}

QString root_window::get_messege_at_position(int i, int j)
{
    int messege_size = messege.size();
    if (messege_size > i*m +j)
        return messege[i*m + j];
    else
        return "";
}

void root_window::add_new_player(QString login)
{
    ui->user_list->addItem(login);
}

void root_window::on_edit_text_editingFinished()
{
    messege.clear();

    QString text = ui->edit_text->text();
    QString new_text = "";
    int count = 0;
    int i = 0;

    while (i!=text.size())
    {
        if (text[i]!=' ')
        {
            count++;
            QString str = "";
            str = str + text[i];
            messege.push_back(str);
            new_text = new_text + str;
        }
        i++;
    }

    ui->lbl_now_using_text->setText(new_text);
    ui->lbl_count_of_char_value->setText(QString::number(count));
}

void root_window::delete_player(QString login)
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

void root_window::mix_message()
{
    vector<QString> new_mess;                       // Новый блок с сообщениями
    int count = (int)messege.size() / colors_size;  // Количество полных блоков

    for (int i = 0; i < count; i++)
    {
        /* Инициализация блока */
        vector<QString> block;
        for (int j = 0; j < colors_size; j++)
            block.push_back(messege[i*colors_size + j]);

        /* Перемешивание */
        for (int j = 0; j < colors_size; j++)
        {
            QString tmp = block[p_colors[j]];
            new_mess.push_back(tmp);
        }
    }

    messege = new_mess;
}

void root_window::pad_message()
{
    int size = messege.size();
    if (size > n*m)
    {
        while (size<n*m)
            messege.pop_back();

        QString str = "";
        for (int i = 0; i < (int)messege.size(); i++)
            str = str + messege[i];

        ui->lbl_now_using_text->setText(str);
        ui->edit_text->setText(str);
        ui->lbl_count_of_char_value->setText(QString::number(messege.size()));
        return;
    }

    std::vector<QString> pad;
    pad.resize(10);
    pad[0] = "Я";
    pad[1] = "ЯН";
    pad[2] = "ЮЛЯ";
    pad[3] = "СТАС";
    pad[4] = "ШПИОН";
    pad[5] = "КЛИЕНТ";
    pad[6] = "КАРТИНА";
    pad[7] = "ПОДДЕЛКА";
    pad[8] = "СМЕШАРИКИ";
    pad[9] = "ШИФРОВАНИЕ";

    while(size!= n*m)
    {
        int new_word = 1 + rand()%pad.size();
        if (size + new_word <= n*m)
        {
            size = size + new_word;
            for (int i = 0; i < (int)pad[new_word-1].size(); i++)
            {
                QString str = "";
                str = str + pad[new_word-1][i];
                messege.push_back(str);
            }
        }
    }

    QString strt = "";
    for (int i = 0; i < (int)messege.size(); i++)
        strt = strt + messege[i];

    ui->lbl_now_using_text->setText(strt);
    ui->edit_text->setText(strt);
    ui->lbl_count_of_char_value->setText(QString::number(messege.size()));
}

