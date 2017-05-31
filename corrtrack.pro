# This file is part of the particle tracking software CorrTrack.
#
# Copyright 2016, 2017 Nicolas Bruot
#
#
# CorrTrack is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# CorrTrack is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with CorrTrack.  If not, see <http://www.gnu.org/licenses/>.


# This configuration supports at least the following environments:
#
#   - MSVC 2015 (Release x86, x64) (tested on Windows 10)
#
#   - GCC, (Release x86, Debug x86) (tested on Debian Jessie)
#
# On Windows, add tiff.dll and zlib.dll in the folder of the executable, or the
# program will crash at startup.


TEMPLATE = app
TARGET = corrtrack

INCLUDEPATH += src

unix:INCLUDEPATH += \
    /usr/include/boost \
    /usr/include/gsl

unix:LIBS += \
    -lboost_regex \
    -lboost_filesystem \
    -lboost_system \
    -lgsl \
    -lgslcblas \
    -ltiff

contains(QT_ARCH, i386) {
    unix:INCLUDEPATH += \
        /usr/include/i386-linux-gnu/
    unix:LIBS += -L/usr/lib/i386-linux-gnu

    win32:INCLUDEPATH += \
        C:\lib\msvc2015_32\include
    win32:LIBS += \
        C:\lib\msvc2015_32\lib\gsl\cblas.lib \
        C:\lib\msvc2015_32\lib\gsl\gsl.lib \
        C:\lib\msvc2015_32\lib\boost\libboost_regex-vc140-mt-1_64.lib \
        C:\lib\msvc2015_32\lib\boost\libboost_system-vc140-mt-1_64.lib \
        C:\lib\msvc2015_32\lib\boost\libboost_filesystem-vc140-mt-1_64.lib \
        C:\lib\msvc2015_32\lib\tiff\tiff.lib
} else {
    unix:INCLUDEPATH += \
        /usr/include/x86_64-linux-gnu/
    unix:LIBS += -L/usr/lib/x86_64-linux-gnu

    win32:INCLUDEPATH += \
        C:\lib\msvc2015_64\include
    win32:LIBS += \
        C:\lib\msvc2015_64\lib\gsl\cblas.lib \
        C:\lib\msvc2015_64\lib\gsl\gsl.lib \
        C:\lib\msvc2015_64\lib\boost\libboost_regex-vc140-mt-1_64.lib \
        C:\lib\msvc2015_64\lib\boost\libboost_system-vc140-mt-1_64.lib \
        C:\lib\msvc2015_64\lib\boost\libboost_filesystem-vc140-mt-1_64.lib \
        C:\lib\msvc2015_64\lib\tiff\tiff.lib
}
win32:QMAKE_LFLAGS += /NODEFAULTLIB:libcmt

CONFIG += c++11

CONFIG(release, debug|release) {
    # See http://stackoverflow.com/a/32807272
    #
    # Release variant of Boost binary libraries is compiled with
    # disabled run-time assertion (NDEBUG is defined).
    # To align binaries with header-only libraries and other headers
    # it is possible to define NDEBUG for project release build.
    # It is not defined in Qt by default.
    DEFINES += NDEBUG
}

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VPATH += src

SOURCES += \
    main.cpp \
    corrtrackwindow.cpp \
    constants.cpp \
    zoomdialog.cpp \
    corrfilterdialog.cpp \
    okcanceldialog.cpp \
    movie/base/frame.cpp \
    math/math.cpp \
    math/corrfilter.cpp \
    math/corrtrackanalyser.cpp \
    math/point.cpp \
    math/imaged.cpp \
    math/pointd.cpp \
    io/exceptions/ioexception.cpp \
    movie/movie.cpp \
    movieintensityminmaxworker.cpp \
    noscrollqgraphicsview.cpp \
    analyseworker.cpp \
    progresswindow.cpp \
    extracttiffsworker.cpp \
    openmovieworker.cpp \
    intensitydialog.cpp \
    movie/base/version.cpp

HEADERS += \
    corrtrackwindow.h \
    constants.h \
    zoomdialog.h \
    corrfilterdialog.h \
    okcanceldialog.h \
    movie/base/frame.h \
    math/math.h \
    math/corrfilter.h \
    math/corrtrackanalyser.h \
    math/point.h \
    math/imaged.h \
    math/pointd.h \
    io/exceptions/ioexception.h \
    movie/movie.h \
    movieintensityminmaxworker.h \
    noscrollqgraphicsview.h \
    analyseworker.h \
    progresswindow.h \
    extracttiffsworker.h \
    openmovieworker.h \
    intensitydialog.h \
    movie/base/movieformats.h \
    movie/base/version.h
