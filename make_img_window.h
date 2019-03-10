/* Mode = player & root;
 * Окно используется как первым, так и вторым
 * Пользователь создает новую руну, после чего из окна шлется сигнал void i_make_img(QImage)
 * сигнал ловится mainwindow, который в зависимости от пользователя интерпретирует сигнал
*/

#ifndef MAKE_IMG_WINDOW_H
#define MAKE_IMG_WINDOW_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>

#include <string>
#include <vector>

#include "qpb_modify.h"

namespace Ui {
class make_img_window;
}

class make_img_window : public QDialog
{
    Q_OBJECT

public:
    explicit make_img_window(QWidget *parent = 0);
    ~make_img_window();

    void set_colors(); /* Ставит pushbuttons с икноками всех возможных цветов (vector<QRGB> colors) */
    void set_runes();  /* Ставит pushbuttons с икноками всех возможных рун (vector<QImage> runes)   */

    QImage get_rune(int i) {return QImage(runes[i%runes.size()]);}  //
    QRgb get_color(int i) {return colors[i%colors.size()];} //
    QImage paint_picture (QImage data, QRgb color);         // Окрашивает руну из data в цвет color
    QImage paint_picture_at_code (int rune_ind, int color_ind); // Окрашивает руну из data в цвет color
    void set_rune(QImage img, int i, int j, QString str);   //
    int get_rune_bd_size(){return runes.size();}
    int get_colors_bd_size(){return colors.size();}

private slots:
    void on_button_cancel_clicked();                        // Слот для возвращенияч к окну send_messege или root_window
    void on_button_ok_clicked();

signals:
    void i_make_img(QImage, QString);                            /* Вызывается, как пользователь закончил создавать изображение,
                                                                  * передает созданное изображение в send_messege. Без координат (брут форс)
                                                                  */

    void root_make_new_img(QImage, int, int, QString, QString);  /* Вызывается, как пользователь закончил создавать изображение,
                                                                  * передает созданное изображение и его коррдинаты в root_window
                                                                  */

private:
    Ui::make_img_window *ui;

    bool RuneSelected = false;
    bool flag_img_choosen;          // Флаг - изображение выбрано или нет
    bool flag_is_it_root = false;   // Флаг, для определения того, кто ползуется окном

    // Хранилище цветов
    // Цвета, где есть 0 и 255 являются вырожденными (см способ шифровния S) или делай свой
    std::vector<QRgb> colors =
    {
        qRgb(139, 0, 0),        //красный
        qRgb(205, 102, 0),      //оранжевый
        qRgb(238, 201, 0),      //желтый
        qRgb(0,100, 0),         //зеленый
        qRgb(185,211, 238),     //голубой
        qRgb(0, 0, 128),        //синий
        qRgb(139, 0, 139),      //фиолетовый
    };

    // Хранилище рун
    std::vector<QString> runes =
    {
        ":/runes/a.png",
        ":/runes/b.png",
        ":/runes/c.png",
        ":/runes/ch.png",
        ":/runes/d.png",
        ":/runes/e.png",
        ":/runes/h.png",
        ":/runes/i.png",
        ":/runes/k.png",
        ":/runes/l.png",
        ":/runes/m.png",
        ":/runes/n.png",
        ":/runes/o.png",
        ":/runes/p.png",
        ":/runes/r.png",
        ":/runes/t.png",
        ":/runes/u.png",
        ":/runes/v.png",
    };

    int root_i; // Координата i, для рута
    int root_j; // Координата j, для рута
    int ic;
    int ir;
};

#endif // MAKE_IMG_WINDOW_H
