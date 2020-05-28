#include "protprop.h"
#include "ui_mainwindow.h"

protprop::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

protprop::~MainWindow()
{
    delete ui;
}

