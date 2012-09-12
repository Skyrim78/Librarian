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
    bookCard.cpp

HEADERS  += librarian.h \
    organizations.h \
    sect_cat.h \
    publish.h \
    authors.h \
    books.h \
    bookCard.h

FORMS    += librarian.ui \
    organizations.ui \
    sect_cat.ui \
    publish.ui \
    authors.ui \
    books.ui \
    bookCard.ui
