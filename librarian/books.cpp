#include "books.h"
#include "bookCard.h"

books::books(QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);
    ui.groupBox_column->hide();

    readSetting();
    readBook();

    connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(close()));

    connect(ui.checkBox_isbn, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_title, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_auth, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_sect, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_cat, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_pub, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_year, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_place, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_in, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_out, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_read, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.checkBox_dost, SIGNAL(clicked()), this, SLOT(selectColumns()));
    connect(ui.toolButton_col, SIGNAL(clicked(bool)), ui.groupBox_column, SLOT(setVisible(bool)));

    connect(ui.actionAdd, SIGNAL(triggered()), this, SLOT(addBook()));
    connect(ui.tableWidget_book, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editBook()));
}

void books::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("book");
        int _w =  node.item(0).attributes().namedItem("width").nodeValue().toInt();
        int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
        int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
        int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
        setGeometry(_x, _y, _w, _h);
        ui.checkBox_isbn->setChecked(node.item(0).attributes().namedItem("isbn").nodeValue().toInt());
        ui.checkBox_title->setChecked(node.item(0).attributes().namedItem("title").nodeValue().toInt());
        ui.checkBox_sect->setChecked(node.item(0).attributes().namedItem("sect").nodeValue().toInt());
        ui.checkBox_cat->setChecked(node.item(0).attributes().namedItem("cat").nodeValue().toInt());
        ui.checkBox_pub->setChecked(node.item(0).attributes().namedItem("pub").nodeValue().toInt());
        ui.checkBox_year->setChecked(node.item(0).attributes().namedItem("year").nodeValue().toInt());
        ui.checkBox_place->setChecked(node.item(0).attributes().namedItem("place").nodeValue().toInt());
        ui.checkBox_in->setChecked(node.item(0).attributes().namedItem("in").nodeValue().toInt());
        ui.checkBox_out->setChecked(node.item(0).attributes().namedItem("out").nodeValue().toInt());
        ui.checkBox_read->setChecked(node.item(0).attributes().namedItem("read").nodeValue().toInt());
        ui.checkBox_dost->setChecked(node.item(0).attributes().namedItem("dost").nodeValue().toInt());
        ui.checkBox_auth->setChecked(node.item(0).attributes().namedItem("auth").nodeValue().toInt());
        setGeometry(_x, _y, _w, _h);
    }
    file.close();
}

void books::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();

    QDomNodeList node = doc.elementsByTagName("book");

    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));
    node.item(0).attributes().namedItem("isbn").setNodeValue(QString("%1").arg(ui.checkBox_isbn->isChecked()));
    node.item(0).attributes().namedItem("title").setNodeValue(QString("%1").arg(ui.checkBox_title->isChecked()));
    node.item(0).attributes().namedItem("sect").setNodeValue(QString("%1").arg(ui.checkBox_sect->isChecked()));
    node.item(0).attributes().namedItem("cat").setNodeValue(QString("%1").arg(ui.checkBox_cat->isChecked()));
    node.item(0).attributes().namedItem("pub").setNodeValue(QString("%1").arg(ui.checkBox_pub->isChecked()));
    node.item(0).attributes().namedItem("year").setNodeValue(QString("%1").arg(ui.checkBox_year->isChecked()));
    node.item(0).attributes().namedItem("place").setNodeValue(QString("%1").arg(ui.checkBox_place->isChecked()));
    node.item(0).attributes().namedItem("in").setNodeValue(QString("%1").arg(ui.checkBox_in->isChecked()));
    node.item(0).attributes().namedItem("out").setNodeValue(QString("%1").arg(ui.checkBox_out->isChecked()));
    node.item(0).attributes().namedItem("read").setNodeValue(QString("%1").arg(ui.checkBox_read->isChecked()));
    node.item(0).attributes().namedItem("dost").setNodeValue(QString("%1").arg(ui.checkBox_dost->isChecked()));
    node.item(0).attributes().namedItem("auth").setNodeValue(QString("%1").arg(ui.checkBox_auth->isChecked()));

    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void books::closeEvent(QCloseEvent *){
    writeSetting();
}

void books::selectColumns(){
    ui.tableWidget_book->setColumnHidden(0, true);
    ui.tableWidget_book->setColumnHidden(1, !ui.checkBox_isbn->isChecked());
    ui.tableWidget_book->setColumnHidden(2, !ui.checkBox_title->isChecked());
    ui.tableWidget_book->setColumnHidden(3, !ui.checkBox_auth->isChecked());
    ui.tableWidget_book->setColumnHidden(4, !ui.checkBox_sect->isChecked());
    ui.tableWidget_book->setColumnHidden(5, !ui.checkBox_cat->isChecked());
    ui.tableWidget_book->setColumnHidden(6, !ui.checkBox_pub->isChecked());
    ui.tableWidget_book->setColumnHidden(7, !ui.checkBox_year->isChecked());
    ui.tableWidget_book->setColumnHidden(8, !ui.checkBox_place->isChecked());
    ui.tableWidget_book->setColumnHidden(9, !ui.checkBox_in->isChecked());
    ui.tableWidget_book->setColumnHidden(10, !ui.checkBox_out->isChecked());
    ui.tableWidget_book->setColumnHidden(11, !ui.checkBox_read->isChecked());
    ui.tableWidget_book->setColumnHidden(12, !ui.checkBox_dost->isChecked());
    ui.tableWidget_book->resizeColumnsToContents();
    ui.tableWidget_book->horizontalHeader()->setStretchLastSection(true);
}

void books::readBook(){
    for (int a = ui.tableWidget_book->rowCount(); a >= 0; a--){
        ui.tableWidget_book->removeRow(a);
    }
    QSqlQuery _b("select books.id, books.isbn, books.title, sections.name, category.name, publish.name, "
                   "books.year "
                   "from books, sections, category, publish "
                   "where sections.id = books.sect and category.id = books.cat and publish.id = books.pub ");
    int row = 0;
    while (_b.next()){
        ui.tableWidget_book->insertRow(row);
        for (int col = 0; col < 3; col++){
            QTableWidgetItem *item = new QTableWidgetItem(_b.value(col).toString());
            ui.tableWidget_book->setItem(row, col, item);
        }
        QSqlQuery _a(QString("select authors.name_k from authors, book_auth "
                               "where book_auth.auth = authors.id and book_auth.book = %1").arg(_b.value(0).toString()));
        QString auth;
        while (_a.next()){
            auth.append(QString("%1 ").arg(_a.value(0).toString()));
        }
        QTableWidgetItem *itemAuth = new QTableWidgetItem(auth);
        ui.tableWidget_book->setItem(row, 3, itemAuth);

        for (int col = 4; col < 8; col++){
            QTableWidgetItem *item = new QTableWidgetItem(_b.value(col-1).toString());
            ui.tableWidget_book->setItem(row, col, item);
        }
        row++;
    }
    selectColumns();
}

void books::addBook(){    
    QSqlQuery _n("insert into books (note) values (:note)");
    _n.bindValue(0, " ");
    _n.exec();
    //qDebug() << _n.lastInsertId().toInt();
    QList<int> list;
    list << _n.lastInsertId().toInt();
    b_card *bc = new b_card(list, 0, true, this);
    bc->exec();
    readBook();
}

void books::editBook(){
    QList<int> list;
    for (int r = 0; r < ui.tableWidget_book->rowCount(); r++){
        list << ui.tableWidget_book->item(r, 0)->text().toInt();
    }
    b_card *bc = new b_card(list, ui.tableWidget_book->currentRow(), false, this);
    bc->exec();
    readBook();

}
