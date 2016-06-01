#include "calendrier.h"
#include "ui_calendrier.h"

calendrier::calendrier(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::calendrier)
{
    ui->setupUi(this);    
    monGoogle = new Google(this);

    if (monGoogle->clientID()!="") ui->ledt_clientID->setText(monGoogle->clientID());
    if (monGoogle->clientSecret()!="") ui->ledt_clientSecret->setText(monGoogle->clientSecret());
    monGoogle->setScope(CALENDAR_AUTH_SCOPE);

    connect(monGoogle,SIGNAL(connexionEtablie()),this,SLOT(onConnexionEtablie()));
    connect(monGoogle,SIGNAL(listeCalendrierChanged()),this,SLOT(onListeCalendrierChanged()));
    connect(monGoogle,SIGNAL(listeCompleteEvenementsChanged()),this,SLOT(onListeCompleteEventsChanged()));
    connect(this, SIGNAL(listeEvenementsAffichesChanged()),this, SLOT(onListeEvenementsAffichesChanged()));
    connect(monGoogle->monCalendar,SIGNAL(listeCouleursChanged()),this,SLOT(onListeCouleursChanged()));

    ui->label->setVisible(false);
    if (ui->ledt_clientID->text()!="" && ui->ledt_clientSecret->text()!="") monGoogle->connexion();

}

calendrier::~calendrier()
{
    delete ui;
}

QList<googleCalendar::evenement> calendrier::listeEvenementsAffiches()
{
    return m_listeEvenementsAffiches;
}

void calendrier::setListeEvenementsAffiches(QList<googleCalendar::evenement> liste)
{
    m_listeEvenementsAffiches=liste;
    emit listeEvenementsAffichesChanged();
}

void calendrier::onConnexionEtablie()
{
    ui->btn_connexion->setText("Déconnexion");
    monGoogle->appelAPICalendarListe();
}

void calendrier::on_btn_connexion_clicked()
{
    if (ui->ledt_clientID->text()!="" && ui->ledt_clientSecret->text()!=""&&ui->btn_connexion->text()=="Connexion") monGoogle->connexion();
    if (ui->btn_connexion->text()=="Déconnexion")
    {
         monGoogle->deconnexion();
         ui->btn_connexion->setText("Connexion");
    }
}

void calendrier::onListeCalendrierChanged()
{
    ui->label->setVisible(true);
    QMap<QString, QVariant> listeCal = monGoogle->listeCalendrier();
    foreach (QVariant valeur, listeCal)
    {
        QCheckBox *check  =new QCheckBox(this);
        check->setText(valeur.toString());
        check->setObjectName(listeCal.key(valeur.toString()));
        connect(check, SIGNAL(clicked(bool)),this, SLOT(onCheckboxCalendrierClicked()));
        ui->layoutCalendriers->addWidget(check);
        monGoogle->ajouteCheckBox(check);
    }
    if (monGoogle->listeCheckBox().count()>0)
    {
        monGoogle->remplissageListeCompleteEvents=true;
        monGoogle->monCalendar->setCalendrierIdDefaut(monGoogle->listeCalendrier().keys().first());
        monGoogle->monCalendar->remplissageListeEvenements(monGoogle->listeCalendrier().keys().first());
    }
}

void calendrier::onListeCompleteEventsChanged()
{
//    ui->textEdit->clear();
    ui->listWidget->clear();
    if (!monGoogle->listeCompleteEvenements().isEmpty())
    {
        QList<googleCalendar::evenement> maListe =monGoogle->listeFiltreeJour(monGoogle->listeCompleteEvenements(),QDate::currentDate());
        if (!maListe.isEmpty()) maListe=monGoogle->listeTriee(maListe);
//        monGoogle->monCalendar->listeCouleurs();
        if (!maListe.isEmpty())
        {
            for (int i=0;i<monGoogle->listeCheckBox().count();i++)
            {
                bool present =false;
                foreach (googleCalendar::evenement event, maListe)
                {
                    if (monGoogle->listeCheckBox().at(i)->objectName()==event.calendarId) present = true;
                }
                monGoogle->listeCheckBox().at(i)->setChecked(present);
            }
            setListeEvenementsAffiches(maListe);
        }
    }
}

void calendrier::onCheckboxCalendrierClicked()
{
    QList<googleCalendar::evenement> listeRetour;
    listeRetour.clear();
    int index=-1;
    for (int i=0;i<monGoogle->listeCheckBox().count();i++)
    {
        if (monGoogle->listeCheckBox().at(i)->objectName()==sender()->objectName())
            index =i;
    }
    if (index!=-1)
    {
        QList<googleCalendar::evenement> listeEvents = monGoogle->listeCompleteEvenements();
        if (!listeEvents.isEmpty())
        {
            foreach (QCheckBox *box, monGoogle->listeCheckBox())
            {
                if (box->isChecked())
                {
                    foreach (googleCalendar::evenement temp, listeEvents)
                    {
                        if (box->objectName()==temp.calendarId)
                            listeRetour<<temp;
                    }
                }
            }
            listeRetour=monGoogle->listeFiltreeJour(listeRetour, QDate::currentDate());
            listeRetour = monGoogle->listeTriee(listeRetour);
            setListeEvenementsAffiches(listeRetour);
        }
    }
}

void calendrier::onListeEvenementsAffichesChanged()
{
//    ui->textEdit->clear();
    ui->listWidget->clear();
    QList<googleCalendar::evenement> maListe = listeEvenementsAffiches();
    foreach (googleCalendar::evenement event, maListe)
    {
//        ui->textEdit->append(QString::number(event.startDateTime.date().day())+" - "+QString::number(event.startDateTime.date().month())+" - "+QString::number(event.startDateTime.date().year())+ " - " + monGoogle->listeCalendrier().value(event.calendarId).toString());
//        ui->textEdit->append(event.summary+ " - " + monGoogle->listeCalendrier().value(event.calendarId).toString() );
            QListWidgetItem *item = new QListWidgetItem(event.summary+ " - " + monGoogle->listeCalendrier().value(event.calendarId).toString(),ui->listWidget);
            Q_UNUSED(item);
    }
//    monGoogle->monCalendar->listeCouleurs();
}

void calendrier::onListeCouleursChanged()
{
    QMap<QString, QString> listeCoul = monGoogle->monCalendar->listeCouleurs();
    if (listeCoul.isEmpty()) return;
    QList<QCheckBox*> listeBox = monGoogle->listeCheckBox();
    if (listeBox.isEmpty()) return;
    QList<googleCalendar::evenement> maListe = listeEvenementsAffiches();
    if (maListe.isEmpty()) return;
    QString tableau[3][listeBox.count()];
    for (int i=0;i<listeBox.count();i++)
    {
        tableau[0][i] = listeBox.at(i)->objectName();
        tableau[1][i]=listeCoul.keys().at(i);
        tableau[2][i]=listeCoul.values().at(i);
        QString texte = "Color : " + listeCoul.keys().at(i) + ";";
        QCheckBox *box = listeBox.at(i);
        box->setStyleSheet(texte);
    }
    ui->listWidget->clear();
    for (int i=0;i<maListe.count();i++)
    {
        QListWidgetItem *item;
        item = new QListWidgetItem(maListe.at(i).summary,ui->listWidget);
        for (int j=0;j<listeBox.count();j++)
        {
            if (maListe.at(i).calendarId==tableau[0][j])
            {
//                item->setBackground(QBrush(QColor(tableau[1][j])));
                item->setForeground(QBrush(QColor(tableau[1][j])));
            }
        }
    }
}

void calendrier::on_pushButton_clicked()
{
//    QDate maDate;
//    maDate.setDate(2016,5,8);
//    QList<googleCalendar::evenement> listeEvents =monGoogle->listeFiltreeJourSelectionne(monGoogle->listeCompleteEvenements(),/*QDate::currentDate()*/maDate);
//    if (!listeEvents.isEmpty())
//    {
//        listeEvents=monGoogle->listeTriee(listeEvents);
//        setListeEvenementsAffiches(listeEvents);
//    }
//    else
//    {
//        ui->textEdit->clear();
//        ui->textEdit->append("Pas d'évènement aujourd'hui!");
//    }
//    googleCalendar *newCal= new googleCalendar(this);
//    newCal->setO2(monGoogle->oauth);
//    monGoogle->monCalendar->requeteurCal = newCal->requeteurCal;
//    connect(newCal->requeteurCal,SIGNAL(finished(int,QNetworkReply::NetworkError,QByteArray)),monGoogle->monCalendar,SLOT(onFinished(int,QNetworkReply::NetworkError,QByteArray)));
    QMap<QString, QString> liste = monGoogle->monCalendar->listeCouleurs();
}
