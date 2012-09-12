#ifndef LIBRARIAN_H
#define LIBRARIAN_H

#include <QMainWindow>
#include <QtSql/QtSql>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class librarian;
}

class librarian : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit librarian(QWidget *parent = 0);
    ~librarian();

    QSqlDatabase db;
    
private:
    Ui::librarian *ui;

public slots:
    void connectDataBase();

    void toSettingDataBase();
    void openDataBase();
    bool testDataBase();
    void startCreateDataBase();
    void createDataBase();
    //----------------
    void toOrganizations();
    void toSectCat();
    void toPublish();
    void toAuthors();
    void toBooks();
};

#endif // LIBRARIAN_H
