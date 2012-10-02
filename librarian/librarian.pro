#-------------------------------------------------
#
# Project created by QtCreator 2012-06-04T21:10:05
#
#-------------------------------------------------

QT       += core gui sql xml

TARGET = librarian
TEMPLATE = app


SOURCES += main.cpp\
        librarian.cpp \
    organizations.cpp \
    sect_cat.cpp \
    publish.cpp \
    authors.cpp \
    books.cpp \
    bookCard.cpp \
    readers.cpp \
    readerCard.cpp \
    readerBook.cpp \
    docs.cpp \
    doc_card.cpp \
    doc_item.cpp \
    searchReaders.cpp \
    import.cpp

HEADERS  += librarian.h \
    organizations.h \
    sect_cat.h \
    publish.h \
    authors.h \
    books.h \
    bookCard.h \
    readers.h \
    readerCard.h \
    readerBook.h \
    docs.h \
    doc_card.h \
    doc_item.h \
    searchReaders.h \
    import.h

FORMS    += librarian.ui \
    organizations.ui \
    sect_cat.ui \
    publish.ui \
    authors.ui \
    books.ui \
    bookCard.ui \
    readers.ui \
    readerCard.ui \
    readerBook.ui \
    docs.ui \
    docs_card.ui \
    doc_items.ui \
    searchReaders.ui \
    import.ui
