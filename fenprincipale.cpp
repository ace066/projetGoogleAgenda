#include "fenprincipale.h"
#include "ui_fenprincipale.h"

fenPrincipale::fenPrincipale(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::fenPrincipale)
{
    ui->setupUi(this);
}

fenPrincipale::~fenPrincipale()
{
    delete ui;
}
