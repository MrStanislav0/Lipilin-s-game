#include <vector>
#include <string>
#include <map>

#include<QRgb>
#include <QColor>

#include "fun.h"

using namespace std;

vector<int> pblok_key(int pblok_lenght, int start)
{
    /*
     * Принимает на вход: int длина П-блока, стартовое значение ГСЧ
     *
     * Функция, которая генерирует ключ перестановки
     * Ключ возвращается ввиде массива (vector), где
     * каждому i-ому символу соответствует его новое место
    */
    srand(start);

    vector<int> key;
    for (int i = 0; i < pblok_lenght; i++)
        key.push_back(i);

    int temp;
    for (int i = 0; i < pblok_lenght; i++)
    {
        temp = rand() % pblok_lenght;
        swap(key[i], key[temp]);
    }

    return key;
}

vector <vector<int>> sblok_like_vigener_key(int count, int start)
{
    /*
     * Принимает на вход: int длина S-блока, стартовое значение ГСЧ
     *
     * Генерирует случайные числа по модулю 256, которые будут являться элементами ключа
     * key[i] = x, где x -> [0..255]
     * Принимает размер ключа, возвращает массив с случайными числами
    */
    srand(start);

    vector <vector<int>> key;
    for (int i = 0; i < 3; i++)
    {
        int temp;
        vector<int> temp_key;
        for (int j = 0; j < count; j++)
        {
            temp = rand() % 256;
            temp_key.push_back(temp);
        }
          key.push_back(temp_key);
    }

    return key;
}

vector<QRgb> pblok_use(vector<QRgb> &original, vector<int> &key)
{
    /*
    Применяет pblok к массиву с пикселями. Возвращает уже перемешанный
    */
    vector<QRgb> p_pixel;
    QRgb temp;
    int key_size = key.size();
    for (int i = 0; i < key_size; i++)
    {
        temp = original[key[i]];
        p_pixel.push_back(temp);
    }
    return p_pixel;
}

vector<QRgb> sblok_like_vigener_use(vector<QRgb> &data, vector<vector<int>> &key)
{
    /*
     * Принимает массив QRgb пикселей и vector< vector <key>> Ключ
     * Шифр Виженера. Каждый цвет пикселя складывается с соответствующим ключом в массиве ключей key
     * Возвращает массив QRgb
     *
     *
    */

    int data_size = data.size();

    for (int i = 0; i < data_size; i++)
    {
        // Способ работы с пикселем - QColor
        // Каждую составляющую складывает с ключом
        QColor temp = QColor(data[i]);
        int red = (temp.red() + key[0][i%key[0].size()]) % 256; // Первый ключ соответствует red
        temp.setRed(red);
        int green = (temp.green() + key[1][i%key[1].size()]) % 256; // Второй green
        temp.setGreen(green);
        int blue = (temp.blue() + key[2][i%key[2].size()]) % 256; // Третий blue
        temp.setBlue(blue);
        data[i] = temp.rgb();
    }

    return data;
}

vector<int> pblok_key_revers(vector<int> &pblok_key)
{
    /*
    Получает на вход p-блок, выводит обратный к нему пблок
    Для каждого возможного элемента в p-блок ищет "обратный к нему"
    Распиши пример на бумажке
    */
    vector<int> revers;
    int i = 0;

    int pblok_key_size = pblok_key.size();

    while (i != pblok_key_size)
    {
        for (int j = 0; j < pblok_key_size; j++)
            if (pblok_key[j] == i)
            {
                revers.push_back(j);
                break;
            }
        i++;
    }
    return revers;
}

vector<QRgb> sblok_like_vigener_reverse(vector<QRgb> &data, vector<vector<int>> &key)
{
    /*
     * Принимает массив QRgb пикселей и vector< vector <key>> Ключ
     * Возвращает массив QRgb
     * a + b (mod n) = c
     * c - b (mod n) = a
     * В данном случае ищем a, т.к. на входе подается уже зашифрованное изображение
     * Поскольку у нас не конечное поле, то могут быть отрицательные значения, преобразуем к
     * c - b + n (mod n) = a
    */

    int data_size = data.size();

    for (int i = 0; i < data_size; i++)
    {
        // Способ работы с пикселем - QColor
        // Каждую составляющую складывает с ключом
        QColor temp = QColor(data[i]);
        int red = (temp.red() - key[0][i%key[0].size()] + 256) % 256; // Первый ключ соответствует red
        temp.setRed(red);
        int green = (temp.green() - key[1][i%key[1].size()] + 256) % 256; // Второй green
        temp.setGreen(green);
        int blue = (temp.blue() - key[2][i%key[2].size()] + 256) % 256; // Третий blue
        temp.setBlue(blue);
        data[i] = temp.rgb();
    }

    return data;
}

long long int prostoe_chislo(long long int max)
{
    long long int a = 0;

    while (1)
    {
        a = rand() % max;

        if (prostoe(a))
        {
            break;
        }
    }
    return a;
}

long long int take_d(long long int e, long long int phi)
{
    long long int d_simple = 3;
    int d = 0;
    while (d_simple !=1)
    {
        d = rand();
        d_simple = (e*d) % phi;
    }

    return d;
}

long long int take_e(long long int phi)
{
    int e = 0;
    int e_simple = 0;
    while (e_simple !=1)
    {
        e = rand()%phi;
        e_simple = gcd (e, phi);
    }

    return e;
}

int gcd(int a,int b)
{
    if (b==0) return a;
    else return gcd(b,a%b);
}

bool prostoe(long long int n)
{
    if (n < 3)
        return false;
    for (int i = 2; i <= sqrt(n); i++)
    {
        if (n % i == 0)
            return false;
    }
    return true;
}

vector <int> crypt(vector<int> s1, int e, int n)
{
    vector <int> result;
    for (int i = 0; i < (int)s1.size(); i++)
    {
        int c = 1;
        int j = 0;
       int ASCIIcode = s1[i];
        while (j < e)
        {
            c = c*ASCIIcode;
            c = c%n;
            j++;
        }
        result.push_back(c);
        //cout << c << " ";
    }
    //cout << endl;
    return result;
}

vector<int> decrypt(vector <int> crypted, int d, int n)
{
    vector<int> s1;
    for (int i = 0; i < (int)crypted.size(); i++)
    {
        int m = 1;
        int j = 0;
        while (j < d)
        {
            m = m * crypted[i];
            m = m % n;
            j++;
        }
        //unsigned int temp = m;
        s1.push_back(m);
    }
    //cout << "TEST: " << s1;
    return s1;
}

vector<int> image_to_vector(QImage img)
{
    vector<int> data;
    QColor pxl;

    for (int i = 0; i < img.width(); i++)
        for (int j = 0; j < img.height(); j++)
        {
            pxl = QColor(img.pixel(i, j));
            data.push_back(pxl.red());
            data.push_back(pxl.green());
            data.push_back(pxl.blue());
        }

    return data;
}

QImage set_vector_at_image(QImage img, vector<int> data)
{
    int pointer = 0;        // Указатель на элемент вектора

    for (int i = 0; i < img.width(); i++)
        for (int j = 0; j < img.height(); j++)
        {
            QColor pxl = QColor(img.pixel(i, j));
            pxl.setRed(data[pointer]);
            pxl.setGreen(data[pointer+1]);
            pxl.setBlue(data[pointer+2]);
            img.setPixel(i, j, pxl.rgb());
            pointer = pointer + 3;
        }

    return img;
}

vector<int> crypt_rsa2(vector<int> data, int e, int n)
{
    srand(e*n);
    for (int i = 0; i < (int)data.size(); i++)
    {
        data[i] = data[i]^(rand() % 256);
    }

    return data;
}

vector<int> decrypt_rsa2(vector<int> data, int e, int n)
{
    srand(e*n);
    for (int i = 0; i < (int)data.size(); i++)
    {
        data[i] = data[i]^(rand() %256);
    }

    return data;
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
            if (i >= hei*wid)
            {
                break;
            }
            QRgb t;
            t=encrypted_image.pixel(i%wid, i/wid);
            temp.push_back(t);
            i++;
        }

        // Шифрование и запись зашифрованных пикселей
        int temp_size = temp.size();
        if (temp_size==p_size)
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
    // Существует out of range of image Хз как фиксить
    int wid = encrypted_image.width();
    int hei = encrypted_image.height();
    int blok_size = sb_key[0].size() * 3;

    int cikl = hei*wid;

    // Считывание
    for (int i = 0; i < cikl;)
    {
        vector<QRgb> temp;
        for (int j=0; j<blok_size; j++)
        {
            if (i >= cikl)
            {
                break;
            }

            QRgb t;

            t=encrypted_image.pixel(i%wid, i/wid);
            temp.push_back(t);
            i++;
        }

        // Шифрование и запись зашифрованных пикселей
        int temp_size = temp.size();
        if (temp_size==blok_size)
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
    int blok_size = sb_key[0].size()*3;

    // Считывание
    int cikl = hei*wid;
    for (int i = 0; i < cikl;)
    {
        vector<QRgb> temp;
        for (int j=0; j<blok_size; j++)
        {
            if (i >= cikl)
            {
                break;
            }

            QRgb t;
            t=encrypted_image.pixel(i%wid, i/wid);
            temp.push_back(t);
            i++;
        }

        // Шифроание и запись зашифрованных пикселей
        int temp_size = temp.size();
        if (temp_size==blok_size)
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
