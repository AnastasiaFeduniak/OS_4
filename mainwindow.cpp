#include "mainwindow.h"
#include <sys/resource.h>
#include "ui_mainwindow.h"
vector<Process> activeProcesses;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    np = new NewProcesses();
    connect(ui->CreateNewProcess, SIGNAL(clicked()), this, SLOT(NewProcessesOpen()));
    connect(np, &NewProcesses::returnProcesses, this, &MainWindow::getProcesses);
    af = new AffinitySet();
    connect(af, &AffinitySet::getAff, this, &MainWindow::putAff);
    DataNewerClock(FuncName);
    ui->InfoTable->setContextMenuPolicy(Qt::CustomContextMenu);
    contextMenu = new QMenu(this);
    killAct = new QAction("Kill", this);
    suspendAct = new QAction("Suspend", this);
    startAct = new QAction("Resume", this);
    setAffAct = new QAction("Set Affinity", this);

    changePrio = new QMenu("Change priority");
    idleAct = new QAction("IDLE", this);
    belowAct = new QAction("LOW", this);
    normalAct = new QAction("NORMAL", this);
    highAct = new QAction("HIGH", this);
    realtAct = new QAction("REAL_TIME", this);

    contextMenu->addAction(killAct);
    contextMenu->addAction(suspendAct);
    contextMenu->addAction(startAct);
    contextMenu->addAction(setAffAct);
    contextMenu->addMenu(changePrio);
    changePrio->addAction(idleAct);
    changePrio->addAction(belowAct);
    changePrio->addAction(normalAct);
    changePrio->addAction(highAct);
    changePrio->addAction(realtAct);

    connect(ui->InfoTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);
    connect(setAffAct, &QAction::triggered, this, &MainWindow::affPr);
    connect(killAct, &QAction::triggered, this, &MainWindow::killPr);
    connect(suspendAct, &QAction::triggered, this, &MainWindow::suspendPr);
    connect(startAct, &QAction::triggered, this, &MainWindow::startPr);
    connect(idleAct, &QAction::triggered, this, &MainWindow::idlePr);
    connect(belowAct, &QAction::triggered, this, &MainWindow::belowPr);
    connect(normalAct, &QAction::triggered, this, &MainWindow::normPr);
    connect(highAct, &QAction::triggered, this, &MainWindow::highPr);
    connect(realtAct, &QAction::triggered, this, &MainWindow::realtPr);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getProcesses(vector<vector<double>>a){
    for(int i = 0; i < a.size(); i++){
       activeProcesses.push_back(*(new Process(a[i])));
      //  addTableInfo(i);
      //  ui->InfoTable->insertRow(ui->InfoTable->rowCount());
     //ui->InfoTable->setItem(i, 0, new QTableWidgetItem(QString::number(a[i][0])));

    }
}
void MainWindow::NewProcessesOpen(){
    np->show();

}
void MainWindow::on_CreateNewProcess_clicked()
{
    NewProcessesOpen();
}

void MainWindow::DataNewer(){
    ui->InfoTable->setRowCount(0);
    for(int i = 0; i < activeProcesses.size(); i++){
        if(activeProcesses[i].DeathCheck() == 1){
            activeProcesses.erase(activeProcesses.begin() + i);
            i--;
        } else {
            addTableInfo(i);}

    }

}
void MainWindow::addTableInfo(int i){
    ui->InfoTable->insertRow(ui->InfoTable->rowCount());
    activeProcesses[i].renewData();
    ui->InfoTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(to_string(static_cast<int>(activeProcesses[i].getID())))));
    ui->InfoTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(activeProcesses[i].getProcName())));
    ui->InfoTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(activeProcesses[i].getProcessState())));
    ui->InfoTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(activeProcesses[i].getPriority())));
    // int temp = activeProcesses[i].getTime();
    // string TimeFormat = to_string(temp/3600) + ":" + to_string((temp%3600)/60) + ":" + to_string(temp%60);
    ui->InfoTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(activeProcesses[i].beautifulShow())));


    //ui->InfoTable->setItem(i, 11, new QTableWidgetItem(QString::number(static_cast<qulonglong>(activeProcesses[i].getProcessThreadCount()))));
}

void MainWindow::DataNewerClock(function<void(void)> a){
    int duration = 1000;
    std::thread([a, duration](){
        while(true){
            a();
            auto ms = std::chrono::steady_clock::now() + std::chrono::milliseconds(duration);
            std::this_thread::sleep_until(ms);
        }
    }).detach();
}



void MainWindow::showContextMenu(const QPoint &pos)
{
   rightClickedRow = ui->InfoTable->indexAt(pos).row();
    ui->InfoTable->selectRow(rightClickedRow);// Store the right-clicked row
    contextMenu->exec(ui->InfoTable->mapToGlobal(pos));
}

void MainWindow::killPr()
{
    int tempo = findProcessByID(rightClickedRow);
    if(!activeProcesses[tempo].KillProcess()){
        activeProcesses.erase(activeProcesses.begin() + tempo);
    }
    //activeProcesses.erase(activeProcesses.begin() + tempo);

}

void MainWindow::suspendPr()
{
    activeProcesses[findProcessByID(rightClickedRow)].SuspendProcess();
}

void MainWindow::startPr()
{
    activeProcesses[findProcessByID(rightClickedRow)].ResumeProcess();
}

int MainWindow::findProcessByID(int r){
    QString te = ui->InfoTable->item(r, 0)->text();
    for(int i = 0; i < activeProcesses.size(); i++){
        if(static_cast<pid_t>(stoi(te.toStdString())) == activeProcesses[i].getPID_T()){
            return i;
        }
    }
    return -1;
}

void MainWindow::idlePr(){
    activeProcesses[findProcessByID(rightClickedRow)].setPriority(SCHED_IDLE);
}
void MainWindow::belowPr(){
    activeProcesses[findProcessByID(rightClickedRow)].setPriority(SCHED_BATCH);
}
void MainWindow::normPr(){
    activeProcesses[findProcessByID(rightClickedRow)].setPriority(SCHED_OTHER);
}
void MainWindow::highPr(){
   activeProcesses[findProcessByID(rightClickedRow)].setPriority(SCHED_FIFO);
}
void MainWindow::realtPr(){
    activeProcesses[findProcessByID(rightClickedRow)].setPriority(SCHED_RR);
}
void MainWindow::affPr(){
    af->show();
}
void MainWindow::putAff(std::string affin){
    activeProcesses[findProcessByID(rightClickedRow)].setAffinity(affin);
}
