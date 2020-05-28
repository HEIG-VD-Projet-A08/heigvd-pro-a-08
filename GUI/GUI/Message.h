#ifndef MESSAGE_H
#define MESSAGE_H
#include <QObject>
#include <QMessageBox>

class Message: public QObject
{
    Q_OBJECT
public:
    Message(QWidget *parent);

    void Error_0(){
        QMessageBox::warning(0, QString("Erreur #0"), QString("Les paramètres ont mal été saisi. Le programme n'a pas été exécuté."));
    }

    void Error_1(){
        QMessageBox::warning(0, QString("Erreur #1"), QString("Les paramètres ne sont pas valides. Le programme n'a pas été exécuté."));
    }

    void Error_2(){
        QMessageBox::warning(0, QString("Erreur #2"), QString("Problème lors de la génération du fichier de configuration."));
    }

    void Error_3(){
        QMessageBox::warning(0, QString("Erreur #3"), QString("Le serveur Tcp a déjà été instancié."));
    }

    void Error_4(){
        QMessageBox::warning(0, QString("Erreur #4"), QString("La connexion n'a pas pu être effectuée."));
    }

    void Error_5(){
        QMessageBox::warning(0, QString("Erreur #5"), QString("Le client Tcp n'a pas été instancié."));
    }

    void Error_6(QString erreurType){
        QMessageBox::warning(0, QString("Erreur #6"), QString("Le Parsing XML de temp.xml a échoué avec cette erreur: " + erreurType));
    }

    void indication_0(){
        QMessageBox::information(0, QString("Indication #0"), QString("Le programme va être exécuté."));
    }

    void indication_1(){
        QMessageBox::information(0, QString("Indication #1"), QString("Le nombre d'itération a été atteint."));
    }
};


#endif // MESSAGE_H
