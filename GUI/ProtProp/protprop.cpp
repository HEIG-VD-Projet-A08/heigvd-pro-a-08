#include "protprop.h"
#include "./ui_protprop.h"

ProtProp::ProtProp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ProtProp)
{
    ui->setupUi(this);
}

ProtProp::~ProtProp()
{
    delete ui;
}


void ProtProp::on_btn_loadDB_clicked()
{
    data_file = QFileDialog::getOpenFileName(this, tr("Select a data file"), "/home/", tr("XML Files (*.xml)"));
    ui->lbl_filename->setText(data_file);
    ui->btn_run->setEnabled(!data_file.isNull());
}
