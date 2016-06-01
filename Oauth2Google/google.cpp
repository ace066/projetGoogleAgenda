#include "google.h"
//#include "ui_google.h"

Google::Google(QWidget *parent) :
    QWidget(parent)
{
    oauth = new O2(this);
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    requeteur = new O2Requestor(manager,oauth,this);
    monCalendar = new googleCalendar(this);

    QCoreApplication::setOrganizationName("Mon Entreprise");
    QCoreApplication::setApplicationName("Mon Application");
    mesParametres = new QSettings;
//    ui->ledt_IDClient->setText(mesParametres->value(GOOGLE_ID).toString());
//    ui->ledt_secretClient->setText(mesParametres->value(GOOGLE_SECRET).toString());
//    ui->ledt_URLAuth->setText(mesParametres->value(GOOGLE_URL_AUTHENTIFICATION).toString());
//    ui->ledt_scope->setText(mesParametres->value(GOOGLE_SCOPE).toString());
//    ui->ledt_URLToken->setText(mesParametres->value(GOOGLE_URL_TOKEN).toString());
//    ui->ledt_code->setText(mesParametres->value(GOOGLE_CODE_AUTHORISATION).toString());
//    ui->ledt_accessToken->setText(mesParametres->value(GOOGLE_ACCESS_TOKEN).toString());
//    ui->ledt_refreshToken->setText(mesParametres->value(GOOGLE_REFRESH_TOKEN).toString());
//    ui->ledt_refreshTokenURL->setText(mesParametres->value(GOOGLE_REFRESH_TOKEN_URL).toString());

    connect(monCalendar,SIGNAL(listeCalendrierChanged()),this,SLOT(onListeCalendriersChanged()));
    connect(monCalendar,SIGNAL(listeEvenementsChanged()),this,SLOT(onListeEvenementsChanged()));
    connect(monCalendar,SIGNAL(detailEvenementChanged()),this,SLOT(onDetailEvenementChanged()));

    connect(monCalendar,SIGNAL(eventEnregistre()),this,SLOT(onEventEnregistre()));
    connect(monCalendar,SIGNAL(eventMaj()),this,SLOT(onEventMaj()));
    connect(monCalendar,SIGNAL(eventDeleted()),this,SLOT(onEventDeleted()));
    connect(monCalendar,SIGNAL(calendarAjoute()),this,SLOT(onCalendarAjoute()));
    connect(monCalendar,SIGNAL(calendarMaj()),this,SLOT(onCalendarMaj()));
    connect(monCalendar,SIGNAL(calendarDeleted()),this,SLOT(onCalendarDeleted()));
    connect(monCalendar,SIGNAL(listeCouleursChanged()),this,SLOT(onListeCouleursChanged()));

    connect(oauth,SIGNAL(openBrowser(QUrl)),this,SLOT(onOpenBrowser(QUrl)));
    connect(oauth,SIGNAL(linkingSucceeded()),this,SLOT(onLinkedSuccess()));
    connect(oauth,SIGNAL(clientIdChanged()),this,SLOT(onClientIDChanged()));
    connect(oauth,SIGNAL(clientSecretChanged()),this,SLOT(onClientSecretChanged()));
    connect(oauth,SIGNAL(requestUrlChanged()),this,SLOT(onUrlAuthChanged()));
    connect(oauth,SIGNAL(scopeChanged()),this,SLOT(onScopeChanged()));
    connect(oauth,SIGNAL(tokenUrlChanged()),this,SLOT(onTokenUrlChanged()));
    connect(oauth,SIGNAL(codeChanged()),this,SLOT(onCodeChanged()));
    connect(oauth,SIGNAL(accessTokenChanged()),this,SLOT(onAccessTokenChanged()));
    connect(oauth,SIGNAL(refreshTokenChanged()),this,SLOT(onRefreshTokenChanged()));
    connect(oauth,SIGNAL(refreshTokenUrlChanged()),this,SLOT(onRefreshTokenUrlChanged()));

    if (mesParametres->value(GOOGLE_URL_AUTHENTIFICATION).toString()=="") mesParametres->setValue(GOOGLE_URL_AUTHENTIFICATION,GOOGLE_CODE_AUTHORISATION);
    if (mesParametres->value(GOOGLE_URL_TOKEN).toString()=="") mesParametres->setValue(GOOGLE_URL_TOKEN,GOOGLE_URL_TOKEN);
    if (mesParametres->value(GOOGLE_REFRESH_TOKEN_URL).toString()=="") mesParametres->setValue(GOOGLE_REFRESH_TOKEN_URL,GOOGLE_REFRESH_TOKEN_URL);

//    connect(ui->cbx_ListeCalendriers,SIGNAL(currentTextChanged(QString)),this,SLOT(onSelectionCalendrierChanged(QString)));
//    connect(ui->cbx_listeEvenements,SIGNAL(currentTextChanged(QString)),this,SLOT(onSelectionEvenementChanged(QString)));

    remplissageListeCompleteEvents=true;
    remplissageOauth(mesParametres->value(GOOGLE_CODE_AUTHORISATION).toString(), mesParametres->value(GOOGLE_ACCESS_TOKEN).toString(), mesParametres->value(GOOGLE_ID).toString(),mesParametres->value(GOOGLE_SECRET).toString(),mesParametres->value(GOOGLE_SCOPE).toString(),mesParametres->value(GOOGLE_URL_AUTHENTIFICATION).toString(),mesParametres->value(GOOGLE_URL_TOKEN).toString(),mesParametres->value(GOOGLE_REFRESH_TOKEN_URL).toString(),mesParametres->value(GOOGLE_REFRESH_TOKEN).toString());

}

Google::~Google()
{
//    delete ui;
}

void Google::connexion()
{
    if ((oauth->refreshToken()!="") & (oauth->refreshTokenUrl()!=""))
        oauth->refresh();
    else
        oauth->link();
}

void Google::deconnexion()
{
    oauth->unlink();
}

void Google::setClientID(QString valeur)
{
    oauth->setClientId(valeur);
}

QString Google::clientID()
{
    return oauth->clientId();
}

void Google::setClientSecret(QString valeur)
{
    oauth->setClientSecret(valeur);
}

QString Google::clientSecret()
{
    return oauth->clientSecret();
}

void Google::setScope(QString valeur)
{
    oauth->setScope(valeur);
}

QString Google::scope()
{
    return oauth->scope();
}

void Google::appelAPICalendarListe()
{
//    remplissageOauth(mesParametres->value(GOOGLE_ACCESS_TOKEN).toString(), ui->ledt_IDClient->text(),ui->ledt_secretClient->text(),ui->ledt_scope->text(),ui->ledt_URLAuth->text(),ui->ledt_URLToken->text(),ui->ledt_refreshTokenURL->text(),ui->ledt_refreshToken->text());
    monCalendar->setApiDemandee(monCalendar->CalendarList);
    monCalendar->setO2(oauth);
    monCalendar->listeCalendriers();
}

void Google::appelAPIEffaceEvent(QString calendrierId, QString eventId)
{
    monCalendar->setApiDemandee(monCalendar->CalendarDeleteEvent);
    monCalendar->setO2(oauth);
    monCalendar->effaceEvent(calendrierId, eventId);
}

void Google::appelAPIMajEvent(QString calendrierID, QString eventId, QString description, QString summary, bool journeeEntiere, QDateTime debut, QDateTime fin)
{
    monCalendar->setApiDemandee(monCalendar->CalendarMajEvent);
    monCalendar->setO2(oauth);
    monCalendar->majevent(calendrierID, eventId,description,summary,journeeEntiere, debut,fin);
}

void Google::appelAPIPosteEvent(QString calendrierID, QString description, QString summary, bool journeeEntiere, QDateTime debut, QDateTime fin)
{
    monCalendar->setApiDemandee(monCalendar->CalendarPostEvent);
    monCalendar->setO2(oauth);
//    for (int i=0;i<monCalendar->listeCalendriers().count();i++)
//        if (monCalendar->listeCalendriers().value(monCalendar->listeCalendriers().keys().at(i))==ui->cbx_ListeCalendriers->currentText())    monCalendar->setCalendrierIdDefaut(monCalendar->listeCalendriers().keys().at(i));
    monCalendar->setCalendrierIdDefaut(calendrierID);
    monCalendar->posteEvent(calendrierID, description,summary,journeeEntiere, debut,fin);
}

void Google::appelAPIPosteCalendrier(QString titre)
{
    monCalendar->setApiDemandee(monCalendar->CalendarPosteCalendar);
    monCalendar->setO2(oauth);
    monCalendar->ajouteCalendrier(titre);
}

void Google::appelAPIMajCalendrier(QString calendrierId, QString nouveauTitre)
{
    monCalendar->setApiDemandee(monCalendar->CalendarMajCalendar);
    monCalendar->setO2(oauth);
    monCalendar->majCalendrier(calendrierId,nouveauTitre);
}

void Google::appelAPIEffaceCalendrier(QString calendrierId)
{
    monCalendar->setApiDemandee(monCalendar->CalendarDeleteCalendar);
    monCalendar->setO2(oauth);
    monCalendar->effaceCalendrier(calendrierId);
}

QMap<QString, QVariant> Google::listeCalendrier()
{
    return m_listeCalendrier;
}

void Google::setListeCalendrier(QMap<QString, QVariant> liste)
{
    m_listeCalendrier=liste;
    emit listeCalendrierChanged();
}

QMap<QString, QVariant> Google::listeEvenements()
{
    return m_listeEvenements;
}

void Google::setListeEvenements(QMap<QString, QVariant> liste)
{
    m_listeEvenements=liste;
    emit listeEvenementsChanged();
}

googleCalendar::evenement Google::evenementSelectionne()
{
    return evenement;
}

void Google::ajouteCheckBox(QCheckBox *boite)
{
    m_listeCheckbox<<boite;
}

void Google::retireCheckBox(QString nomObjet)
{
    int index =-1;
    for (int i=0;i<m_listeCheckbox.count();i++)
    {
        QCheckBox *temp = m_listeCheckbox.at(i);
        if (temp->objectName()==nomObjet) index =i;
    }
    if (index!=-1) m_listeCheckbox.removeAt(index);
}

QList<QCheckBox* > Google::listeCheckBox()
{
    return m_listeCheckbox;
}

void Google::ajouteEvenementListe(googleCalendar::evenement temp)
{
    m_listeCompleteEvenements<<temp;
}

QList<googleCalendar::evenement> Google::listeCompleteEvenements()
{
    if (m_listeCompleteEvenements.isEmpty())
    {
        remplissageListeCompleteEvents=true;
        monCalendar->setCalendrierIdDefaut(listeCalendrier().keys().first());
        monCalendar->remplissageListeEvenements(listeCalendrier().keys().first());
    }
    return m_listeCompleteEvenements;
}

QList<googleCalendar::evenement> Google::listeFiltreeJour(QList<googleCalendar::evenement> listeComplete, QDate dateJour)
{
    QList<googleCalendar::evenement> maListe;
    foreach (googleCalendar::evenement temp, listeComplete)
    {
        if (temp.startDate.year()!=0&&temp.startDate.year()>=dateJour.year())
        {
            if (temp.startDate.year()!=0&&temp.startDate.year()>dateJour.year())
                maListe<<temp;
            else
            {
                if (temp.startDate.month()!=0&&temp.startDate.month()>=dateJour.month())
                {
                    if (temp.startDate.month()!=0&&temp.startDate.month()>dateJour.month())
                        maListe<<temp;
                    else
                    {
                        if (temp.startDate.day()!=0&&temp.startDate.day()>=dateJour.day())
                            maListe<<temp;
                    }
                }
            }
        }
        if (temp.startDateTime.date().year()!=0&&temp.startDateTime.date().year()>=dateJour.year())
        {
            if (temp.startDateTime.date().year()!=0&&temp.startDateTime.date().year()>dateJour.year())
                maListe<<temp;
            else
            {
                if (temp.startDateTime.date().month()!=0&&temp.startDateTime.date().month()>=dateJour.month())
                {
                    if (temp.startDateTime.date().month()!=0&&temp.startDateTime.date().month()>dateJour.month())
                        maListe<<temp;
                    else
                    {
                        if (temp.startDateTime.date().day()!=0&&temp.startDateTime.date().day()>=dateJour.day())
                            maListe<<temp;
                    }
                }
            }
        }
    }
    return maListe;
}

QList<googleCalendar::evenement> Google::listeTriee(QList<googleCalendar::evenement> listeFiltree)
{
    QList<googleCalendar::evenement> listeAnnee, listeRetour;
    listeAnnee=listeFiltree;
    /* Pour que l'item StartDateTime soit renseigné pour chaque évènement*/
    foreach (googleCalendar::evenement event, listeAnnee)
    {
        if (event.startDate.year()==0&&event.startDateTime.date().year()!=0)
        {
            event.startDate.setDate(event.startDateTime.date().year(),event.startDateTime.date().month(),event.startDateTime.date().day());
        }
        else
        {
            if (event.startDateTime.date().year()==0&&event.startDate.year()!=0) event.startDateTime.date().setDate(event.startDate.year(),event.startDate.month(),event.startDate.day());
        }
    }

    /*Tri de la liste en fonction de l'année*/
    int i, j;
    for (i=1;i<listeAnnee.count();i++)
    {
        googleCalendar::evenement temp= listeAnnee.at(i);
        for (j=i;j>0&&listeAnnee.at(j-1).startDateTime.date().year()>=temp.startDateTime.date().year();j--)
        {
            listeAnnee.move(j-1,j);
        }
    }
    /*Séparation des évènements de la liste par année (tous les 2016 dans une liste, 2017 dans une autre, etc..)*/
    QList<QList<googleCalendar::evenement> > listeMois;
    QList<googleCalendar::evenement> listeMonth;
    int ref = listeAnnee.first().startDateTime.date().year();
    foreach (googleCalendar::evenement temp, listeAnnee)
    {
        if (temp.startDateTime.date().year()!=ref)
        {
            listeMois<<listeMonth;
            listeMonth.clear();
            ref=temp.startDateTime.date().year();
        }
        listeMonth<<temp;
        if (temp.id==listeAnnee.last().id) listeMois<<listeMonth;
    }

    /*Tri de chaque liste d'années en fonction des mois dans une liste de transition --> tamponMois*/
    QList<QList<googleCalendar::evenement> > tamponMois;
    foreach (QList<googleCalendar::evenement> liste, listeMois)
    {
        for (i=1;i<liste.count();i++)
        {
            googleCalendar::evenement temp = liste.at(i);
            for (j=i;j>0&&liste.at(j-1).startDateTime.date().month()>temp.startDateTime.date().month();j--)
            {
                liste.move(j-1,j);
            }
        }
        tamponMois<<liste;
    }

    QList<QList<googleCalendar::evenement> > listeJour;
    QList<googleCalendar::evenement> listeday;
    ref= tamponMois.first().first().startDateTime.date().month();
    foreach (QList<googleCalendar::evenement> liste, tamponMois)
    {
        foreach (googleCalendar::evenement event, liste)
        {
            if (event.startDateTime.date().month()!=ref)
            {
                listeJour<<listeday;
                listeday.clear();
                ref=event.startDateTime.date().month();
            }
            listeday<<event;
            if (event.id==tamponMois.last().last().id)listeJour<<listeday;
        }
    }

    QList<QList<googleCalendar::evenement> > tamponJour;
    foreach (QList<googleCalendar::evenement> liste, listeJour)
    {
        for (i=1;i<liste.count();i++)
        {
            googleCalendar::evenement temp = liste.at(i);
            for (j=i;j>0&&liste.at(j-1).startDateTime.date().day()>temp.startDateTime.date().day();j--)
            {
                liste.move(j-1,j);
            }
        }
        tamponJour<<liste;
    }

    listeRetour.clear();
    foreach (QList <googleCalendar::evenement> temp, tamponJour)
    {
        foreach (googleCalendar::evenement tempo, temp)
        {
            listeRetour<<tempo;
        }
    }
    return listeRetour;
}

QList<googleCalendar::evenement> Google::listeFiltreeJourSelectionne(QList<googleCalendar::evenement> listeComplete, QDate dateJourSelectionne)
{
    QList<googleCalendar::evenement> listeRetour;
    QList<googleCalendar::evenement> listetampon;
    foreach (googleCalendar::evenement event, listeComplete)
    {
        if (event.startDate.year()==0&&event.startDateTime.date().year()!=0)
        {
            event.startDate.setDate(event.startDateTime.date().year(),event.startDateTime.date().month(),event.startDateTime.date().day());
        }
        if (event.startDateTime.date().year()==0&&event.startDate.year()!=0)
        {
            event.startDateTime.date().setDate(event.startDate.year(),event.startDate.month(),event.startDate.day());
        }
        listetampon<<event;
    }
    if (!listetampon.isEmpty())
    {
        int annee = dateJourSelectionne.year();
        int mois = dateJourSelectionne.month();
        int jour = dateJourSelectionne.day();
        foreach (googleCalendar::evenement temp, listetampon)
        {
            int anneeTemp, moisTemp, jourTemp;
            if (temp.startDate.year()!=0)anneeTemp=temp.startDate.year();
            if (temp.startDate.month()!=0)moisTemp=temp.startDate.month();
            if (temp.startDate.day()!=0)jourTemp=temp.startDate.day();
            if (annee==anneeTemp&&mois==moisTemp&&jour==jourTemp)
                listeRetour<<temp;
        }
    }

    return listeRetour;
}

void Google::onOpenBrowser(QUrl adresse)
{
    QDesktopServices::openUrl(adresse);
}

void Google::onLinkedSuccess()
{
//    ui->btn_connexion->setText("Connecté");
    emit connexionEtablie();
//    appelAPICalendarListe();
}

void Google::onClientIDChanged()
{
    if (oauth->clientId()!="")
    {
//        ui->ledt_IDClient->setText(oauth->clientId());
        if (mesParametres->value(GOOGLE_ID).toString()!=oauth->clientId()) mesParametres->setValue(GOOGLE_ID,oauth->clientId());
    }
}

void Google::onClientSecretChanged()
{
    if (oauth->clientSecret()!="")
    {
//        ui->ledt_secretClient->setText(oauth->clientSecret());
        if (mesParametres->value(GOOGLE_SECRET).toString()!=oauth->clientId()) mesParametres->setValue(GOOGLE_SECRET,oauth->clientSecret());
    }
}

void Google::onUrlAuthChanged()
{
    if (oauth->requestUrl()!="")
    {
//        ui->ledt_URLAuth->setText(oauth->requestUrl());
        if (mesParametres->value(GOOGLE_URL_AUTHENTIFICATION).toString()!=oauth->requestUrl()) mesParametres->setValue(GOOGLE_URL_AUTHENTIFICATION,oauth->requestUrl());
    }
}

void Google::onScopeChanged()
{
    if (oauth->scope()!="")
    {
//        ui->ledt_scope->setText(oauth->scope());
        if (mesParametres->value(GOOGLE_SCOPE).toString()!=oauth->scope()) mesParametres->setValue(GOOGLE_SCOPE,oauth->scope());
    }
}

void Google::onTokenUrlChanged()
{
    if (oauth->tokenUrl()!="")
    {
//        ui->ledt_URLToken->setText(oauth->tokenUrl());
        if (mesParametres->value(GOOGLE_URL_TOKEN).toString()!=oauth->tokenUrl()) mesParametres->setValue(GOOGLE_URL_TOKEN,oauth->tokenUrl());
    }
}

void Google::onCodeChanged()
{
    if (oauth->code()!="")
    {
//        ui->ledt_code->setText(oauth->code());
        if (mesParametres->value(GOOGLE_CODE_AUTHORISATION).toString()!=oauth->code()) mesParametres->setValue(GOOGLE_CODE_AUTHORISATION,oauth->code());
    }
}

void Google::onAccessTokenChanged()
{
    if (oauth->token()!="")
    {
//        ui->ledt_accessToken->setText(oauth->token());
        if (mesParametres->value(GOOGLE_ACCESS_TOKEN).toString()!=oauth->token()) mesParametres->setValue(GOOGLE_ACCESS_TOKEN,oauth->token());
    }
}

void Google::onRefreshTokenChanged()
{
    if (oauth->refreshToken()!="")
    {
//        ui->ledt_refreshToken->setText(oauth->refreshToken());
        if (mesParametres->value(GOOGLE_REFRESH_TOKEN).toString()!=oauth->refreshToken()) mesParametres->setValue(GOOGLE_REFRESH_TOKEN,oauth->refreshToken());
    }
}

void Google::onRefreshTokenUrlChanged()
{
    if (oauth->refreshTokenUrl()!="")
    {
//        ui->ledt_refreshTokenURL->setText(oauth->refreshTokenUrl());
        if (mesParametres->value(GOOGLE_REFRESH_TOKEN_URL).toString()!=oauth->refreshTokenUrl()) mesParametres->setValue(GOOGLE_REFRESH_TOKEN_URL,oauth->refreshTokenUrl());
    }
}

void Google::onListeCalendriersChanged()
{
//    ui->cbx_ListeCalendriers->clear();
//    QMap<QString, QVariant> retour = monCalendar->listeCalendriers();
//    foreach (QString index, retour.keys()) ui->cbx_ListeCalendriers->addItem(retour.value(index).toString());
    setListeCalendrier(monCalendar->listeCalendriers());
    if (remplissageListeCompleteEvents)
    {
        googleCalendar *newCal= new googleCalendar(this);
        newCal->setO2(oauth);
        monCalendar->requeteurCal = newCal->requeteurCal;
        connect(newCal->requeteurCal,SIGNAL(finished(int,QNetworkReply::NetworkError,QByteArray)),monCalendar,SLOT(onFinished(int,QNetworkReply::NetworkError,QByteArray)));
        newCal->remplissageListeEvenements(monCalendar->calendrierIdDefaut());
    }
}

void Google::onSelectionCalendrierChanged(QString valeur)
{
    monCalendar->setCalendrierIdDefaut(m_listeCalendrier.key(QVariant(valeur)));
    monCalendar->setCalendrierLibelleDefaut(valeur);
    monCalendar->setApiDemandee(monCalendar->CalendarEvents);
    monCalendar->effaceListeEvenements();
//    ui->cbx_listeEvenements->clear();
    monCalendar->remplissageListeEvenements(monCalendar->calendrierIdDefaut());
}

void Google::onListeEvenementsChanged()
{
//    QMap<QString, QVariant> maListe = monCalendar->listeEvenements();
//    ui->cbx_listeEvenements->clear();
//    foreach (QString index, maListe.keys()) ui->cbx_listeEvenements->addItem(maListe.value(index).toString());
    setListeEvenements(monCalendar->listeEvenements());

    if (remplissageListeCompleteEvents)
    {
        if (monCalendar->jsonValeur.toArray().count()>0)
        {
            for (int i=0;i<monCalendar->jsonValeur.toArray().count();i++)
            {
                QJsonObject temp = monCalendar->jsonValeur.toArray().at(i).toObject();
                googleCalendar::evenement event;
                event.calendarId=monCalendar->calendrierIdDefaut();
                event.id=temp.value("id").toString();
                for (int j=0;j<listeCompleteEvenements().count();j++)
                {
                    if (event.id==listeCompleteEvenements().at(j).id) return;
                }
                event.summary=temp.value("summary").toString();

                if (temp.value("start").toObject().value("dateTime").toString()!="")
                {
                    QStringList liste = QString(temp.value("start").toObject().value("dateTime").toString()).split("T");
                    QStringList date = QString(liste[0]).split("-");
                    QString annee = date[0];
                    QString mois = date[1];
                    QString jour = date[2];
                    event.startDate.setDate(annee.toInt(),mois.toInt(),jour.toInt());
                    QStringList time = QString(liste[1]).split(":");
                    QString heures = time[0];
                    QString minutes = time[1];
                    QStringList listesecondes = QString(time[2]).split("Z");
                    QString secondes = listesecondes[0];
                    event.startDateTime.setTime(QTime(heures.toInt(),minutes.toInt(),secondes.toInt()));
                }
                else
                {
                    if (temp.value("start").toObject().value("date").toString()!="")
                    {
                        QStringList date = QString(temp.value("start").toObject().value("date").toString()).split("-");
                        QString annee = date[0];
                        QString mois = date[1];
                        QString jour = date[2];
                        event.startDateTime.setDate(QDate(annee.toInt(),mois.toInt(),jour.toInt()));
                    }
                }
                if (temp.value("end").toObject().value("date").toString()!="")
                {
                    QStringList temp2 =QString(temp.value("end").toObject().value("date").toString()).split('-');
                    QString annee = temp2[0];
                    QString mois = temp2[1];
                    QString jour = temp2[2];
                    event.endDate.setDate(annee.toInt(),mois.toInt(),jour.toInt());
                }
                if (temp.value("end").toObject().value("dateTime").toString()!="")
                {
                    QStringList liste = QString(temp.value("end").toObject().value("dateTime").toString()).split("T");
                    QStringList date = QString(liste[0]).split("-");
                    QString annee = date[0];
                    QString mois = date[1];
                    QString jour = date[2];
                    event.endDateTime.setDate(QDate(annee.toInt(),mois.toInt(),jour.toInt()));
                    QStringList time = QString(liste[1]).split(":");
                    QString heures = time[0];
                    QString minutes = time[1];
                    QStringList listesecondes = QString(time[2]).split("Z");
                    QString secondes = listesecondes[0];
                    event.endDateTime.setTime(QTime(heures.toInt(),minutes.toInt(),secondes.toInt()));
                }
                event.startTimeZone=temp.value("start").toObject().value("timeZone").toString();
                event.endTimeZone=temp.value("end").toObject().value("timeZone").toString();
                m_listeCompleteEvenements<<event;
            }
            if (monCalendar->calendrierIdDefaut()!=listeCalendrier().keys().last())
            {
                int index=listeCalendrier().keys().indexOf(monCalendar->calendrierIdDefaut())+1;
                monCalendar->setCalendrierIdDefaut(listeCalendrier().keys().at(index));
                bool reponse = monCalendar->requeteurCal->reply_->isFinished();
                if (reponse)
                {
                    googleCalendar *newCal= new googleCalendar(this);
                    newCal->setO2(oauth);
                    monCalendar->requeteurCal = newCal->requeteurCal;
                    connect(newCal->requeteurCal,SIGNAL(finished(int,QNetworkReply::NetworkError,QByteArray)),monCalendar,SLOT(onFinished(int,QNetworkReply::NetworkError,QByteArray)));
                    newCal->remplissageListeEvenements(monCalendar->calendrierIdDefaut());
                }
            }
            else
            {
                if (monCalendar->calendrierIdDefaut()==listeCalendrier().keys().last())
                {
                    remplissageListeCompleteEvents=false;
                    emit listeCompleteEvenementsChanged();
                }
            }
        }
    }

}

void Google::onSelectionEvenementChanged(QString valeur)
{
    monCalendar->setEventIdSelect(monCalendar->listeEvenements().key(QVariant(valeur)));
    monCalendar->setEventLibelleSelect(valeur);
    monCalendar->detailEvenement();
}

void Google::onDetailEvenementChanged()
{
    emit detailEventChanged();
}

void Google::onListeCouleursChanged()
{
    emit listeCouleursChanged();
}

void Google::onEventEnregistre()
{
    //lorsque l'event saisi est enregistré
    emit eventEnregistre();
}

void Google::onEventMaj()
{
    //lorsque l'event saisi est mis à jour
    emit eventMaj();
}

void Google::onEventDeleted()
{
    //lorsque l'event saisi est supprimé
    emit eventDeleted();
}

void Google::onCalendarAjoute()
{
    //lorsque le calendrier est enregistré
    emit calendarAjoute();
}

void Google::onCalendarMaj()
{
    //lorsque le calendrier est mis à jour
    emit calendarMaj();
}

void Google::onCalendarDeleted()
{
    //lorsque le calendrier est suprrimé
    emit calendarDeleted();
}

void Google::on_btn_connexion_clicked()
{
//    remplissageOauth(ui->ledt_accessToken->text(),ui->ledt_IDClient->text(),ui->ledt_secretClient->text(),ui->ledt_scope->text(),ui->ledt_URLAuth->text(),ui->ledt_URLToken->text(),ui->ledt_refreshTokenURL->text(),ui->ledt_refreshToken->text());
    connexion();
}

void Google::remplissageOauth(QString code, QString token, QString clientID, QString clientSecret, QString scope, QString urlAuth, QString urlToken, QString urlRefreshToken, QString refreshToken)
{
    oauth->setCode(code);
    oauth->setClientId(clientID);
    oauth->setClientSecret(clientSecret);
    oauth->setScope(scope);
    oauth->setRequestUrl(urlAuth);
    oauth->setTokenUrl(urlToken);
    oauth->setToken(token);
    oauth->setRefreshTokenUrl(urlRefreshToken);
    oauth->setRefreshToken(refreshToken);
}

googleCalendar::googleCalendar(QWidget *parent): QWidget(parent)
{
    QCoreApplication::setOrganizationName("Mon Entreprise");
    QCoreApplication::setApplicationName("Mon Application");
    mesParametres = new QSettings;
    oauthCal = new O2(this);
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    requeteurCal = new O2Requestor(manager,oauthCal,this);
    connect(requeteurCal,SIGNAL(finished(int,QNetworkReply::NetworkError,QByteArray)),this,SLOT(onFinished(int,QNetworkReply::NetworkError,QByteArray)));
}

googleCalendar::~googleCalendar()
{

}

QString googleCalendar::urlListeAgendas()
{
    return CALENDAR_URLCALENDAR_LIST;
}

void googleCalendar::setUrlListeAgendas(QString valeur)
{
    mesParametres->setValue(CALENDAR_URLCALENDAR_LIST,valeur);
    emit urlListeAgendasChanged();
}

QString googleCalendar::urlListeEvenements()
{
    return CALENDAR_URLEVENTS_LIST;
}

void googleCalendar::setUrlListeEvenements(QString valeur)
{
    mesParametres->setValue(CALENDAR_URLEVENTS_LIST,valeur);
    emit urlListeEvenementsChanged();
}

QString googleCalendar::calendrierIdDefaut()
{
    return mesParametres->value(CALENDAR_CAL_ID_DEFAUT).toString();
}

void googleCalendar::setCalendrierIdDefaut(QString valeur)
{
    mesParametres->setValue(CALENDAR_CAL_ID_DEFAUT,valeur);
    emit calendrierIdDefautChanged();
}

QString googleCalendar::calendrierLibelleDefaut()
{
    return mesParametres->value(CALENDAR_CAL_LIBELLE_DEFAUT).toString();
}

void googleCalendar::setCalendrierLibelleDefaut(QString valeur)
{
    mesParametres->setValue(CALENDAR_CAL_LIBELLE_DEFAUT,valeur);
    emit calendrierLibelleDefautChanged();
}

QString googleCalendar::eventIdSelect()
{
    return m_eventIdDefaut;
}

void googleCalendar::setEventIdSelect(QString valeur)
{
    m_eventIdDefaut=valeur;
    emit eventIdSelectChanged();
}

QString googleCalendar::eventLibelleSelect()
{
    return m_eventLibelleDefaut;
}

void googleCalendar::setEventLibelleSelect(QString valeur)
{
    m_eventLibelleDefaut = valeur;
    emit eventLibelleSelectChanged();
}

QMap<QString, QVariant> googleCalendar::listeCalendriers()
{
    if (m_listeCalendriers.isEmpty())
    {
        setApiDemandee(CalendarList);
        QNetworkRequest *maRequete = new QNetworkRequest(QUrl(urlListeAgendas()));
        requeteurCal->setO2(oauthCal);
        requeteurCal->get(*maRequete);
    }
    return m_listeCalendriers;
}

void googleCalendar::setListeCalendriers(QMap<QString, QVariant> liste)
{
    /*if(!liste.isEmpty())*/ m_listeCalendriers = liste;
    emit listeCalendrierChanged();
}

QMap<QString, QVariant> googleCalendar::listeEvenements()
{
    return m_listeEvenements;
}

void googleCalendar::setListeEvenements(QMap<QString, QVariant> liste)
{
    m_listeEvenements = liste;
    emit listeEvenementsChanged();
}

void googleCalendar::detailEvenement()
{
    if (m_apiUtilisee==CalendarGetEvent)
    {
        QByteArray temp = QUrl::toPercentEncoding(calendrierIdDefaut());
        QByteArray temp2 = QUrl::toPercentEncoding(eventIdSelect());
        QNetworkRequest *maRequete = new QNetworkRequest(QUrl(urlListeEvenements()+temp+"/events/"+temp2));

        requeteurCal->setO2(oauthCal);
        requeteurCal->get(*maRequete);
    }
//    return m_detailEvenement;
}

void googleCalendar::setDetailEvenement(evenement liste)
{
    m_detailEvenement=liste;
    emit detailEvenementChanged();
}

QMap<QString, QString> googleCalendar::listeCouleurs()
{
    if (m_listeCouleurs.isEmpty())
    {
        googleCalendar *newCal= new googleCalendar;
        newCal->setO2(oauthCal);
        requeteurCal = newCal->requeteurCal;
        connect(newCal->requeteurCal,SIGNAL(finished(int,QNetworkReply::NetworkError,QByteArray)),this,SLOT(onFinished(int,QNetworkReply::NetworkError,QByteArray)));
        setApiDemandee(CouleurCalendars);
        QNetworkRequest *maRequete = new QNetworkRequest(QUrl("https://www.googleapis.com/calendar/v3/colors"));
        newCal->requeteurCal->setO2(oauthCal);
        newCal->requeteurCal->get(*maRequete);
//        requeteurCal->setO2(oauthCal);
//        requeteurCal->get(*maRequete);
    }
    return m_listeCouleurs;
}

void googleCalendar::setListeCouleurs(QMap<QString, QString> liste)
{
    m_listeCouleurs=liste;
    emit listeCouleursChanged();
}

void googleCalendar::setO2(O2 *oauth)
{
    oauthCal = oauth;
}

void googleCalendar::setApiDemandee(googleCalendar::listAppelApi valeur)
{
    m_apiUtilisee = valeur;
}

QString googleCalendar::calendrierId(QString libelleCalendrier)
{
    return listeCalendriers().key(QVariant(libelleCalendrier));
}

googleCalendar::listAppelApi googleCalendar::apiAppelee()
{
    return m_apiUtilisee;
}

void googleCalendar::effaceListeEvenements()
{
    m_listeEvenements.clear();
}

void googleCalendar::posteEvent(QString calendrierID, QString description, QString summary, bool journeeEntiere, QDateTime debut, QDateTime fin)
{
    if (m_apiUtilisee==CalendarPostEvent)
    {
        QJsonObject monobjet;

        if (!journeeEntiere)
        {
            QJsonObject temp2;
            QString dateFin = QString::number(fin.date().year()) + "-"+QString::number(fin.date().month())+ "-"+QString::number(fin.date().day())+"T"+fin.time().toString();
            temp2.insert("dateTime", QJsonValue(dateFin));
            temp2.insert("timeZone", QJsonValue("Europe/Paris"));
            monobjet.insert("end",QJsonValue(temp2));

            QString dateDebut = QString::number(debut.date().year()) + "-"+QString::number(debut.date().month())+ "-"+QString::number(debut.date().day())+"T"+debut.time().toString();
            QJsonObject temp;
            temp.insert("dateTime", QJsonValue(dateDebut));
            temp.insert("timeZone", QJsonValue("Europe/Paris"));
            monobjet.insert("start",QJsonValue(temp));
        }
        else
        {
            QJsonObject temp;
            QJsonObject temp2;
            QString dateDebut = QString::number(debut.date().year()) + "-"+QString::number(debut.date().month())+ "-"+QString::number(debut.date().day());
            QString dateFin = QString::number(debut.date().year()) + "-"+QString::number(debut.date().month())+ "-"+QString::number(debut.date().day()+1);
            temp.insert("date", QJsonValue(dateDebut));
            temp.insert("timeZone", QJsonValue("Europe/Paris"));
            monobjet.insert("start",QJsonValue(temp));
            temp2.insert("date", QJsonValue(dateFin));
            temp2.insert("timeZone", QJsonValue("Europe/Paris"));
            monobjet.insert("end",QJsonValue(temp2));
        }
        monobjet.insert("summary", QJsonValue(summary));
        monobjet.insert("description", (description));

        QJsonDocument doc;
        doc.setObject(monobjet);
        QByteArray body = doc.toJson(QJsonDocument::Indented);

        QByteArray temp3 = QUrl::toPercentEncoding(calendrierID);
        QNetworkRequest *maRequete = new QNetworkRequest(QUrl(urlListeEvenements()+temp3+"/events"));

        maRequete->setRawHeader("Content-Type","application/json");
        requeteurCal->setO2(oauthCal);
        requeteurCal->post(*maRequete,body);
    }
}

void googleCalendar::majevent(QString calendrierID, QString eventId, QString description, QString summary, bool journeeEntiere, QDateTime debut, QDateTime fin)
{
    if (m_apiUtilisee==CalendarMajEvent)
    {
        QJsonObject monobjet;

        if (!journeeEntiere)
        {
            QJsonObject temp2;
            QString dateFin = QString::number(fin.date().year()) + "-"+QString::number(fin.date().month())+ "-"+QString::number(fin.date().day())+"T"+fin.time().toString();
            temp2.insert("dateTime", QJsonValue(dateFin));
            temp2.insert("timeZone", QJsonValue("Europe/Paris"));
            monobjet.insert("end",QJsonValue(temp2));

            QString dateDebut = QString::number(debut.date().year()) + "-"+QString::number(debut.date().month())+ "-"+QString::number(debut.date().day())+"T"+debut.time().toString();
            QJsonObject temp;
            temp.insert("dateTime", QJsonValue(dateDebut));
            temp.insert("timeZone", QJsonValue("Europe/Paris"));
            monobjet.insert("start",QJsonValue(temp));
        }
        else
        {
            QJsonObject temp;
            QJsonObject temp2;
            QString dateDebut = QString::number(debut.date().year()) + "-"+QString::number(debut.date().month())+ "-"+QString::number(debut.date().day());
            QString dateFin = QString::number(debut.date().year()) + "-"+QString::number(debut.date().month())+ "-"+QString::number(debut.date().day()+1);
            temp.insert("date", QJsonValue(dateDebut));
            temp.insert("timeZone", QJsonValue("Europe/Paris"));
            monobjet.insert("start",QJsonValue(temp));
            temp2.insert("date", QJsonValue(dateFin));
            temp2.insert("timeZone", QJsonValue("Europe/Paris"));
            monobjet.insert("end",QJsonValue(temp2));
        }
        monobjet.insert("summary", QJsonValue(summary));
        monobjet.insert("description", (description));

        QJsonDocument doc;
        doc.setObject(monobjet);
        QByteArray body = doc.toJson(QJsonDocument::Indented);

        QByteArray temp3 = QUrl::toPercentEncoding(calendrierID);
        QByteArray temp4 = QUrl::toPercentEncoding(eventId);
        QNetworkRequest *maRequete = new QNetworkRequest(QUrl(urlListeEvenements()+temp3+"/events/"+temp4));

        maRequete->setRawHeader("Content-Type","application/json");
        requeteurCal->setO2(oauthCal);
        requeteurCal->put(*maRequete,body);
    }
}

void googleCalendar::effaceEvent(QString calendrierId, QString eventId)
{
    if (m_apiUtilisee==CalendarDeleteEvent)
    {
        QByteArray temp3 = QUrl::toPercentEncoding(calendrierId);
        QByteArray temp4 = QUrl::toPercentEncoding(eventId);
        QNetworkRequest *maRequete = new QNetworkRequest(QUrl(urlListeEvenements()+temp3+"/events/"+temp4));

        maRequete->setRawHeader("Content-Type","application/json");
        requeteurCal->setO2(oauthCal);
        requeteurCal->deleteResource(*maRequete);
    }
}

void googleCalendar::ajouteCalendrier(QString titre)
{
    if (m_apiUtilisee==CalendarPosteCalendar)
    {
        QNetworkRequest *maRequete = new QNetworkRequest(QUrl(urlListeEvenements()));
        maRequete->setRawHeader("Content-Type","application/json");
        requeteurCal->setO2(oauthCal);

        QJsonObject monObjet;
        monObjet.insert("summary",QJsonValue(titre));
        QJsonDocument doc;
        doc.setObject(monObjet);
        QByteArray body = doc.toJson(QJsonDocument::Indented);
        requeteurCal->post(*maRequete,body);
    }
}

void googleCalendar::majCalendrier(QString calendrierId, QString nouveauTitre)
{
    if (m_apiUtilisee==CalendarMajCalendar)
    {
        QByteArray temp = QUrl::toPercentEncoding(calendrierId);
        QNetworkRequest *maRequete = new QNetworkRequest(QUrl(urlListeEvenements()+temp));
        maRequete->setRawHeader("Content-Type","application/json");
        requeteurCal->setO2(oauthCal);

        QJsonObject monObjet;
        monObjet.insert("summary",QJsonValue(nouveauTitre));
        QJsonDocument doc;
        doc.setObject(monObjet);
        QByteArray body = doc.toJson(QJsonDocument::Indented);
        requeteurCal->put(*maRequete,body);
    }
}

void googleCalendar::effaceCalendrier(QString calendrierId)
{
    if (m_apiUtilisee==CalendarDeleteCalendar)
    {
        QByteArray temp = QUrl::toPercentEncoding(calendrierId);
        QNetworkRequest *maRequete = new QNetworkRequest(QUrl(urlListeEvenements()+temp));

        maRequete->setRawHeader("Content-Type","application/json");
        requeteurCal->setO2(oauthCal);
        requeteurCal->deleteResource(*maRequete);
    }
}


void googleCalendar::onFinished(int id, QNetworkReply::NetworkError erreur, QByteArray data)
{
    Q_UNUSED(id);
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject monObjet = doc.object();
    if (erreur==QNetworkReply::NoError)
    {
        if (m_apiUtilisee==CalendarList)
        {
            QMap<QString,QVariant> listeCal;
            QJsonValue maValeur = monObjet.value("items");
            if (maValeur.isArray())
            {
                QJsonArray test(maValeur.toArray());
                for (int i=0;i<test.count();i++)
                {
                    QJsonObject temp = test.at(i).toObject();
                    listeCal.insert(temp.value("id").toString(),temp.value("summary").toVariant());
                }
            }
            setListeCalendriers(listeCal);
            setApiDemandee(CalendarEvents);
            setCalendrierIdDefaut(m_listeCalendriers.keys().first());
            setCalendrierLibelleDefaut(m_listeCalendriers.values().first().toString());
            return;
        }
        if (m_apiUtilisee==CalendarEvents)
        {
            QMap<QString, QVariant> listeEvents;
            QJsonValue maValeur = monObjet.value("items");
            jsonValeur=maValeur;
            if (maValeur.isArray())
            {
                QJsonArray test(maValeur.toArray());
                for (int i=0;i<test.count();i++)
                {
                    QJsonObject temp = test.at(i).toObject();
                    listeEvents.insert(temp.value("id").toString(),temp.value("summary").toVariant());
                }
            }
            setListeEvenements(listeEvents);
        }
        if (m_apiUtilisee==CalendarGetEvent)
        {
            evenement detailEvenement;
            detailEvenement.calendarId = calendrierIdDefaut();
            detailEvenement.id=monObjet.value("id").toString();
            detailEvenement.status=monObjet.value("status").toString();
            detailEvenement.summary=monObjet.value("summary").toString();
            detailEvenement.description=monObjet.value("description").toString();
            detailEvenement.location=monObjet.value("location").toString();
            if (monObjet.value("start").toObject().value("date").toString()!="")
            {
                QStringList temp =QString(monObjet.value("start").toObject().value("date").toString()).split('-');
                QString annee = temp[0];
                QString mois = temp[1];
                QString jour = temp[2];
                detailEvenement.startDate.setDate(annee.toInt(),mois.toInt(),jour.toInt());
            }
            if (monObjet.value("start").toObject().value("dateTime").toString()!="")
            {
                QStringList liste = QString(monObjet.value("start").toObject().value("dateTime").toString()).split("T");
                QStringList date = QString(liste[0]).split("-");
                QString annee = date[0];
                QString mois = date[1];
                QString jour = date[2];
                detailEvenement.startDateTime.setDate(QDate(annee.toInt(),mois.toInt(),jour.toInt()));
                QStringList time = QString(liste[1]).split(":");
                QString heures = time[0];
                QString minutes = time[1];
                QStringList listesecondes = QString(time[2]).split("Z");
                QString secondes = listesecondes[0];
                detailEvenement.startDateTime.setTime(QTime(heures.toInt(),minutes.toInt(),secondes.toInt()));
            }
            if (monObjet.value("end").toObject().value("date").toString()!="")
            {
                QStringList temp =QString(monObjet.value("end").toObject().value("date").toString()).split('-');
                QString annee = temp[0];
                QString mois = temp[1];
                QString jour = temp[2];
                detailEvenement.endDate.setDate(annee.toInt(),mois.toInt(),jour.toInt());
            }
            if (monObjet.value("end").toObject().value("dateTime").toString()!="")
            {
                QStringList liste = QString(monObjet.value("end").toObject().value("dateTime").toString()).split("T");
                QStringList date = QString(liste[0]).split("-");
                QString annee = date[0];
                QString mois = date[1];
                QString jour = date[2];
                detailEvenement.endDateTime.setDate(QDate(annee.toInt(),mois.toInt(),jour.toInt()));
                QStringList time = QString(liste[1]).split(":");
                QString heures = time[0];
                QString minutes = time[1];
                QStringList listesecondes = QString(time[2]).split("Z");
                QString secondes = listesecondes[0];
                detailEvenement.endDateTime.setTime(QTime(heures.toInt(),minutes.toInt(),secondes.toInt()));
            }
            detailEvenement.startTimeZone=monObjet.value("start").toObject().value("timeZone").toString();
            detailEvenement.endTimeZone=monObjet.value("end").toObject().value("timeZone").toString();
            setDetailEvenement(detailEvenement);
        }
        if (m_apiUtilisee==CalendarPostEvent)
        {
            if (monObjet.value("status").toString() =="confirmed") emit eventEnregistre();
        }
        if (m_apiUtilisee==CalendarMajEvent)
        {
            emit eventMaj();
        }
        if (m_apiUtilisee==CalendarDeleteEvent)
        {
            emit eventDeleted();
        }
        if (m_apiUtilisee==CalendarPosteCalendar)
        {
            emit calendarAjoute();
        }
        if (m_apiUtilisee==CalendarMajCalendar)
        {
            emit calendarMaj();
        }
        if (m_apiUtilisee==CalendarDeleteCalendar)
        {
            emit calendarDeleted();
        }
//        if (m_apiUtilisee==CouleurCalendars)
//        {
            if (monObjet.value("kind").toString()=="calendar#colors")
            {
                QMap<QString, QString> listeRetour;
                QJsonObject valeur = monObjet.value("calendar").toObject();
                for (int i=1;i<=valeur.count();i++)
                {
                    listeRetour.insert(valeur.value(QString::number(i)).toObject().value("background").toString(),valeur.value(QString::number(i)).toObject().value("foreground").toString());
                }
                //            foreach (QString temp, listeRetour.keys())
                //                qDebug()<<temp<<listeRetour.value(temp);
                setListeCouleurs(listeRetour);
            }
        }
//    }
}

void googleCalendar::remplissageListeEvenements(QString calendrierId)
{
    setApiDemandee(CalendarEvents);
    QByteArray temp = QUrl::toPercentEncoding(calendrierId);
    QNetworkRequest *maRequete = new QNetworkRequest(QUrl(urlListeEvenements()+temp+"/events"));

    requeteurCal->setO2(oauthCal);
    requeteurCal->get(*maRequete);
}

O2 *googleCalendar::authenticator()
{
    return oauthCal;
}

googleCalendar::evenement googleCalendar::evenementMemorise()
{
    return m_detailEvenement;
}

void Google::on_pushButton_3_clicked()
{
//    appelAPIPosteCalendrier(ui->ledt_summary->text());
}

void Google::on_pushButton_clicked()
{
//    appelAPIMajCalendrier(monCalendar->calendrierIdDefaut(),ui->ledt_summary->text());
}

void Google::on_pushButton_2_clicked()
{
//    appelAPIEffaceCalendrier(monCalendar->calendrierIdDefaut());
}
