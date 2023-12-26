#ifndef FORTABULATION_H
#define FORTABULATION_H

#include <QWidget>

namespace Ui {
class ForTabulation;
}

class ForTabulation : public QWidget
{
    Q_OBJECT

public:
    explicit ForTabulation(QWidget *parent = nullptr);
    ~ForTabulation();
signals:
    void returnData(double a, double b, int st, int N);
private slots:
    void on_pushButton_clicked();

private:
    Ui::ForTabulation *ui;
};

#endif // FORTABULATION_H
