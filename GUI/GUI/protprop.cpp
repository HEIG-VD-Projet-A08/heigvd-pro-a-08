
#include "protprop.h"
#include "./ui_protprop.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <QXmlStreamReader>
#include <QRegExpValidator>
#include "Message.h"

/**
 * @brief ProtProp::ProtProp définit les regex pour les paramètres,
 * @param parent parent de l'objet
 */
ProtProp::ProtProp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ProtProp), socket(nullptr), isStopRequested(false), message()
{
    // set validators for input
    QRegExpValidator *inputNumberChar   = new QRegExpValidator(  QRegExp("(?:[0-9]){2}"));
    QRegExpValidator *inputNumberPort   = new QRegExpValidator(  QRegExp("(?:[0-9]){5}"));
    QRegExpValidator *inputIteration    = new QRegExpValidator(  QRegExp("(?:[0-9]){4}"));
    QRegExpValidator *inputWordMax      = new QRegExpValidator(  QRegExp("(?:[0-9]){3}"));
    QRegExpValidator *ipValidator       = new QRegExpValidator(  QRegExp("(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)"));

    ui->setupUi(this);

    // enable validator for each input
    ui->IP->setText("192.168.1.206");
    ui->IP->setValidator(ipValidator);
    ui->CharMax->setValidator(inputNumberChar);
    ui->CharMin->setValidator(inputNumberChar);
    ui->CharMax->setText("30");
    ui->CharMin->setText("20");
    ui->iterations->setValidator(inputIteration);
    ui->iterations->setText("100");
    ui->nbWords->setValidator(inputWordMax);
    ui->nbWords->setText("10");
    ui->port->setValidator(inputNumberPort);
    ui->port->setText("9001");
}

/**
 * @brief ProtProp::~ProtProp destructeur de la classe
 */
ProtProp::~ProtProp()
{
    delete ui;
}

/**
 * @brief ProtProp::on_btn_run_clicked gestion du chargement des paramètres lorsqu'on presse sur run,
 *  envoit les informations au serveur et attend la réception de résultats pour commencer à dessiner le graphe
 */
void ProtProp::on_btn_run_clicked()
{
    stopConnection = false;

    nbCharsMax  = ui->CharMax->text();
    nbCharsMin  = ui->CharMin->text();
    nbWords     = ui->nbWords->text();
    nbIter      = ui->iterations->text();
    ip          = ui->IP->text();
    port        = ui->port->text();

    // test que les arguments soient tous rempli
    if (nbCharsMax == "" ||
            nbWords == "" ||
            nbIter == "" ||
            nbCharsMin == "" ||
            ip == "" ||
            port == "" ){
        message->Error_0();
        return;
    }

    // contrôle des bornes des paramètres entrés
    if(nbIter.toInt() < 1 || nbIter.toInt() > 1000 ||
            nbWords.toInt() < 1 || nbWords.toInt() > 100 ||
            port.toInt() < 1024 || port.toInt() > 65535 ||
            nbCharsMax.toInt() > 49 || nbCharsMin.toInt() < 10 ||
            nbCharsMin.toInt() > nbCharsMax.toInt() ){
        message->Error_1();
        return;
    }

    // écriture du fichier XML dans le dossier de l'application
    QDir dir;
    QFile file(dir.currentPath() + "/option.xml");

    // écriture du fichier de paramètres
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("Options");
    xmlWriter.writeTextElement("Nb_words", nbWords);
    xmlWriter.writeTextElement("Nb_char_Max", nbCharsMax);
    xmlWriter.writeTextElement("Nb_char_Min", nbCharsMin);
    xmlWriter.writeTextElement("Nb_iter", nbIter);
    xmlWriter.writeEndElement();

    // test si une erreur est survenue pendant l'écriture du fichier xml
    if(xmlWriter.hasError())
        message->Error_2();

    file.close();

    // test si le serveur est déjà instancié
    if (socket != nullptr){
        message->Error_3();
        return;
    }

    socket = new ClientTcp(this, ip, port.toInt());
    connect(socket, &ClientTcp::readResultXML, this, &ProtProp::updateGraphe);

    if(!socket->sendGreetings())
        socket->sendData(file);
    else{
        socket = nullptr;
        return;
    }
    message->indication_0();


	
	//Après avoir envoyé les paramètres au serveur, on commence à construire note zone de graphique, on va ensuite atteindre les résultats du serveur pour commencer à dessiner nos graphes
    int sizeX = nbIter.toInt(); //taille de l'axe des X, représente le nombre d'itérations, sera adapté au paramètre correspondant rentré
    int sizeY = 100; //représente le résultat obtenu en pourcentage pour chaque graphe, est donc de taille fixe (100)

    contX.clear();
    contY1.clear();
    contY2.clear();

    ui->widget->clearGraphs();
    ui->widget->replot();
    ui->widget->setInteraction(QCP::iRangeDrag, true);
    ui->widget->setInteraction(QCP::iRangeZoom, true);
    ui->widget->addGraph();
    ui->widget->addGraph();
    //donne un label aux axes:
    ui->widget->xAxis->setLabel("iterations");
    ui->widget->yAxis->setLabel("success rate");
    //fixe la portée des axes
    ui->widget->xAxis->setRange(0, sizeX);
    ui->widget->yAxis->setRange(0, sizeY);
    ui->widget->replot();

    connect(ui->widget, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showPointToolTip(QMouseEvent*)));
}

/**
 * @brief ProtProp::showPointToolTip Permet d'afficher les coordonnées du point sur le graphique sur lequel on passe note souris
 * @param event Evénement qui va permettre de récupérer les coordonnées de notre curseur
 */
void ProtProp::showPointToolTip(QMouseEvent *event)
{
    int x = ui->widget->xAxis->pixelToCoord(event->pos().x());
    double y = ui->widget->yAxis->pixelToCoord(event->pos().y());

    y = roundf(y * 10000)/10000;

    setToolTip(QString("%1 , %2").arg(x).arg(y));
}

/**
 * @brief ProtProp::updateGraphe mets à jour le graphique à partir des valeurs récupérées dans le fichier XML du serveur.
 */
void ProtProp::updateGraphe()
{
    double x;
    double y1;
    double y2;
    QVector<QString> word;
    getValuesFromServer(x, y1, y2, word);
    contX.push_back(x);
    contY1.push_back(y1);
    contY2.push_back(y2);
    words.push_back(word);

   QCPGraph* dwPoints1 = new QCPGraph(ui->widget->xAxis, ui->widget->yAxis);
      dwPoints1->setAdaptiveSampling(false);
      dwPoints1->setLineStyle(QCPGraph::lsNone);
      dwPoints1->setScatterStyle(QCPScatterStyle::ssCircle);
      dwPoints1->setPen(QPen(QBrush(Qt::blue), 2));
      dwPoints1->addData(contX, contY1);

  QCPGraph* dwPoints2 = new QCPGraph(ui->widget->xAxis, ui->widget->yAxis);
     dwPoints2->setAdaptiveSampling(false);
     dwPoints2->setLineStyle(QCPGraph::lsNone);
     dwPoints2->setScatterStyle(QCPScatterStyle::ssTriangle);
     dwPoints2->setPen(QPen(QBrush(Qt::red), 2));
     dwPoints2->addData(contX, contY2);

    ui->widget->graph(0)->setData(contX, contY1);
    ui->widget->graph(0)->setPen(QPen(QBrush(Qt::blue), 2));
    ui->widget->graph(1)->setData(contX, contY2);
    ui->widget->graph(1)->setPen(QPen(QBrush(Qt::red), 2));
    ui->widget->replot();
}

/**
 * @brief ProtProp::on_btn_stop_clicked envoie un message au serveur pour arrêter les calculs et ferme la connexion
 */
void ProtProp::on_btn_stop_clicked()
{
    if (socket == nullptr){
        message->Error_5();
        return;
    }
    socket->sendStop();
    quitAndResetRessources();
    stopConnection = true;
}

/**
 * @brief ProtProp::on_btn_save_actual_clicked envoie un message au serveur pour lui dire d'envoyer encore le dernier résultat et
 *                                              après de fermer la connexion
 */
void ProtProp::on_btn_save_actual_clicked()
{
    if (socket == nullptr){
        message->Error_5();
        return;
    }
    socket->sendStopRecovery();
    isStopRequested = true;
    stopConnection = true;
}

/**
 * @brief ProtProp::on_btn_save_res_clicked, Sauvegarde les résultats obtenu dans un fichier CSV.
 */
void ProtProp::on_btn_save_res_clicked()
{
    if (socket == nullptr && !stopConnection){
        message->Error_5();
        return;
    }


	//sauvegarde les résultats obtenus dans un fichier CSV
	//on sauvegarde les valeurs x (results) et y (test et predict) pour chaque itération ainsi que le batch de mots utilisés pour calculer ceux-ci
    std::ofstream myFile;
	


    QString filePath = QFileDialog::getSaveFileName(this, tr("Sauvegarder les résultats"), "", tr("results (*.csv);;All Files (*)"));

	filePath += ".csv";


    myFile.open(filePath.toStdString());

	
    std::ostringstream textToWriteOSS;

    textToWriteOSS << "iteration, test, predict";
    for(int i = 1; i <= nbWords.toInt(); i++){
        textToWriteOSS << ", mot" << i;
    }

    textToWriteOSS << ";\n";
    for(int i = 0; i < contX.size(); i++){
        QVector<QString> word = words[i];

        textToWriteOSS << "" << contX[i] << ", " << contY1[i] << ", " << contY2[i];

        for(int j = 0; j < word.size(); j++){
            textToWriteOSS << ", " << word[j].toStdString();
        }
        textToWriteOSS << ";\n";
    }
    std::string textToWrite = textToWriteOSS.str();

    myFile << textToWrite;
    myFile.close();
}

/**
 * @brief ProtProp::getValuesFromServer, Récupère les informations du fichier XML et les copies. 
 * On supprime également le fichier XML afin d'atteindre correctement le prochain
 * @param x référence sur numéro de l'itération recue
 * @param y1 référence la coordonnée Y du graphe test
 * @param y2 référence la coordonnée Y du graphe predict
 * @param word référence sur les le batch de mot résultant pour cette itération
 */
void ProtProp::getValuesFromServer(double &x, double &y1, double &y2, QVector<QString> &word)
{
    QString it = "0";
    QString test;
    QString predict;
    ReadXMLFile(it, test, predict, word);
    QFile file("temp.xml");
    file.remove();

    x = it.toDouble();
    y1 = test.toDouble();
    y2 = predict.toDouble();
}

/**
 * @brief ProtProp::ReadXMLFile, Parse le fichier XML afin de récupérer le numéro d'itération ainsi que le score de test et predict
 * @param it référence sur numéro de l'itération recue
 * @param test référence la coordonnée Y du graphe test
 * @param predict référence la coordonnée Y du graphe predict
 * @param word référence sur les le batch de mots résultant pour cette itération
 */
void ProtProp::ReadXMLFile(QString &it, QString &test, QString &predict, QVector<QString> &word)
{
    int memIt = it.toInt();
    QXmlStreamReader Rxml;

    QDir dir;
    QFile file(dir.currentPath() + "/tmp.xml");

    while(!file.open(QFile::ReadOnly | QFile::Text));

    Rxml.setDevice(&file);
    Rxml.readNext();

    while(!Rxml.atEnd()){
        if(Rxml.readNext() != QXmlStreamReader::EndDocument){
            if(Rxml.isStartElement()){
                if(Rxml.name() == "result"){
                    Rxml.readNext();
                }
                while(!Rxml.atEnd()){
                     if(Rxml.isEndElement()){
                         Rxml.readNext();
                         break;
                     }else if(Rxml.isCharacters()){
                         Rxml.readNext();
                     }else if(Rxml.isStartElement()){
                         if(Rxml.name() == "it"){
                            it = Rxml.readElementText();    //récupère la valeur xml
                         }else if(Rxml.name() == "test"){
                            test = Rxml.readElementText();    //récupère la valeur xml
                         }else if(Rxml.name() == "predict"){
                             predict = Rxml.readElementText(); //récupère la valeur xml
                         }else if(Rxml.name() == "word"){
                             word.push_back(Rxml.readElementText());
                         }
                         Rxml.readNext();
                     }else{
                        Rxml.readNext();
                     }
                }
            }
        }
    }

    file.close();


    if (Rxml.hasError()){
       std::cerr << "Error: Failed to parse file "
                 << qPrintable("temp.xml") << ": "
                 << qPrintable(Rxml.errorString()) << std::endl;
    }else if (file.error() != QFile::NoError){
        std::cerr << "Error: Cannot read file " << qPrintable("temp.xml")
                  << ": " << qPrintable(file.errorString())
                  << std::endl;
    }


    // si un arrêt est demandé, on traite les dernière données et on ferme le client tcp
    if(isStopRequested && memIt != it.toInt()){
        isStopRequested = false;
        quitAndResetRessources();
    }

    // si on a effectué le nombre d'itération voulue on termine la connexion
    if(it.toInt() == nbIter.toInt()) {
        quitAndResetRessources();
    }
}

/**
 * @brief resetRessources permet de réinitialiser l'environnement pour un nouveau calcul
 */
void ProtProp::quitAndResetRessources(){
    socket->TerminConnexion();
    socket = nullptr;
}
