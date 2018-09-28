/*
 * Домашнее окно пользователя
 * Mode = player
 * В данном окно пользователь может открывать различыне окна путем посыла сигналов в MainWindow
 * Окна, которые можно открыть: 1) send_messege
 *                              2) ...
*/

#ifndef HOME_WINDOW_H
#define HOME_WINDOW_H

#include <QWidget>
#include <QObject>
#include <QString>
#include <QFileDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QPixmap>
#include <QImage>
#include <QVBoxLayout>
#include <QImage>
#include <QTimer>

#include <vector>

#include "qpb_modify.h"
#include "send_messege.h"

using namespace std;

namespace Ui {
class home_window;
}

class home_window : public QWidget
{
    Q_OBJECT

public:
    explicit home_window(QString str_login, QWidget *parent = 0);
    ~home_window();
    void create_img_buttons(vector<vector<QImage>> cut, int n, int m,
                            vector<vector<QString>> codes,
                            vector<vector<QString>> mess,
                            QIcon icon_test);             /* Функция, которая создает иконки изображений (QpusgButton с натянутым QIcon),
                                                                         * каждая кнопка по клику на неё вызывает сигнал home_window::i_opend(QImage, int, int)
                                                                         * После генерации каждой кнопки шлет сигнал home_window::do_it() в Main_window
                                                                         * для того, чтобы создать идентичную кнопку в окне choose_button
                                                                         */

    void add_new_player(QString new_player_login);                      // Добавляет нового пользователя в список пользователей (ui->list_users)

    vector<vector<QString>> set_messeges (vector<vector<QString>> msg){return msg;}
    vector<vector<QImage>> set_runes(vector<vector<QImage>> runes) {return runes;}

    void set_visibale_new_messege(bool vis);                            // Делает изображение с конвертом видимым и нет
    void i_find_image_bf(int i, int j);
    void set_lvl_label(QString lvl);
    void up_level(int level);
    void set_button_was_sending(int i, int j);
    void delete_player(QString login);
    void set_flag_offline(bool t){flag_offline = t;}

signals:
    void change_wnd_to_swnd();             /* Сигнал для переключения с home_window на send_messege */

    void do_it(int, int);                  /* Сигнал для создания кнопок в окне choose_button */

    void i_opend(QImage,int, int, QString);/* Сигнал, который шлет созданная кнопка в home_window в main_window
                                            * для того, чтобы сообщить о том, что кнопка выбрана пользователем
                                            */

    void get_source_image(int, int);        /* Сигнал, который окно шлет в main_window,
                                             *для того, чтобы оно ему вернуло source_image (присланное root)
                                             * с индексом i, j
                                             */

    void show_intercept_wnd_please();       // Просит показать окно перехвата
    void i_want_intercept(QString);         // Передается в mainwindow логин игрока, которого будем перехватывать
    void from_home_wnd_to_rsa_wnd();

private slots:
    QImage i_get_source_img_for_u(QImage img){return img;}  //
    int i_get_n_for_u(int n){return n;}                     //
    int i_get_m_for_u(int m){return m;}                     //
    void on_button_overhear_messege_clicked();              // Переход в окно overhere - intercept
    void on_button_inercept_clicked();
    void on_automat_clicked();
    void rand_image(int c);
    void rotate_rulet();

    void on_button_assimetry_clicked();
private:
    Ui::home_window *ui;
    QImage original_img;
    vector <vector<QPB_modify *>> icons;
    int count_777 = 0;
    bool flag_offline = false;
    int count_timer = 0;
    QTimer *timer;

    std::vector<QImage> runes =
    {
        QImage(":/runes/a.png"),
        QImage(":/runes/b.png"),
        QImage(":/runes/c.png"),
        QImage(":/runes/ch.png"),
        QImage(":/runes/d.png")
    };
};

vector <vector<QImage>> cut_image(QImage &image, int n, int m); /* Функция, которая разрезает исходное изображение на n*m частей.
                                                                 * Для игры локально
                                                                 */


#endif // HOME_WINDOW_H
