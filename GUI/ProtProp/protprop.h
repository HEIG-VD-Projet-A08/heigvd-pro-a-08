#ifndef PROTPROP_H
#define PROTPROP_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class ProtProp; }
QT_END_NAMESPACE

class ProtProp : public QMainWindow
{
    Q_OBJECT

public:
    ProtProp(QWidget *parent = nullptr);
    ~ProtProp();

private:
    Ui::ProtProp *ui;
};
#endif // PROTPROP_H
