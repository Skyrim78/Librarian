#include "bookCard.h"
#include <QDesktopServices>

b_card::b_card(QList<int> _list, int _item, bool _card, QWidget *parent):QDialog(parent){
    ui.setupUi(this);
    list = _list;
    item = _item;
    card = _card;
    readSetting();
    ui.tableWidget_auth->setColumnHidden(0, true);
    ui.tableWidget_auth->setColumnHidden(1, true);

    QSqlQuery _s("select sections.name from sections ");
    while (_s.next()){
        ui.comboBox_sect->addItem(_s.value(0).toString());
    }
    QSqlQuery _p("select publish.name from publish ");
    while (_p.next()){
        ui.comboBox_pub->addItem(_p.value(0).toString());
    }
    QSqlQuery _a("select authors.name_k from authors ");
    while (_a.next()){
        ui.comboBox_auth->addItem(_a.value(0).toString());
    }
    QSqlQuery _c("select category.name from category ");
    while (_c.next()){
        ui.comboBox_cat->addItem(_c.value(0).toString());
    }
    QSqlQuery _l("select languages.name from languages ");
    while (_l.next()){
        ui.comboBox_lang->addItem(_l.value(0).toString());
    }
    updateCard();

    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(closeCard()));

    connect(ui.pushButton_toFirst, SIGNAL(clicked()), this, SLOT(toFirst()));
    connect(ui.pushButton_toPrev, SIGNAL(clicked()), this, SLOT(toPrev()));
    connect(ui.pushButton_toNext, SIGNAL(clicked()), this, SLOT(toNext()));
    connect(ui.pushButton_toLast, SIGNAL(clicked()), this, SLOT(toLast()));

    connect(ui.comboBox_sect, SIGNAL(currentIndexChanged(int)), this, SLOT(selectSections()));
    connect(ui.toolButton_auth_add, SIGNAL(clicked()), this, SLOT(addAuthors()));
    connect(ui.toolButton_auth_del, SIGNAL(clicked()), this, SLOT(delAuthors()));

    connect(ui.toolButton_photo_add, SIGNAL(clicked()), this, SLOT(addPhoto()));
    connect(ui.toolButton_photo_del, SIGNAL(clicked()), this, SLOT(delPhoto()));

    connect(ui.toolButton_file_set, SIGNAL(clicked()), this, SLOT(addEBook()));
    connect(ui.toolButton_file_del, SIGNAL(clicked()), this, SLOT(delEBook()));
    connect(ui.pushButton_file_open, SIGNAL(clicked()), this, SLOT(openBook()));

    connect(ui.pushButton_save, SIGNAL(clicked()), this, SLOT(saveCard()));
}

void b_card::readSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QDomDocument doc;
    if (file.open(QIODevice::ReadOnly)){
        doc.setContent(&file);
        QDomNodeList node = doc.elementsByTagName("bcard");
        int _w = node.item(0).attributes().namedItem("width").nodeValue().toInt();
        int _h = node.item(0).attributes().namedItem("height").nodeValue().toInt();
        int _x = node.item(0).attributes().namedItem("x").nodeValue().toInt();
        int _y = node.item(0).attributes().namedItem("y").nodeValue().toInt();
        setGeometry(_x, _y, _w, _h);
    }
    file.close();
    QFile lang(QString("%1/lang.txt").arg(sett.value("path").toString()));
     lang.open(QFile::ReadOnly);
     QTextStream sp(&lang);
     while (!sp.atEnd()){
         QString line = sp.readLine();
         QStringList fieldsA = line.split('/n');
         ui.comboBox_lang->addItems(fieldsA);
     }
}

void b_card::writeSetting(){
    QSettings sett("Skyrim", "lib");
    QString path(QString("%1/setting.xml").arg(sett.value("path").toString()));
    QFile file(path);
    QTextStream in(&file);
    QTextStream out(&file);

    QDomDocument doc;
    file.open(QIODevice::ReadOnly);
    doc.setContent(out.readAll());
    file.close();

    QDomNodeList node = doc.elementsByTagName("bcard");
    node.item(0).attributes().namedItem("width").setNodeValue(QString("%1").arg(geometry().size().width()));
    node.item(0).attributes().namedItem("height").setNodeValue(QString("%1").arg(geometry().size().height()));
    node.item(0).attributes().namedItem("x").setNodeValue(QString("%1").arg(geometry().x()));
    node.item(0).attributes().namedItem("y").setNodeValue(QString("%1").arg(geometry().y()));
    file.open(QIODevice::WriteOnly);
    doc.save(out, 4);
    file.close();
}

void b_card::closeEvent(QCloseEvent *){
    writeSetting();
}

void b_card::updateCard(){
    for (int a = ui.tableWidget_auth->rowCount(); a >= 0; a--){
        ui.tableWidget_auth->removeRow(a);
    }    
    if (card == true){
        ui.lineEdit_title->clear();
        ui.lineEdit_isbn->clear();
        ui.comboBox_auth->setCurrentIndex(-1);
        ui.comboBox_cat->setCurrentIndex(-1);
        ui.comboBox_pub->setCurrentIndex(-1);
        ui.comboBox_sect->setCurrentIndex(-1);
        ui.comboBox_lang->setCurrentIndex(-1);
        ui.spinBox_year->setValue(QDate::currentDate().year());
        ui.lineEdit_note->clear();
        ui.lineEdit_file->clear();
        ui.label_photo->clear();
        ui.lineEdit_file_photo->clear();
        ui.pushButton_del->setEnabled(false);
    } else if (card == false){
        QSqlQuery book(QString("select books.title, books.isbn, sections.name, category.name, publish.name, "
                               "books.year, languages.name, books.note, books.photo, books.ebook "
                               "from books, sections, category, publish, languages "
                               "where publish.id = books.pub and sections.id = books.sect and "
                               "category.id = books.cat and languages.id = books.lang "
                               "and books.id = \'%1\'").arg(list.at(item)));
        book.next();
        ui.lineEdit_title->setText(book.value(0).toString());
        ui.lineEdit_isbn->setText(book.value(1).toString());
        ui.comboBox_sect->setCurrentIndex(ui.comboBox_sect->findText(book.value(2).toString()));
        ui.comboBox_cat->setCurrentIndex(ui.comboBox_cat->findText(book.value(3).toString()));
        ui.comboBox_pub->setCurrentIndex(ui.comboBox_pub->findText(book.value(4).toString()));
        ui.spinBox_year->setValue(book.value(5).toInt());        
        ui.comboBox_lang->setCurrentIndex(ui.comboBox_lang->findText(book.value(6).toString()));
        ui.lineEdit_note->setText(book.value(7).toString());
        ui.lineEdit_file_photo->setText(book.value(8).toString());
        ui.lineEdit_file->setText(book.value(9).toString());

        if (!ui.lineEdit_file_photo->text().isEmpty()){
            QPixmap pix(ui.lineEdit_file_photo->text());
            QPixmap _p(pix.scaled(160, 250, Qt::KeepAspectRatio, Qt::FastTransformation));
            ui.label_photo->setPixmap(_p);
        }

        //authors
        updateAuthors(list.at(item));
        //info identifiers
        updateIdentifiers();
    }

    if (item == 0){
        ui.pushButton_toPrev->setEnabled(false);
        ui.pushButton_toFirst->setEnabled(false);
    } else {
        ui.pushButton_toPrev->setEnabled(true);
        ui.pushButton_toFirst->setEnabled(true);
    }
    if (item == list.size()-1){
        ui.pushButton_toNext->setEnabled(false);
        ui.pushButton_toLast->setEnabled(false);
    } else {
        ui.pushButton_toNext->setEnabled(true);
        ui.pushButton_toLast->setEnabled(true);
    }
    if (list.size() == 0){
        ui.pushButton_toPrev->setEnabled(false);
        ui.pushButton_toFirst->setEnabled(false);
        ui.pushButton_toNext->setEnabled(false);
        ui.pushButton_toLast->setEnabled(false);
    }
}

void b_card::updateAuthors(int id_book){
    for (int a = ui.tableWidget_auth->rowCount(); a >= 0; a--){
        ui.tableWidget_auth->removeRow(a);
    }
    QSqlQuery auth(QString("select book_auth.id, authors.id, authors.name_k from authors, book_auth "
                           "where book_auth.auth = authors.id and book_auth.book = \'%1\' ")
                   .arg(id_book));
    int row = 0;
    while (auth.next()){
        ui.tableWidget_auth->insertRow(row);
        for (int col = 0; col < 3; col++){
            QTableWidgetItem *_i = new QTableWidgetItem(auth.value(col).toString());
            ui.tableWidget_auth->setItem(row, col, _i);
        }
        row++;
    }
    ui.comboBox_auth->setCurrentIndex(-1);
}

void b_card::toFirst(){
    item = 0;
    updateCard();
}

void b_card::toPrev(){
    item = item - 1;
    updateCard();
}

void b_card::toNext(){
    item = item + 1;
    updateCard();
}

void b_card::toLast(){
    item = list.size() - 1;
    updateCard();
}

void b_card::selectSections(){
    QSqlQuery _s(QString("select sections.id from sections where sections.name = \'%1\'")
                 .arg(ui.comboBox_sect->currentText()));
    _s.next();
    ui.comboBox_cat->clear();
    QSqlQuery _c(QString("select category.name from category where category.sect = \'%1\'")
                 .arg(_s.value(0).toInt()));
    while (_c.next()){
        ui.comboBox_cat->addItem(_c.value(0).toString());
    }
    ui.comboBox_cat->setCurrentIndex(-1);
}

void b_card::addAuthors(){
    int res = 0;
    for (int a = 0; a < ui.tableWidget_auth->rowCount(); a++){
        if (ui.tableWidget_auth->item(a, 2)->text() == ui.comboBox_auth->currentText()){
            res++;
        }
    }
    if (res > 0){
        QMessageBox messa;
        messa.setText("Этот автор уже добавлен...");
        messa.exec();

    } else if (res == 0){
        QSqlQuery _a(QString("select authors.id from authors where authors.name_k = \'%1\'")
                     .arg(ui.comboBox_auth->currentText()));
        _a.next();

        QSqlQuery inAuth("insert into book_auth (book, auth) values (:book, :auth)");
        inAuth.bindValue(0, list.at(item));
        inAuth.bindValue(1, _a.value(0).toString());
        inAuth.exec();
        updateAuthors(list.at(item));
    }
}

void b_card::delAuthors(){
    if (ui.tableWidget_auth->rowCount() > 0){
        int id = ui.tableWidget_auth->item(ui.tableWidget_auth->currentRow(), 0)->text().toInt();
        QSqlQuery delAuth(QString("delete from book_auth where book_auth.id = \'%1\' ").arg(id));
        delAuth.exec();
        updateAuthors(list.at(item));
    }
}

void b_card::addPhoto(){
    if (ui.lineEdit_title->text().isEmpty()){
        QMessageBox messa;
        messa.setText("Введите название книги...");
        messa.exec();
    } else {
        QSettings sett("Skyrim", "lib");
        QString path(QString("%1/photo/").arg(sett.value("path").toString()));

        QString fileName = QFileDialog::getOpenFileName(this,
             "Open Image", "/home", tr("Image Files (*.png *.jpg *.bmp)"));

        ui.lineEdit_file_photo->setText(QString("%1%2%3").arg(path).arg(list.at(item)).arg(fileName.right(4)));

        QFile _f(fileName);
        if (_f.copy(fileName, ui.lineEdit_file_photo->text()) == true){
            QPixmap pix(ui.lineEdit_file_photo->text());
            QPixmap _p(pix.scaled(160, 250, Qt::KeepAspectRatio, Qt::FastTransformation));
            ui.label_photo->setPixmap(_p);
        }
    }
}

void b_card::delPhoto(){
    if (!ui.lineEdit_file_photo->text().isEmpty()){
        QFile _f(ui.lineEdit_file_photo->text());
        _f.remove();
        ui.lineEdit_file_photo->clear();
        ui.label_photo->clear();
    }
}

void b_card::addEBook(){
    if (ui.lineEdit_title->text().isEmpty()){
        QMessageBox messa;
        messa.setText("Введите название книги...");
        messa.exec();
    } else {
        QSettings sett("Skyrim", "lib");
        QString path(QString("%1/ebook/").arg(sett.value("path").toString()));

        QString fileName = QFileDialog::getOpenFileName(this,
             "Open Book", "/home/", "eBook Files (*.pdf *.djvu *.txt *.doc)");
        if (fileName.size() > 0){
            QFileInfo fi(fileName);
            ui.lineEdit_file->setText(QString("%1%2.%3").arg(path).arg(list.at(item)).arg(fi.completeSuffix()));

            QFile _f(fileName);
            _f.copy(fileName, ui.lineEdit_file->text());
        }
    }
}

void b_card::delEBook(){
    if (!ui.lineEdit_file->text().isEmpty()){
        QFile _f(ui.lineEdit_file->text());
        _f.remove();
        ui.lineEdit_file->clear();
    }
}

void b_card::openBook(){ ////////???????????????????????
    if (!ui.lineEdit_file->text().isEmpty()){
        QDesktopServices::openUrl(ui.lineEdit_file->text());
    }
}

void b_card::saveCard(){
    qDebug() << file_photo;
    QSqlQuery _s(QString("select sections.id from sections where sections.name = \'%1\' ")
                 .arg(ui.comboBox_sect->currentText()));
    _s.next();
    QSqlQuery _c(QString("select category.id from category where category.name = \'%1\' ")
                 .arg(ui.comboBox_cat->currentText()));
    _c.next();
    QSqlQuery _p(QString("select publish.id from publish where publish.name = \'%1\' ")
                 .arg(ui.comboBox_pub->currentText()));
    _p.next();
    QSqlQuery _l(QString("select languages.id from languages where languages.name = \'%1\' ")
                 .arg(ui.comboBox_lang->currentText()));
    _l.next();

    QSqlQuery upCard(QString("update books set title = \'%1\', isbn = \'%2\', sect = \'%3\', cat = \'%4\', "
                         "pub = \'%5\', year = \'%6\', lang = \'%7\', note = \'%8\', "
                         "photo = \'%9\', ebook = \'%10\' where books.id = \'%11\' ")
                 .arg(ui.lineEdit_title->text())
                 .arg(ui.lineEdit_isbn->text())
                 .arg(_s.value(0).toInt())
                 .arg(_c.value(0).toInt())
                 .arg(_p.value(0).toInt())
                 .arg(ui.spinBox_year->value())
                 .arg(_l.value(0).toInt())
                 .arg(ui.lineEdit_note->text())
                 .arg(ui.lineEdit_file_photo->text())
                 .arg(ui.lineEdit_file->text())
                 .arg(list.at(item)));
    upCard.exec();
    if (upCard.lastError().text().size() == 1) {
        ui.lineEdit_status->setText("Сохранено...");
        if (card == true){
            addCard();
        } else if (card == false){
            updateCard();
        }
    } else {
        ui.lineEdit_status->setText(upCard.lastError().text());
    }

}

void b_card::deleteCard(){
    QSqlQuery qClose(QString("delete from books where books.id = \'%1\' ").arg(list.at(item)));
    qClose.exec();
    delAuthors();
    delPhoto();
    delEBook();
    close();
}

void b_card::closeCard(){
    if (card == true){
        deleteCard();
    } else if (card == false){
        close();
    }
}

void b_card::addCard(){
    QSqlQuery _n("insert into books (note) values (:note)");
    _n.bindValue(0, " ");
    _n.exec();
    list.clear();
    list << _n.lastInsertId().toInt();
    item = 0;
    card = true;
    updateCard();
}

void b_card::updateIdentifiers(){
    for (int x = ui.tableWidget_identifiers->rowCount(); x >= 0; x--){
        ui.tableWidget_identifiers->removeRow(x);
    }
    QSqlQuery qIdentifiers(QString("select book_item.identifier from book_item where book_item.id_book = \'%1\'")
                           .arg(list.at(item)));
    int row = 0;
    while (qIdentifiers.next()){
        QTableWidgetItem *item = new QTableWidgetItem(qIdentifiers.value(0).toString());
        ui.tableWidget_identifiers->setItem(row, 0, item);
    }
    ui.spinBox_total->setValue(ui.tableWidget_identifiers->rowCount());
}

void b_card::lookHistoryBook(){

}
