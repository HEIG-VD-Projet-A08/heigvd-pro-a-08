#ifndef PROTPROP_H
#define PROTPROP_H

#include <QMainWindow>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class ProtProp; }
QT_END_NAMESPACE

class ProtProp : public QMainWindow
{
    Q_OBJECT

public:
    ProtProp(QWidget *parent = nullptr);
    ~ProtProp();

private slots:
    void on_btn_loadDB_clicked();

private:
    Ui::ProtProp *ui;
    QString data_file;
};
#endif // PROTPROP_H
