#include "librarian.h"
#include "ui_librarian.h"
#include "organizations.h"
#include "sect_cat.h"
#include "publish.h"
#include "authors.h"
#include "books.h"
#include "readers.h"
#include "docs.h"
#include "searchReaders.h"
//-----------------------------
#include "import.h"
#include "search.h"
#include "reports.h"
#include "owner.h"


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
    connect(ui->actionReaders, SIGNAL(triggered()), this, SLOT(toReaders()));
    connect(ui->action_doc_in, SIGNAL(triggered()), this, SLOT(toDocsIn()));
    connect(ui->action_doc_out, SIGNAL(triggered()), this, SLOT(toDocsOut()));
    connect(ui->action_biblio, SIGNAL(triggered()), this, SLOT(toBiblio()));
    //------------------
    connect(ui->actionImport, SIGNAL(triggered()), this, SLOT(toImport()));
    connect(ui->actionSearch, SIGNAL(triggered()), this, SLOT(toSearch()));
    //---------------?
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutProg()));
    connect(ui->actionOwner, SIGNAL(triggered()), this, SLOT(toOwner()));
    //---------reports
    connect(ui->actionReport1, SIGNAL(triggered()), this, SLOT(toReport1()));
    connect(ui->actionReport2, SIGNAL(triggered()), this, SLOT(toReport2()));
    connect(ui->actionReport3, SIGNAL(triggered()), this, SLOT(toReport3()));
    connect(ui->actionReport4, SIGNAL(triggered()), this, SLOT(toReport4()));
    connect(ui->actionReport5, SIGNAL(triggered()), this, SLOT(toReport5()));
    connect(ui->actionReport6, SIGNAL(triggered()), this, SLOT(toReport6()));
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
    listA << "authors" << "book_auth" << "book_item"<< "books" << "card_read" << "category" << "doc_item" << "docs"
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

void librarian::toReaders(){
    readers *_r = new readers(this);
    _r->show();
}

void librarian::toDocsIn(){
    docs *doc_in = new docs(1, this);
    doc_in->show();
}

void librarian::toDocsOut(){
    docs *doc_out = new docs(2, this);
    doc_out->show();
}

void librarian::toBiblio(){
    sReaders *sR = new sReaders(this);
    sR->exec();
}

void librarian::toImport(){
    import *im = new import(this);
    im->exec();
}

void librarian::toSearch(){
    search *s = new search(this);
    s->exec();
}

void librarian::aboutQt(){
    QMessageBox qtAbout;
    qtAbout.aboutQt(this, "About Qt");
}

void librarian::aboutProg(){
    QMessageBox progAbout;
    progAbout.about(this, tr("About librarian"), tr("<p align=\"center\"><b>librarian</b>"
                                                    "<br>version: 0.1beta"
                                                    "<br>date: 2012-10-16"
                                                    "<br>Ev Shevchenko"
                                                    "<br>e-mail: Skyrim78@yandex.ru"));
}

void librarian::toOwner(){
    owner *o = new owner(this);
    o->exec();
}

void librarian::toReport1(){
    reports *rep1 = new reports(0, this);
    rep1->exec();
}

void librarian::toReport2(){
    reports *rep1 = new reports(1, this);
    rep1->exec();
}
void librarian::toReport3(){
    reports *rep1 = new reports(2, this);
    rep1->exec();
}
void librarian::toReport4(){
    reports *rep1 = new reports(3, this);
    rep1->exec();
}
void librarian::toReport5(){
    reports *rep1 = new reports(4, this);
    rep1->exec();
}
void librarian::toReport6(){
    reports *rep1 = new reports(5, this);
    rep1->exec();
}

