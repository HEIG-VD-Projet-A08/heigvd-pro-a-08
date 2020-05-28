
#ifndef CLIENTTCP_H
#define CLIENTTCP_H
#include <QObject>
#include <QtNetwork>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QtCore>
#include "Message.h"

class ClientTcp : public QObject
{
Q_OBJECT
private:
    QObject *parent;        // pointeur sur le parent
    QTcpSocket *socket;     // pointeur sur la socket vers le serveur
    QString add;            // adresse de la socket
    int port;               // port de la socket
    Message *message;

public:
    /**
     * @brief ClientTcp constructeur de la classe
     * @param parent ClientTcp classe créant l'instance client tcp
     * @param ipAdd adresse IP
     * @param port port de connection
     */
    ClientTcp(QObject *parent, QString ipAdd, int port);

    /**
      * @brief destructeur de la classe, ferme aussi le socket
      */
    ~ClientTcp();

    /**
     * @brief sendGreetings établit une connexion avec le serveur
     * @return si 1 cela veut dire qu'une erreur s'est produite, si  0 tout est en ordre
     */
    int sendGreetings();

    /**
     * @brief sendData envoie un fichier XML pour la configuration de l'algorithme
     * @param file fichier à envoyer
     */
    void sendData(QFile &file);

    /**
     * @brief sendStop demande d'arrêt au serveur //TODO à tester
     */
    void sendStop();

    /**
     * @brief sendStopRecovery demande l'arrêt au serveur en gardant de quoi poursuivre par la suite //TODO à tester
     */
    void sendStopRecovery();

    /**
     * @brief TerminConnexion termine la connexion avec le serveur et détruit la socket
     */
    void TerminConnexion();

signals:
    /**
     * @brief readResultXML reçoit les résultats du serveur sous format XML
     */
    void readResultXML();

public slots:
    /**
     * @brief readyRead signal lors de la réception d'un message du serveur
     */
    void readyRead();
};

#endif // CLIENTTCP_H
