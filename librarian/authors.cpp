#include "authors.h"
#include "bookCard.h"

auth::auth(QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);
    ui.groupBox_card->hide();
    ui.tableWidget_book->setColumnHidden(0, true);
    ui.tableWidget_authors->setColumnHidden(0, true);

    readSetting();
    updateAuth();

    connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(close()));

    connect(ui.actionAdd, SIGNAL(triggered()), this, SLOT(addAuth()));
    connect(ui.tableWidget_authors, SIGNAL(clicked(QModelIndex)), this, SLOT(editAuth()));
    connect(ui.pushButton_save, SIGNAL(clicked()), this, SLOT(saveAuth()));
    connect(ui.pushButton_del, SIGNAL(clicked()), this, SLOT(deleteAuth()));
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(closeAuth()));
    //----
    connect(ui.lineEdit_fam, SIGNAL(textEdited(QString)), this, SLOT(makeNameK()));
    connect(ui.lineEdit_ima, SIGNAL(textEdited(QString)), this, SLOT(makeNameK()));
    connect(ui.lineEdit_otc, SIGNAL(textEdited(QString)), this, SLOT(makeNameK()));
    //----
    connect(ui.tableWidget_book, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(toBook()));

}

void auth::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("auth");
        int _w = node.item(0).attributes().namedItem("width").nodeValue().toInt();
        int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
        int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
        int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
        setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void auth::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();

    QDomNodeList node = doc.elementsByTagName("auth");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1") .arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));

    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void auth::closeEvent(QCloseEvent *){
    writeSetting();
}

void auth::updateAuth(){
    for (int a = ui.tableWidget_authors->rowCount(); a >= 0; a--){
        ui.tableWidget_authors->removeRow(a);
    }
    QSqlQuery q("select authors.id, authors.fam, authors.ima, authors.otc "
                "from authors order by authors.fam");
    int row = 0;
    while (q.next()){
        ui.tableWidget_authors->insertRow(row);
        for (int col = 0; col < 4; col++){
            QTableWidgetItem *item = new QTableWidgetItem(q.value(col).toString());
            ui.tableWidget_authors->setItem(row, col, item);
        }
        row++;
    }
    ui.tableWidget_authors->resizeColumnsToContents();
    ui.tableWidget_authors->horizontalHeader()->setStretchLastSection(true);
}

void auth::addAuth(){
    id = 0;
    ui.groupBox_card->setVisible(true);
    ui.lineEdit_fam->clear();
    ui.lineEdit_ima->clear();
    ui.lineEdit_otc->clear();
    ui.lineEdit_k->clear();
    ui.pushButton_del->setEnabled(false);
    for (int a = ui.tableWidget_book->rowCount(); a >= 0; a--){
        ui.tableWidget_book->removeRow(a);
    }
    ui.lineEdit_fam->setFocus();
}

void auth::editAuth(){
    ui.groupBox_card->setVisible(true);
    int cRow = ui.tableWidget_authors->currentRow();
    id = ui.tableWidget_authors->item(cRow, 0)->text().toInt();
    QSqlQuery q(QString("select authors.fam, authors.ima, authors.otc, authors.name_k "
                "from authors where authors.id = \'%1\'").arg(id));
    q.next();
    ui.lineEdit_fam->setText(q.value(0).toString());
    ui.lineEdit_ima->setText(q.value(1).toString());
    ui.lineEdit_otc->setText(q.value(2).toString());
    ui.lineEdit_k->setText(q.value(3).toString());

    for (int a = ui.tableWidget_book->rowCount(); a >= 0; a--){
        ui.tableWidget_book->removeRow(a);
    }
    QSqlQuery _b(QString("select books.id, books.isbn, books.title, publish.name, books.year "
                         "from books, publish, book_auth "
                         "where books.pub = publish.id and book_auth.book = books.id "
                         "and book_auth.auth = \'%1\' ").arg(id));
    int row = 0;
    while (_b.next()){
        ui.tableWidget_book->insertRow(row);
        for (int col = 0; col < 5; col++){
            QTableWidgetItem *item = new QTableWidgetItem(_b.value(col).toString());
            ui.tableWidget_book->setItem(row, col, item);
        }
        row++;
    }
    ui.tableWidget_book->resizeColumnsToContents();
    ui.tableWidget_book->horizontalHeader()->setStretchLastSection(true);

    if (ui.tableWidget_book->rowCount() == 0){
        ui.pushButton_del->setEnabled(true);
    } else {
        ui.pushButton_del->setEnabled(false);
    }
}

void auth::saveAuth(){

    if (!ui.lineEdit_fam->text().isEmpty()){
        if (!ui.lineEdit_k->text().isEmpty()){
            if (id == 0){
                QSqlQuery _a("insert into authors (fam, ima, otc, name_k) "
                             "values (:fam, :ima, :otc, :name_k)");
                _a.bindValue(0, ui.lineEdit_fam->text());
                _a.bindValue(1, ui.lineEdit_ima->text());
                _a.bindValue(2, ui.lineEdit_otc->text());
                _a.bindValue(3, ui.lineEdit_k->text());
                _a.exec();
                addAuth();
            } else if (id > 0){
                QSqlQuery _u(QString("update authors set fam = \'%1\', ima = \'%2\', otc = \'%3\', "
                                     "name_k = \'%4\' where authors.id = \'%5\' ")
                             .arg(ui.lineEdit_fam->text())
                             .arg(ui.lineEdit_ima->text())
                             .arg(ui.lineEdit_otc->text())
                             .arg(ui.lineEdit_k->text())
                             .arg(id));
                _u.exec();
            }
        } else {
            QMessageBox messa;
            messa.setText("Укажите краткое имя автора...<br><i>(Например: Пушкин А.С.)</i>");
            messa.exec();
        }
    } else {
        QMessageBox messa;
        messa.setText("Укажите фамилию автора...");
        messa.exec();
    }
    updateAuth();
}

void auth::deleteAuth(){
    QSqlQuery _d(QString("delete from authors where authors.id = \'%1\' ").arg(id));
    _d.exec();
    updateAuth();
    closeAuth();
}

void auth::closeAuth(){
    ui.groupBox_card->hide();
}

void auth::makeNameK(){
    QString ima = ui.lineEdit_ima->text();
    QString otc = ui.lineEdit_otc->text();

    QString name_k = ui.lineEdit_fam->text();

    if (!ima.isEmpty()){
        name_k.append(QString(" %1.").arg(ima.left(1)));
    }
    if (!otc.isEmpty()){
        name_k.append(QString(" %1.").arg(otc.left(1)));
    }

    ui.lineEdit_k->setText(name_k);
}

void auth::toBook(){
    QList<int> list;
    for (int row = 0; row < ui.tableWidget_book->rowCount(); row++){
        list << ui.tableWidget_book->item(row, 0)->text().toInt();
    }
    b_card *bc = new b_card(list, ui.tableWidget_book->currentRow(), false, this);
    bc->exec();
}
