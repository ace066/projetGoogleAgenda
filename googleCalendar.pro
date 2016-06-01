#-------------------------------------------------
#
# Project created by QtCreator 2016-04-22T20:18:24
#
#-------------------------------------------------

QT       += core gui network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = googleCalendar
TEMPLATE = app


SOURCES += main.cpp\
    Oauth2Google/google.cpp \
    Oauth2Google/o2.cpp \
    Oauth2Google/o2reply.cpp \
    Oauth2Google/o2replyserver.cpp \
    Oauth2Google/o2requestor.cpp \
    Oauth2Google/o2settingsstore.cpp \
    Oauth2Google/simplecrypt.cpp \
    calendrier.cpp

HEADERS  += \
    Oauth2Google/google.h \
    Oauth2Google/o2.h \
    Oauth2Google/o2abstractstore.h \
    Oauth2Google/o2globals.h \
    Oauth2Google/o2reply.h \
    Oauth2Google/o2replyserver.h \
    Oauth2Google/o2requestor.h \
    Oauth2Google/o2settingsstore.h \
    Oauth2Google/simplecrypt.h \
    calendrier.h

FORMS    += \
    calendrier.ui
