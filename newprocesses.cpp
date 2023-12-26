#include "newprocesses.h"
#include "ui_newprocesses.h"
vector<vector<double>> tmp;
NewProcesses::NewProcesses(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewProcesses)
{
    ft = new ForTabulation();
    connect(ft, &ForTabulation::returnData, this, &NewProcesses::getData);
    this->setWindowTitle("New Processes");
    ui->setupUi(this);
}

NewProcesses::~NewProcesses()
{
    delete ui;
}


void NewProcesses::on_CreateButton_clicked()
{
    if(ui->Palindrome->isChecked()){
        tmp.push_back({0});
    }
    emit returnProcesses(tmp);
    tmp.clear();
    ClearForm();
    close();
}

void NewProcesses::getData(double a, double b, int st, int N){
    tmp.push_back({a, b, (double)st, (double)N});

}

void NewProcesses::on_CancelButton_clicked()
{
    ClearForm();
    close();
}

void NewProcesses::ClearForm(){
    ui->Tabulation->setCheckState(Qt::Unchecked);
    ui->Palindrome->setCheckState(Qt::Unchecked);
}

void NewProcesses::on_Tabulation_stateChanged(int arg1)
{
    if(arg1 !=0){
        ft->show();}
}

