/*
 * This file is part of the particle tracking software CorrTrack.
 *
 * Copyright 2016, 2017 Nicolas Bruot
 *
 *
 * CorrTrack is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CorrTrack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CorrTrack.  If not, see <http://www.gnu.org/licenses/>.
 */


#pragma once


#include <QMainWindow>
#include <QPixmap>
#include <QAction>
#include <QActionGroup>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QComboBox>
#include <QDoubleValidator>
#include <QSlider>
#include <QObject>
#include <QEvent>
#include <QTimer>
#include <QThread>
#include <QString>
#include <QWidget>
#include <QGraphicsEllipseItem>
#include "noscrollqgraphicsview.h"
#include "movie/movie.h"
#include "math/corrtrackanalyser.h"
#include "openmovieworker.h"
#include "analyseworker.h"
#include "extracttiffsworker.h"
#include "progresswindow.h"


class CorrTrackWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum class IntensityMode {
        BitDepth,
        MinMax,
        AutoVariable,
    };

private:
    bool movieIsSet;
    IntensityMode intensityMode;
    unsigned int bitDepth;
    uint16_t intensityMin;
    uint16_t intensityMax;
    int zoomIndex;
    QString filterFile;
    double framerate;
    double currentTime;
    size_t currentFrameIndex;

    CorrTrackAnalyser *analyser;
    AnalyseWorker* analyseWorker;
    QThread *taskThread;
    OpenMovieWorker* openMovieWorker;
    ExtractTiffsWorker* extractTiffsWorker;
    ProgressWindow* progressWindow;

    QString lastFolder;
    QString lastMovieFolder;
    QString lastFilterFolder;

    QTimer *playTimer;

    QGraphicsScene *scene;
    QGraphicsRectItem *frame;
    NoScrollQGraphicsView *view;
    QGraphicsPixmapItem *pixmapItem;

    std::vector<QGraphicsEllipseItem*> *pointItems;
    std::vector<QGraphicsRectItem*> *innerRectItems;
    std::vector<QGraphicsRectItem*> *outerRectItems;
    std::vector<QGraphicsPixmapItem*> *correlationMapItems;

    QLabel *fileNameLabel;
    QSlider *movieSlider;
    QPushButton *playButton;
    QDoubleValidator* speedValidator;
    QComboBox* speedCBox;
    QLabel *frameIndexLabel;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *filterMenu;
    QMenu *helpMenu;
    QActionGroup *alignmentGroup;
    // File
    QAction *openAct;
    QAction *testCorrAct;
    QAction *analyseAct;
    QAction *extractCurrentTiffAct;
    QAction *extractTiffsAct;
    QAction *closeAct;
    QAction *exitAct;
    // View
    QAction *intensityAct;
    QAction *zoomAct;
    // Filter
    QAction *removeLastAct;
    QAction *removeAllAct;
    QAction *corrFilterAct;
    // Help
    QAction *aboutAct;
    QAction *aboutQtAct;

    void openMovie(const QString path);
    void updateZoom();
    void zoomIn();
    void zoomOut();
    void clearCorrelationMaps();
    void updateOuterRectItems();
    void updateCorrelationMaps();
    void updatePointsCtrlMenuItems();
    bool validateCorrelation() const;
    void initSlider();
    void addPoint(Point);
    void createActions();
    void createMenus();
    void setMovieRelatedItemsEnabled(const bool state);
    void displayMessageBox(const QString text) const;
    void updateFrameDisplay();
    int frameCoordinate(const int coordinate) const;
    int zoomedCoordinate(const int coordinate) const;

private slots:
    // File
    void openMovieWithDialog();
    void testCorrelation();
    void analyse();
    void extractCurrentTiff();
    void extractTiffs();
    void closeMovie();
    // View
    void intensity();
    void zoom();
    // Filter
    void removeLastPoint();
    void removeAllPoints();
    void corrFilter();
    // Help
    void about();
    void aboutQt() const;

    void onPlayButtonClick(const bool checked);
    void nextPlayedFrame();
    void updateFrameIndex(const int frame);
    void onSpeedChanged(const QString text);

    void onWindowLoaded();
    void onOpenMovieFinished(const QString &fileName, const QString& msg);
    void onAnalyseFinished(const QString& msg);
    void onExtractTiffsFinished(const QString& msg);

public:
    explicit CorrTrackWindow(QWidget *parent = 0);
    ~CorrTrackWindow();

protected:
    virtual bool eventFilter(QObject *target, QEvent *event) override;
};
