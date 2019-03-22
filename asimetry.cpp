#include "asimetry.h"
#include "ui_asimetry.h"

using namespace std;

asimetry::asimetry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::asimetry)
{
    ui->setupUi(this);
    this->setWindowTitle("Ключи RSA");
    ui->progress_bar_exchange->setVisible(false);
}

asimetry::~asimetry()
{
    delete ui;
}

void asimetry::on_button_generate_clicked()
{

    srand(time(0));
    int p = prostoe_chislo(100);
    int q = prostoe_chislo(100);

    n = p*q;
    long long int phi = (p - 1)*(q - 1);
    e = (int)take_e(phi);
    d = (int)take_d(e, phi);

    QString s = QString::number(p);
    ui->lbl_p_value->setText(s);
    s = QString::number(q);
    ui->lbl_q_value->setText(s);
    s = QString::number(n);
    ui->lbl_n_value->setText(s);
    s = QString::number(phi);
    ui->lbl_f_n_value->setText(s);
    s = QString::number(e);
    ui->lbl_e_value->setText(s);
    s = QString::number(d);
    ui->lbl_d_value->setText(s);

    flag_key_generated = true;
}

void asimetry::on_button_exchange_clicked()
{
    if (!flag_key_generated)
    {
        QMessageBox::information(this, "error", "Ключи не сгенерированы!");
        return;
    }

    ui->progress_bar_exchange->setVisible(true);

    for (int i = 0; i < 100; i++)
    {
        QEventLoop loop;
        QTimer::singleShot(mc, &loop, SLOT(quit()));
        loop.exec();
        loop.exit();
        ui->progress_bar_exchange->setValue(i+1);
    }

     ui->label_4->setText("2713");

    ui->progress_bar_exchange->setVisible(false);
    ui->lbl_status_value->setText("Обмен завершен");

    emit this->keys_was_generated(n, e, d);
}

void asimetry::on_button_back_clicked()
{
    emit this->from_rsa_to_home_wnd();
}
