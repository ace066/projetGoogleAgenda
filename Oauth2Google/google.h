#ifndef GOOGLE_H
#define GOOGLE_H

#include <QWidget>
#include <QSettings>
#include <QDesktopServices>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCheckBox>
#include <QDate>

#include "o2.h"
#include "o2requestor.h"

//Paramètres de la classe Google
const char GOOGLE_ID[] = "googleClientID";
const char GOOGLE_SECRET[] = "googleClientSecret";
const char GOOGLE_SCOPE[] = "googleScope";
const char GOOGLE_URL_AUTHENTIFICATION[] = "https://accounts.google.com/o/oauth2/v2/auth";
const char GOOGLE_URL_TOKEN[]="https://www.googleapis.com/oauth2/v4/token";
const char GOOGLE_CODE_AUTHORISATION[]= "code";
const char GOOGLE_ACCESS_TOKEN[] = "access token";
const char GOOGLE_REFRESH_TOKEN[]="refresh token";
const char GOOGLE_REFRESH_TOKEN_URL[]="https://www.googleapis.com/oauth2/v4/token";

//Paramètres de la classe googleCalendar
//URL de connexion et authentification aux agendas
const char CALENDAR_AUTH_SCOPE[] = "https://www.googleapis.com/auth/calendar";
//URL de récupération de la liste des agendas du compte
const char CALENDAR_URLCALENDAR_LIST[] = "https://www.googleapis.com/calendar/v3/users/me/calendarList/";
const char CALENDAR_CAL_ID_DEFAUT[] = "Id du calendrier par défaut";
const char CALENDAR_CAL_LIBELLE_DEFAUT[] = "Libellé du calendrier par défaut";
const char CALENDAR_URLEVENTS_LIST[] = "https://www.googleapis.com/calendar/v3/calendars/";

class googleCalendar:public QWidget
{
    Q_OBJECT

public:
    explicit googleCalendar(QWidget *parent = 0);
    ~googleCalendar();

    struct evenement{
        QString calendarId;
        QString id;
        QString status;
        QString summary;
        QString description;
        QString location;
        QString creatorId;
        QString creatorEmail;
        QString creatorDisplayName;
        QDate startDate;
        QDateTime startDateTime;
        QString startTimeZone;
        QDate endDate;
        QDateTime endDateTime;
        QString endTimeZone;
    };

    //Propriété affectant le paramètre CALENDAR_AUTH_SCOPE
    Q_PROPERTY(QString urlListeAgendas READ urlListeAgendas WRITE setUrlListeAgendas NOTIFY urlListeAgendasChanged)
    QString urlListeAgendas();
    void setUrlListeAgendas(QString valeur);

    //Propriété affectant le paramètre CALENDAR_URLEVENTS_LIST
    Q_PROPERTY(QString urlListeEvenements READ urlListeEvenements WRITE setUrlListeEvenements NOTIFY urlListeEvenementsChanged)
    QString urlListeEvenements();
    void setUrlListeEvenements(QString valeur);

    //Propriété pour définir l'id du calendrier par défaut
    Q_PROPERTY(QString calendrierIdDefaut READ calendrierIdDefaut WRITE setCalendrierIdDefaut NOTIFY calendrierIdDefautChanged)
    QString calendrierIdDefaut();
    void setCalendrierIdDefaut(QString valeur);

    //Propriété pour définir le libellé du calendrier par défaut
    Q_PROPERTY(QString calendrierLibelleDefaut READ calendrierLibelleDefaut WRITE setCalendrierLibelleDefaut NOTIFY calendrierLibelleDefautChanged)
    QString calendrierLibelleDefaut();
    void setCalendrierLibelleDefaut(QString valeur);

    //Propriété pour définir l'ID de l'évnèment sélectioné
    Q_PROPERTY(QString eventIdSelect READ eventIdSelect WRITE setEventIdSelect NOTIFY eventIdSelectChanged)
    QString eventIdSelect();
    void setEventIdSelect(QString valeur);

    //Propriété pour définir le libellé de l'évènement selectionné
    Q_PROPERTY(QString eventLibelleSelect READ eventLibelleSelect WRITE setEventLibelleSelect NOTIFY eventLibelleSelectChanged)
    QString eventLibelleSelect();
    void setEventLibelleSelect(QString valeur);

    //Implémente et renvoie la liste des calendriers avec leur ID et leur libellé
    QMap<QString, QVariant> listeCalendriers();
    void setListeCalendriers(QMap<QString, QVariant> liste);

    //Implémente et renvoie la liste des évènements d'un calendrier
    QMap<QString, QVariant> listeEvenements();
    void setListeEvenements(QMap<QString, QVariant> liste);

    //Implémente et renvoie le détail de l'évènement selectionné
    void detailEvenement();
    void setDetailEvenement(evenement liste);

    //Renvoie la iste des couleurs contenues
    QMap<QString, QString> listeCouleurs();
    void setListeCouleurs(QMap<QString, QString> liste);

    enum listAppelApi{CalendarList, CalendarEvents, CalendarGetEvent, CalendarPostEvent, CalendarMajEvent, CalendarDeleteEvent, CalendarPosteCalendar, CalendarMajCalendar, CalendarDeleteCalendar, CouleurCalendars};
    void setO2(O2 *oauth);
    void setApiDemandee(listAppelApi valeur);
    QString calendrierId(QString libelleCalendrier);
    listAppelApi apiAppelee();
    void effaceListeEvenements();
    void posteEvent(QString calendrierID, QString description, QString summary, bool journeeEntiere, QDateTime debut, QDateTime fin);
    void majevent(QString calendrierID, QString eventId, QString description, QString summary, bool journeeEntiere, QDateTime debut, QDateTime fin);
    void effaceEvent(QString calendrierId, QString eventId);
    void ajouteCalendrier(QString titre);
    void majCalendrier(QString calendrierId, QString nouveauTitre);
    void effaceCalendrier(QString calendrierId);
    void remplissageListeEvenements(QString calendrierId);
    QSettings *mesParametres;
    O2 *authenticator();
    evenement evenementMemorise();
    QJsonValue jsonValeur;
    O2Requestor *requeteurCal;

public slots :
    void onFinished(int id, QNetworkReply::NetworkError erreur, QByteArray data);

signals:
    void urlListeAgendasChanged();
    void listeCalendrierChanged();
    void calendrierIdDefautChanged();
    void calendrierLibelleDefautChanged();
    void listeEvenementsChanged();
    void detailEvenementChanged();
    void urlListeEvenementsChanged();
    void eventIdSelectChanged();
    void eventLibelleSelectChanged();
    void listeCouleursChanged();

    void eventEnregistre();
    void eventMaj();
    void eventDeleted();
    void calendarAjoute();
    void calendarMaj();
    void calendarDeleted();

private:

    QMap<QString, QVariant> m_listeCalendriers;
    QMap<QString, QVariant> m_listeEvenements;
    QMap<QString, QString> m_listeCouleurs;     //le premier QString représente le background, le second le foreground
    evenement m_detailEvenement;
    O2 *oauthCal;    
    listAppelApi m_apiUtilisee;
    QString m_eventIdDefaut;
    QString m_eventLibelleDefaut;        
};

//namespace Ui {
//class Google;
//}

class Google : public QWidget
{
    Q_OBJECT

public:
    explicit Google(QWidget *parent = 0);
    ~Google();

    void connexion();
    void deconnexion();

    //Propriété affectant le ID client
    Q_PROPERTY(QString clientID READ clientID WRITE setClientID NOTIFY clientIDChanged)
    void setClientID(QString valeur);
    QString clientID();

    //Propriété affectant le secret de l'application
    Q_PROPERTY(QString clientSecret READ clientSecret WRITE setClientSecret NOTIFY clientSecretChanged)
    void setClientSecret(QString valeur);
    QString clientSecret();

    //Propriété affectant le scope
    Q_PROPERTY(QString scope READ scope WRITE setScope NOTIFY scopeChanged)
    void setScope(QString valeur);
    QString scope();

    void appelAPICalendarListe();
    void appelAPIEffaceEvent(QString calendrierId, QString eventId);
    void appelAPIMajEvent(QString calendrierID, QString eventId, QString description, QString summary, bool journeeEntiere, QDateTime debut, QDateTime fin);
    void appelAPIPosteEvent(QString calendrierID, QString description, QString summary, bool journeeEntiere, QDateTime debut, QDateTime fin);
    void appelAPIPosteCalendrier(QString titre);
    void appelAPIMajCalendrier(QString calendrierId, QString nouveauTitre);
    void appelAPIEffaceCalendrier(QString calendrierId);

    googleCalendar *monCalendar;
    QMap<QString, QVariant> listeCalendrier();
    void setListeCalendrier(QMap<QString, QVariant> liste);

    QMap<QString, QVariant> listeEvenements();
    void setListeEvenements(QMap<QString, QVariant> liste);
    googleCalendar::evenement evenementSelectionne();    

    void ajouteCheckBox(QCheckBox *boite);
    void retireCheckBox (QString nomObjet);
    QList<QCheckBox*>  listeCheckBox();

    void ajouteEvenementListe(googleCalendar::evenement temp);
    QList<googleCalendar::evenement> listeCompleteEvenements();

    bool remplissageListeCompleteEvents;
    QList<googleCalendar::evenement> listeFiltreeJour(QList<googleCalendar::evenement> listeComplete, QDate dateJour);
    QList<googleCalendar::evenement> listeTriee(QList<googleCalendar::evenement> listeFiltree);

    QList<googleCalendar::evenement> listeFiltreeJourSelectionne(QList<googleCalendar::evenement> listeComplete, QDate dateJourSelectionne);
    O2 *oauth;

private slots:
    void onOpenBrowser(QUrl adresse);
    void onLinkedSuccess();
    void on_btn_connexion_clicked();

    void onClientIDChanged();
    void onClientSecretChanged();
    void onUrlAuthChanged();
    void onScopeChanged();
    void onTokenUrlChanged();
    void onCodeChanged();
    void onAccessTokenChanged();
    void onRefreshTokenChanged();
    void onRefreshTokenUrlChanged();
    void onListeCalendriersChanged();
    void onSelectionCalendrierChanged(QString valeur);
    void onListeEvenementsChanged();
    void onSelectionEvenementChanged(QString valeur);
    void onDetailEvenementChanged();
    void onListeCouleursChanged();

    void onEventEnregistre();
    void onEventMaj();
    void onEventDeleted();
    void onCalendarAjoute();
    void onCalendarMaj();
    void onCalendarDeleted();

    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

signals:
    void clientIDChanged();
    void clientSecretChanged();
    void scopeChanged();
    void eventEnregistre();
    void detailEventChanged();
    void eventMaj();
    void eventDeleted();
    void calendarAjoute();
    void calendarMaj();
    void calendarDeleted();
    void listeCalendrierChanged();
    void listeEvenementsChanged();
    void listeCouleursChanged();
    void connexionEtablie();
    void listeCompleteEvenementsChanged();

private:
//    Ui::Google *ui;

    QSettings *mesParametres;
    O2Requestor *requeteur;    
    QMap<QString, QVariant> m_listeCalendrier;
    QMap<QString, QVariant> m_listeEvenements;
    googleCalendar::evenement evenement;
    QList<QCheckBox*> m_listeCheckbox ;
    QList<googleCalendar::evenement> m_listeCompleteEvenements;

    void remplissageOauth(QString code, QString token, QString clientID, QString clientSecret, QString scope, QString urlAuth, QString urlToken, QString urlRefreshToken, QString refreshToken);
};

#endif // GOOGLE_H
