#-------------------------------------------------
#
# Project created by QtCreator 2016-02-24T19:56:12
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Oauth2Google
TEMPLATE = app


SOURCES += main.cpp\
        google.cpp \
    o2.cpp \
    o2reply.cpp \
    o2replyserver.cpp \
    o2settingsstore.cpp \
    simplecrypt.cpp \
    o2requestor.cpp

HEADERS  += google.h \
    o2.h \
    o2abstractstore.h \
    o2globals.h \
    o2reply.h \
    o2replyserver.h \
    o2settingsstore.h \
    simplecrypt.h \
    o2requestor.h

FORMS    += google.ui
