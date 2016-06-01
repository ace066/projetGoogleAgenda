#ifndef FENPRINCIPALE_H
#define FENPRINCIPALE_H

#include <QMainWindow>

namespace Ui {
class fenPrincipale;
}

class fenPrincipale : public QMainWindow
{
    Q_OBJECT

public:
    explicit fenPrincipale(QWidget *parent = 0);
    ~fenPrincipale();

private:
    Ui::fenPrincipale *ui;
};

#endif // FENPRINCIPALE_H
