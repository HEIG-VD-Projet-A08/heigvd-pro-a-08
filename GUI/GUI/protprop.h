#ifndef PROTPROP_H
#define PROTPROP_H

#include <QMainWindow>
#include "clienttcp.h"

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
    void on_btn_run_clicked();
    void on_btn_stop_clicked();
    void on_btn_save_actual_clicked();
    void on_btn_save_res_clicked();


    void showPointToolTip(QMouseEvent *event);
    void ReadXMLFile(QString &it, QString &score, QString &newValue, QVector<QString> &word);
    void getValuesFromServer(double &x, double &y1, double &y2, QVector<QString> &word);
    void updateGraphe();

private:
    Ui::ProtProp *ui;
    QString nbWords;
    QString nbCharsMax;
    QString nbCharsMin;
    QString nbIter;
    QString ip;
    QString port;
    ClientTcp *socket;
    QVector<QVector<QString>> words;
    QVector<double> contX, contY1, contY2;
    bool isStopRequested;
    bool stopConnection = false;
    Message *message;

    void quitAndResetRessources();
};
#endif // PROTPROP_H
