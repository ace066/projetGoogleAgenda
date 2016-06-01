#ifndef CALENDRIER_H
#define CALENDRIER_H

#include <QWidget>
#include "Oauth2Google/google.h"
#include <QCheckBox>
#include <QPushButton>

namespace Ui {
class calendrier;
}

class calendrier : public QWidget
{
    Q_OBJECT

public:
    explicit calendrier(QWidget *parent = 0);
    ~calendrier();

private:
    Ui::calendrier *ui;
    Google *monGoogle;
    QList<googleCalendar::evenement> listeEvenementsAffiches();
    void setListeEvenementsAffiches(QList<googleCalendar::evenement> liste);
    QList<googleCalendar::evenement>  m_listeEvenementsAffiches;

signals:
    listeEvenementsAffichesChanged();

private slots:
    void onConnexionEtablie();
    void on_btn_connexion_clicked();
    void onListeCalendrierChanged();
    void onListeCompleteEventsChanged();
    void onCheckboxCalendrierClicked();
    void onListeEvenementsAffichesChanged();
    void onListeCouleursChanged();

    void on_pushButton_clicked();
};

#endif // CALENDRIER_H
