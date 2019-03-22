/*
 * This file is part of the particle tracking software CorrTrack.
 *
 * Copyright 2018-2019 Nicolas Bruot and CNRS
 * Copyright 2016-2018 Nicolas Bruot
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
#include <QPoint>
#include <QPushButton>
#include <QComboBox>
#include <QDoubleValidator>
#include <QGraphicsLineItem>
#include <QSlider>
#include <QObject>
#include <QMenu>
#include <QEvent>
#include <QCloseEvent>
#include <QTimer>
#include <QThread>
#include <QToolBar>
#include <QString>
#include <QVector>
#include <QRgb>
#include <QWidget>
#include <QGraphicsEllipseItem>
#include "noscrollqgraphicsview.h"
#include "settings.h"
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
    Settings *settings;

    bool movieIsSet;
    IntensityMode intensityMode;
    QVector<QRgb> colorTable;
    unsigned int bitDepth;
    uint16_t intensityMin;
    uint16_t intensityMax;
    int zoomIndex;
    QString filterFile;
    double framerate;
    double currentTime;
    size_t currentFrameIndex;

    bool isDrawingLine;

    CorrTrackAnalyser *analyser;
    AnalyseWorker* analyseWorker;
    QThread *taskThread;
    OpenMovieWorker* openMovieWorker;
    ExtractTiffsWorker* extractTiffsWorker;
    ProgressWindow* progressWindow;

    QTimer *playTimer;

    QGraphicsScene *scene;
    QGraphicsRectItem *frame;
    NoScrollQGraphicsView *view;
    QGraphicsPixmapItem *pixmapItem;
    QGraphicsLineItem *line;

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
    QToolBar *imageToolBar;
    // File
    QAction *openAct;
    QAction *testCorrAct;
    QAction *analyseAct;
    QAction *extractCurrentTiffAct;
    QAction *extractTiffsAct;
    QAction *closeAct;
    QAction *settingsAct;
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
    // Image toolbar
    QAction *lineToolAct;
    QAction *trackingAreaToolAct;

    QMenu *fileNameLabelMenu;
    QAction *copyFileNameAct;
    QAction *copyPathAct;

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
    void createToolBars();
    void setMovieRelatedItemsEnabled(const bool state);
    void displayMessageBox(const QString text) const;
    void updateColorTable();
    void updateFrameDisplay();

private slots:
    // File
    void openMovieWithDialog();
    void testCorrelation();
    void analyse();
    void extractCurrentTiff();
    void extractTiffs();
    void closeMovie();
    void editSettings();
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

    void copyFileName();
    void copyPath();

    void showFileNameLabelContextMenu(const QPoint& pos);

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

    virtual QMenu* createPopupMenu() override;

protected:
    virtual bool eventFilter(QObject *target, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
};
