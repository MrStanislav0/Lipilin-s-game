#ifndef ROOT_WINDOW_H
#define ROOT_WINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <QString>
#include <QPushButton>
#include <QLayout>
#include <QRgb>
#include <QMap>
#include <QUdpSocket>

#include <vector>
#include <ctime>

#include "qpb_modify.h"
#include "fun.h"

namespace Ui {
class root_window;
}

class root_window : public QDialog
{
    Q_OBJECT

public:
    explicit root_window(QWidget *parent = 0);
    ~root_window();

    void create_images();                               // Создает кнопки в окне

    void set_temp_rune(QImage img) {temp_rune = img;}   // Костыль. Ставит руну созданную в make_rune
    void set_rune_at_GL(QPB_modify *pb, int i, int j);  // Установка руны в сетке рун, после её создания в mainwindow

    QString get_messege_at_position(int i, int j);      // Получение сообщения, соответствующего позиции в mainwindow
    QString get_rune_code_at_position(int i, int j);    // Передает руну на определенной позиции в mainwindow

    int get_n() {return n;}                             // Передает n в mainwindow
    int get_m() {return m;}                             // Передает m в mainwindow

    void add_new_player(QString login);                 // Добавляет нового пользователя в список на ui
    bool get_flag_game_on(){return flag_gamego_on;}     // Флаг, указывающий, игра началась или нет
    void delete_player(QString login);
    int level = 1;

private slots:
    void on_button_apply_clicked();                     // Изменение n, m рутом (пересчет сетки)

    void on_button_default_clicked();                   // Возврат к дефолтным настройкам

    void on_button_start_clicked();                     // Старт игры

    void on_edit_text_editingFinished();
    void Lvl_up_all();
    void Lvl_down_all();


signals:
    void get_rune(int, int);                                    // Сигнал "дай мне руну" (шлется в make_img), там есть vector с рунами
    void show_make_img_with_my_img(QImage, int, int, QString);  // Смена окон
    void start();                                               // Старт игры
    void lvl_up();

private:
    Ui::root_window *ui;

    /* Параметры по-умолчанию */
    int n = 6;
    int m = 7;
    std::vector<QString> default_text = {
                        "Д", "Е", "Н", "Ь", "И",
                        "К", "С", "В", "В", "О",
                        "С", "Е", "М", "Ь", "Д",
                        "В", "А", "Д", "Ц", "А",
                        "Т", "Ь", "У", "Ф", "О",
                        "Н", "Т", "А", "Н", "А",
                        "Г", "Е", "Р", "О", "Й",
                        "К", "А", "Р", "Т", "И",
                        "Н", "А"};

    bool flag_gamego_on = false;

    QMap<QString, QString> user_list_root;

    std::vector<QString> messege;                   // Сообщение, которое закодировано рунами (изначально равно default_text)
    std::vector<QImage> runes;                      // Здесь зранятся "Индексы" рун (строка из "i_j" i - номер исходной руны
                                                    //                                                 j - номер цвета
    QImage temp_rune;                               // Костыль
    std::vector<std::vector<QPB_modify*>> pb_runes; // Массив с указателями на руны (сразу надо было делать везде)
    int colors_size = 7;                            // Количество цветов в игре
    int runes_size = 18;                            // Количество рун в игре
    std::vector<int> p_colors;                      // Ключ перестановки цветов

    QMap<QString, int> letters;                     // Инициализация при создании rootwnd. Соответствие между char и руной
                                                    // QString потому что я заебался

    int max_level = 9;

    void mix_message();
    void pad_message();

};

#endif // ROOT_WINDOW_H
