/*
 * MainWindow является центральной частью программы.
 * Все остальные окна общаются межд у собой путем проброса сигналов через MainWindow
 * Существует два варианта работы MainWindow: для root и для player
 * В начале комментария к любым атрибутам, методам, сигналам и слотам MainWindow
 * указано, к какому режиму относится данный объект
 *
 * П.С. поскольку у нас говнокод, а кодить меня никто не учил, часть вещей, которые для рута - скрыты
*/

/* Протокол работы с сетью.
 * Любое сообщение представляет собой следующий вид:
 * |-----------------------------------
 * |char who | char action | data ...
 * |-----------------------------------
 * | 1 byte  | 1 byte      | ...
 * |-----------------------------------
 *
 * I (поле who)
 *      1) who = 0 - любой игрок, в дальнейшем player
 *      2) who = 1 - штаб, в дальнейшем root
 *      В данный момент поле who содержит избыточность, которая внесена для возможного добавления других действующих лиц (не баг, а фича)
 *
 * II (поле action)
 *      1) action = r - регистрация игроков
 *         data = QString login (в случае если who = 0, т.е. отправляет пользователь)
 *         data = QString verdict (в случае, если who = 1, т.е. отправляет root)
 *
 *      2) action = n
 *         data = QString new_player_login
 *
 *      3) action = S (S заглавная)
 *         data = n, m записанные через пробел в string, после чего n*m слов, записанных через пробел
 *                     через пробел далее идут коды изображений. Код имеет вид i_j,
 *                                                               где i - номер руны
 *                                                                   j - номер цвета
 *
 *
 *         action = g - root завершил передачу исходных данных всем пользователям, game_must_go_on!
 *                      запускает старт игры
 *
 *      4) action = i - intercept (перехват)
 *         data = yes/no + QString login (в случае если who = 0, т.е. отправляет пользователь)
 *         data = yes/no + QString intercept_addr (в случае, если who = 1, т.е. отправляет root)
 *
 *         action = I - уже перехваченное сообщение
 *         data = QString messege + QImage = concat(p_key || p_key_size || s_key || s_key_size || i || j) + QImage
 *
 *      5) action = a - add users
 *         Посылает вновь прибывшему пользователю информацию о всех игроках в сети
 *         data = Qstring "..." через пробел логины пользователей: for example ("1auser1 user2 user3")
 *
 *      6) action = l - указывает уровень (возможности пользователя)\
 *         data = 1 - только P
 *              = 2 - PS
 *              = 3 - (PS)^n, ключи перехватываются
 *              = 4 - ассиметричная криптография
 *         Согобщение, согласно протоколу имеет вид: "1l 2"
 *
 * III (примечания)
 * Задействован QUdpSocket, следовательно, общаются UDP пакетами
 * При получении сигнала "readyRead" сокет вызывает слот void NET_datagramm_analysis(), который анализирует полученный пакет
 * Все, что связано с сетью имеет префикс NET_
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QUdpSocket>
#include <QMap>
#include <QTime>
#include <QBuffer>
#include <QVector>
#include <QIcon>

#include <vector>

#include "home_window.h"
#include "send_messege.h"
#include "choose_button.h"
#include "root_window.h"
#include "make_img_window.h"
#include "interception.h"
#include "asimetry.h"
#include "fun.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
/**********************************************************************************************/
    void rootwindow();   /* Mode = root;                                                      //
                          * Функция, формирующая окно root и connect для него                 //
                          */                                                                  //
                                                                                              //
    void playerwindow(); /* Mode = player;                                                    //
                          * Функция, формирующая окно player и connect для него               //
                          */                                                                  //
                                                                                              //
/**********************************************************************************************/

private slots:
/************************* Слоты для анализа введенного в окне регистрации (mainwindow) ***************************************/
    void ok_enabled();                      /* Mode = any;                                                                    //
                                             * Слот, который проверяет, ввёл ли пользователь верное значение в поле "логин"   //
                                             */                                                                               //
                                                                                                                              //
    void on_Login_button_clicked();         /* Mode = any;                                                                    //
                                             * Слот, анализирующий "логин" и выполняющий дальнейшие действия                  //
                                             */                                                                               //
                                                                                                                              //                                                                       //
                                                                                                                              //
/**************************************************************************************************************************** */


/****************************************************** Cлоты для переключения между окнами *********************************************** */
    void home_wnd__sendmess_wnd(); /* Mode = player;                                                                                        //
                                    * Слот, позволяющий переключиться между окном home_window и send_messege                                //
                                    */                                                                                                      //
                                                                                                                                            //
    void sendmess_wnd__home_wnd();/* Mode = player;                                                                                         //
                                   * Слот, позволяющий переключиться между окном send_messege и home_window                                 //
                                   */                                                                                                       //
                                                                                                                                            //
    void show_ch_wnd();/* Mode = player;                                                                                                    //
                        * Слот, замораживающий окно send_messege и открывающий окно choose_button                                           //
                        */                                                                                                                  //
                                                                                                                                            //
    void if_close_wnd(); /* Mode = player;                                                                                                  //
                          * Слот срабатывает при закрытии окна choose_button                                                                //
                          * Активирует окно send_messege */                                                                                 //
                                                                                                                                            //
    void show_make_wnd();/* Mode = player;                                                                                                  //
                          * Слот, замораживающий окно send_messege и открывающий окно make_image_window                                     //
                          */                                                                                                                //
                                                                                                                                            //
    void show_intercept_window(); // mode = player Открытие окна intercept                                                                  //
                                                                                                                                            //
    void back__homecoming();      // mode = player Из окна перехвата обратно в домашнее                                                     //
                                                                                                                                            //
    void show_make_wnd_to_root(QImage img, int i, int j, QString str); /* mode = root                                                       //
                                                                        * Вызывает для root make_img_window.set_rune(img, i, j, str);       //
                                                                        * Данный метод ставит активным изображение,                         //
                                                                        * которое выбрал root + указывает позицию изображения               //
                                                                        */                                                                  //
                                                                                                                                            //
    void if_close_wnd_fo_root(); /* Защита от неожиданного и от санкционированного закрытия окна */

    void show_rsa_wnd();
    void back_from_rsa_wnd();//
    void use_assimetry_crypto(int n,int e,int d);
                                                                                                                                            //
/****************************************************************************************************************************************** */


/****************************************************** Служебные слоты, для работы программы ********************************************************* */
                                                                                                                                                        //
    void create_pb(int i, int j);/* Mode = player;                                                                                                      //
                                  * Реагирует на сигнал от home_window::do_it(int, int)                                                                 //
                                  * Слот, который создает в окне choose_button кнопку в соответствующей позиции i, j                                    //
                                  * Для кнопки задается connect:                                                                                        //
                                  *                             в случае, если кнопка открыта - вызывает сигнал choose_button::i_choose_img(...)        //
                                  *                             иначе вызывает QMessegeBox с информацией об ошибке                                      //
                                  * Вызывает метод choose_button::set_image(QPB_modify *pb, int i, int j),                                              //
                                  * который ставит кнопку в соответствующую позицию                                                                     //
                                  */                                                                                                                    //
                                                                                                                                                        //
                                                                                                                                                        //
    void then_opend_img(QImage img, int i, int j, QString code); /* Mode = player;                                                                                    //
                                                                  * Слот срабатывает на сигнал home_window::i_opend(QImage, int, int)                                 //
                                                                  * Слот, который вызывает метод choose_button::open_button(QImage img, int i, int j)                 //
                                                                  * Данный метод создает новую кнопку, устанавливает на неё икноку (reverse), делает кнопку активной  //
                                                                  */                                                                                                  //
                                                                                                                                                        //
    void then_choosen_img(QImage img, int i, int j, QString code); /* Mode = player                                                                                   //
                                        * Ловит сигнал choose_buttons::i_choose_img(QImage)                                                             //
                                        * Данный слот устанавливает в send_messege то изображение, которое выбрал пользователь в качестве рабочкго      //
                                        */                                                                                                              //
                                                                                                                                                        //
                                                                                                                                                        //
    void set_rune(int rune, int color);   /* Получают значение i, задают значения temp_... i%data.size()  Внимание, внимание, костыль!                                //
                                           * Это связано с тем, что все возможные руны и цвета хранятся в make_wnd                                                    //
                                           * Выковыривает от туда руну и цвет и устанавливает в temp_rune в root (необходимо для default)                             //
                                           */                                                                                                                         //
                                                                                                                                                        //
    void then_made_img(QImage img, QString code); /* mode = player                                                                                                    //
                                                   * слот, который ловит сигнал от make_img_window, после того, как пользователь создал изображения для brute_forse   //
                                                   */                                                                                                                 //
                                                                                                                                                        //
    void new_rune_created_root(QImage img ,int i, int j, QString str, QString img_code); /* mode = root                                                                   //
                                                                                          * в случае, если преподаватель хочет создать свою руну,                         //
                                                                                          * то он жмет на изображение, открывается make_img_window                        //
                                                                                          * После этого, преподаватель создает руну, жмет "ок". "ок" возвращает руну,     //
                                                                                          * строчку, которая закодированна таким способом + позицию в сетке рун           //
                                                                                          */                                                                              //
                                                                                                                                                        //
    void test_player_image(QString code              ,      // mode = player                                                                            //
                           QString p_key, int p_key_size,   // отсылает изображение (код), ключи P                                                            //
                           QString s_key, int s_key_size,   // S (для перехвата)                                                                        //
                           int i, int j,                    // позицию в сетке изображений                                                              //
                           QString algoritm);               // алгоритм шифрования                                                                      //
                                                            // Проверят соотвествует ли присланное изображение родному                                  //
                                                            // Если пользователя перехватывают, то отсылает информацию                                  //
                                                                                                                                                        //
    void send_messege_wnd_on_intercept_value(QImage img, QString code, int i, int j);
    void solo();
    void set_flag_assimetry(bool t){flag_assimetry_done = t;}
                                                                                                                          //
    /************************************************************************************************************************************************** */


/****************************************************** Cлоты для работы с сетью ***************************************************************************** */
                                                                                                                                                               //
    // Самый главный слот для работы с сетью!                                                                                                                  //
    //                                                                                                                                                         //
    //                                                                                                                                                         //
    void NET_datagramm_analysis(); /* mode = all                                                                                                               //
                                    * Данный слот анализирует датаграмму из сокета. Подробнее см. протокол + "внутренности" метода                             //
    //                              */                                                                                                                         //
    //                                                                                                                                                         //
    //                                                                                                                                                         //
                                                                                                                                                               //
                                                                                                                                                               //
                                                                                                                                                               //
    void NET_send_players_inercept_login(QString login); // mode = player отсылает руту логин того, кого хочет перехватывать                                   //
                                                                                                                                                               //
    void NET_registration_for_root(QString login, QHostAddress sender); /* mode = root                                                                         //
                                                                         * проверка уникальности имени пользователя                                            //
                                                                         * занесение в БД пользователей в случае успеха                                        //
                                                                         * генерация ответа (verdict)                                                          //
                                                                         */                                                                                    //
    void NET_registration_for_player(QString verdict); /* mode = player                                                                                        //
                                                        * пользователь анализирует ответ рута (уникальный логин или очередной плагиат)                         //
                                                        * П.С. socket отправляется при нажатии кноппки ok, SLOT on_Login_button_clicked()                      //
                                                        */                                                                                                     //
                                                                                                                                                               //
    void NET_a_new_player_come(QString new_player_login, QString sender); /* mode = root                                                                       //
                                                                           * оповещение других пользователей о том, что появился новый игрок                   //
                                                                           */                                                                                  //
                                                                                                                                                               //
    void NET_add_new_player (QString login, QHostAddress sender); /* mode = player                                                                                                  //
                                              * В случае появления нового пользователя, root шлет всем пользователям сообщение,                                //
                                              * которое содержит action = n, data = login                                                                      //
                                              * Данный метод добавляет нового пользователя                                                                     //
                                              */                                                                                                               //
                                                                                                                                                               //
    void NET_send_info_for_start(); /* mode = root                                                                                                             //
                                     * Шлет всем пользователям информацию о старте                                                                             //
                                     * Переделать чтобы для одного пользователя!!!!                                                                            //
                                     */                                                                                                                        //
                                                                                                                                                               //
    void NET_start_messeges_phase_1(QString messeges);                    /* mode = player                                                                     //
                                                                           * Первая фаза передачи информации необходимой для старта                            //
                                                                           * Анализирует все слова и составляет массив vector<vector> messeges из строки       //
                                                                           * делает resize source_img!!!                                                   !!! //
                                                                           */                                                                                  //
                                                                                                                                                               //
                                                                                                                                                               //
                                                                                                                                                               //
    void NET_players_intercept_for_root(QString data, QHostAddress sender);                                                                                    //
    void NET_players_intercept_for_player(QString data);                                                                                                       //
    void NET_send_intercepted_messege_for_player (QString addres, QString code,     // адресс, изображение                                                       //
                                                 QString p_key, int p_key_size,   // ключи P                                                                   //
                                                 QString s_key, int s_key_size,   // S (для перехвата)                                                         //
                                                 int i, int j, QString algoritm);                                                                              //
                                                                                                                                                               //
    void NET_no_overhere_for_root (QString login); /* В случае, если пользователь не хочет больше подслушивать этого человека                                  //
                                                    * шлет root информацию ою этом                                                                             //
                                                    */                                                                                                         //
                                                                                                                                                               //
    void NET_list_of_user_in_game(QString data, QHostAddress sender);                                                                                                               //
    void NET_send_info_for_player(QString address, QString &messeges, QString &codes);                                                                                         //
    void NET_add_intercepted_messege(QString data);                                                                                         //

    void NET_send_rsa_img(QString code, int e, int n, int i, int j);
    void NET_set_rsa_intercept_mess(QString data);

    void NET_send_lvl();
    void create_source_images();

    void NET_add_player (QString login, QHostAddress sender);

    QString getIpRoot();
/************************************************************************************************************************************************************* */                                                                                                                                                               //

private:
    Ui::MainWindow *ui;
    QUdpSocket *socket;
    QString root_address = "127.0.0.1";       // Меняется в зависиости от сети! Менять ручками в исходном коде
    QMap<QString, QString> user_list;         // Список пользователей. Ключ - адресс (IPv4), value -  логин
    vector<vector<QString>> code_messege;     // Строки, которые кодируются рунами
    vector<vector<QString>> runes_code;       // Коды source image
    int img_count_n;                          // Дефолтное значение
    int img_count_m;                          // Дефолтное значение
    QVector <QString> me_overhere_addres_list;// Список людей, которые меня прослушивают
    QString i_overhear_login = "";            // Логин того, кого я подслушиваю

    // root тоже имеет право на свои переменные в mainwindow
    QString messeges;                         // Строка из закодированных рунами строк (через пробел)
    QString img_code;                         // Строка с закодированными изображениями (номер руны_номерцвета)
    //

    home_window *home_wnd;
    send_messege *send_messege_wnd;
    choose_button *choose_button_wnd;
    make_img_window *make_wnd;
    root_window *root_wnd;
    interception *intercept_wnd;
    asimetry *rsa_wnd;

    bool flag_is_it_root = false;
    bool flag_assimetry_done = false;
    QIcon icon_test = QIcon(":/images/question.png");
};

int count_simbols_befor(QString data, char befor); /* Находит количество символов до определенного */

QString cut_string_befor_simbol(QString &str, char befor); /* Вырезает из строки кусок, который идет до символа befor. Символ befor удаляет */

QString simbols_in_str_at_positions(QString data, int position, int count);

#endif // MAINWINDOW_H
