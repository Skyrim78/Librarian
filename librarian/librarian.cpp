#include "librarian.h"
#include "ui_librarian.h"
#include "organizations.h"
#include "sect_cat.h"
#include "publish.h"
#include "authors.h"
#include "books.h"

librarian::librarian(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::librarian)
{
    ui->setupUi(this);
    ui->dockWidge_db->hide();
    ui->groupBox->hide();

    db = QSqlDatabase::addDatabase("QSQLITE");
    connectDataBase();

    connect(ui->actionSettingDB, SIGNAL(triggered()), this, SLOT(toSettingDataBase()));
    connect(ui->pushButton_open, SIGNAL(clicked()), this, SLOT(openDataBase()));
    connect(ui->pushButton_create, SIGNAL(clicked()), this, SLOT(startCreateDataBase()));
    connect(ui->pushButton_new_make, SIGNAL(clicked()), this, SLOT(createDataBase()));
    //------------------
    connect(ui->actionOrganizations, SIGNAL(triggered()), this, SLOT(toOrganizations()));
    connect(ui->actionSectCat, SIGNAL(triggered()), this, SLOT(toSectCat()));
    connect(ui->actionPub, SIGNAL(triggered()), this, SLOT(toPublish()));
    connect(ui->actionAuth, SIGNAL(triggered()), this, SLOT(toAuthors()));
    connect(ui->actionBooks, SIGNAL(triggered()), this, SLOT(toBooks()));
}

librarian::~librarian()
{
    db.close();
    delete ui;
}

void librarian::connectDataBase(){
    QSettings sett("Skyrim", "lib");
    QString db_path = sett.value("db_path").toString();
    if (db_path.isEmpty()){
        ui->dockWidge_db->setVisible(true);
        ui->lineEdit_now->clear();
        ui->label_status->setText("<span style=\" color:red;\">Disconnect</span>");
    } else {
        ui->lineEdit_now->setText(db_path);
        db.setDatabaseName(db_path);
        db.open();
        if (db.isOpen()){
            if (testDataBase() == true){
                ui->label_status->setText("<span style=\" color:green;\">Connect</span>");
            } else {
                db.close();
                db.removeDatabase(db_path);
                sett.setValue("db_path", " ");
                 ui->label_status->setText("<span style=\" color:red;\">Disconnect</span>");
            }
        } else {
            ui->label_status->setText("<span style=\" color:red;\">Disconnect</span>");
        }
    }
}

void librarian::toSettingDataBase(){
    ui->dockWidge_db->setVisible(true);
}

void librarian::openDataBase(){
    QSettings sett("Skyrim", "lib");
    QString db_path = sett.value("db_path").toString();
    db.close();
    QSqlDatabase::removeDatabase(db_path);

    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Select database"), "/home", tr("Database Files (*.db3)"));
        sett.setValue("db_path", fileName);
        connectDataBase();
}

bool librarian::testDataBase(){
    QStringList listA;
    listA << "authors" << "book_auth" << "books" << "card_read" << "category" << "doc_item" << "docs"
          << "languages" << "organizations" << "publish" << "readers" << "sections" << "sqlite_sequence";
    listA.sort();
    QStringList listB;
    QSqlQuery testA("select sqlite_master.name from sqlite_master", db);
     while (testA.next()){
            listB << testA.value(0).toString();
     }
     listB.sort();
     if (listA == listB){
            return true;
        } else {
            return false;
        }
}

void librarian::startCreateDataBase(){
    ui->groupBox->setVisible(true);
    ui->lineEdit_new_name->setFocus();
}

void librarian::createDataBase(){
    QSettings sett("Skyrim", "lib");
    QString db_path = sett.value("db_path").toString();
    if (!ui->lineEdit_new_name->text().isEmpty()){
        if (!ui->lineEdit_new_org->text().isEmpty()){
            QFile _empty("librarian.db3");
            _empty.copy(QString("%1.db3").arg(ui->lineEdit_new_name->text()));
            if (db.isOpen()){
                db.close();
                db.removeDatabase(db_path);
            }
            sett.setValue("db_path", QString("%1.db3").arg(ui->lineEdit_new_name->text()));
            connectDataBase();
            QSqlQuery org("insert into organizations (name) values (:name)");
            org.bindValue(0, ui->lineEdit_new_org->text());
            org.exec();
            ui->groupBox->hide();
        } else {
            QMessageBox messa;
            messa.setText("Введите название Вашей организации...");
            messa.exec();
        }
    } else {
        QMessageBox messa;
        messa.setText("Введите имя файла для будущей базы данных...");
        messa.exec();
    }
}

void librarian::toOrganizations(){
    org *_o = new org(this);
    _o->show();
}

void librarian::toSectCat(){
    sect_cat *sc = new sect_cat(this);
    sc->show();
}

void librarian::toPublish(){
    pub *p = new pub(this);
    p->show();
}
void librarian::toAuthors(){
    auth *_a = new auth(this);
    _a->show();
}

void librarian::toBooks(){
    books *_b = new books(this);
    _b->show();
}
