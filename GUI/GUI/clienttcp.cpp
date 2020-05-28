#include "clienttcp.h"

/**
 * @brief ClientTcp::ClientTcp constructeur de la classe ClientTcp
 * @param parent classe qui va créer l'instance
 * @param ipAdd adresse IP sur laquelle se connecter
 * @param port port IP sur laquelle se connecter
 */
ClientTcp::ClientTcp(QObject *parent, QString ipAdd, int port) : parent(parent), add(ipAdd), port(port), message(){
    // partie client TCP
    socket = new QTcpSocket( parent );

    connect(socket, &QTcpSocket::readyRead, this, &ClientTcp::readyRead);
    connect(socket, &QTcpSocket::readyRead, this, &ClientTcp::readResultXML);

    socket->connectToHost(ipAdd, port);
}

/**
 * @brief ClientTcp::~ClientTcp destructeur de la class ClientTcp
 */
ClientTcp::~ClientTcp(){
}

/**
 * @brief ClientTcp::sendGreetings message pour initier la conversation avec le serveur
 * @return 1 si il y a eut un problème lors de l'envoi des salutations aux serveur sinon 0
 */
int ClientTcp::sendGreetings(){
    if(!socket->waitForConnected()){
        message->Error_4();
        return 1;
    }

    socket->write( "Hello Server\n" );
    return 0;
}

/**
 * @brief ClientTcp::sendStop indique au serveur que la GUI va arrêter de recevoir les données et qu'il
 *                              faut arrêter les calculs
 */
void ClientTcp::sendStop(){
    // if disconnect
    if(!socket->waitForConnected()){
        message->Error_4();
        return;
    }

    socket->write( "STOP\n" );
}

/**
 * @brief ClientTcp::sendStopRecovery demande au serveur de s'arrêter les calculs en envoyant un message "STOP -R\n"
 */
void ClientTcp::sendStopRecovery(){
    // if disconnect
    if(!socket->waitForConnected()){
        message->Error_4();
        return;
    }

    socket->write( "STOP -R\n" );
}

/**
 * @brief ClientTcp::TerminConnexion termine la connexion entre le serveur et le client en envoyant "BYE\n"
 */
void ClientTcp::TerminConnexion(){
    // if disconnect
    if(!socket->waitForConnected()){
        message->Error_4();
        return;
    }

    socket->write( "BYE\n" );
    socket->disconnectFromHost();
    socket->close();
}

/**
 * @brief ClientTcp::sendData permet d'envoyer le fichier de configuration pour le serveur en envoyant "START\n"
 * @param file fichier xml à envoyer
 */
void ClientTcp::sendData(QFile &file){
    file.open(QIODevice::ReadOnly);
    QByteArray mydata=file.readAll();

    socket->write( "START\n" + mydata + "\n");
    file.close();
    socket->disconnectFromHost();
    while(!socket->waitForBytesWritten());
    socket->connectToHost(add, port);
}

/**
 * @brief ClientTcp::readyRead socket permettant de recevoir le fichier de résultat en tout temps
 */
void ClientTcp::readyRead()
{
    // read the data from the socket
    QByteArray temp = socket->readAll();

    QDir dir;
    QFile file(dir.currentPath() + "/tmp.xml");

    // partie client TCP
    file.open(QIODevice::WriteOnly);
    file.write(temp);
    file.close();
}
