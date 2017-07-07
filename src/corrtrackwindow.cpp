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


#include <QApplication>
#include <QStatusBar>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QBoxLayout>
#include <QSlider>
#include <QStringList>
#include <QPushButton>
#include <QComboBox>
#include <QValidator>
#include <QDoubleValidator>
#include <QEvent>
#include <QFileDialog>
#include <QDir>
#include <QWidget>
#include <QGraphicsSceneMouseEvent>
#include <QContextMenuEvent>
#include <QDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QImage>
#include <QFileInfo>
#include <QGraphicsSceneWheelEvent>
#include <QTimer>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include "constants.h"
#include "noscrollqgraphicsview.h"
#include "corrtrackwindow.h"
#include "intensitydialog.h"
#include "zoomdialog.h"
#include "corrfilterdialog.h"
#include "math/math.h"
#include "math/corrfilter.h"
#include "math/point.h"
#include "movie/base/frame.h"
#include "movie/base/frame.cpp" // needed because it is a template

#include "io/exceptions/ioexception.h"
#include "openmovieworker.h"
#include "analyseworker.h"
#include "extracttiffsworker.h"


using namespace constants;


CorrTrackWindow::CorrTrackWindow(QWidget *parent)
    : QMainWindow(parent),
      movieIsSet{false},
      intensityMode{IntensityMode::BitDepth},
      bitDepth{8},
      intensityMin{0},
      intensityMax{255},
      zoomIndex{0},
      filterFile{QString()},
      currentTime{0.0},
      currentFrameIndex{1},
      analyser{new CorrTrackAnalyser},
      analyseWorker{nullptr},
      taskThread{nullptr},
      openMovieWorker{nullptr},
      extractTiffsWorker{nullptr},
      progressWindow{nullptr},
      lastFolder{QDir::homePath()},
      lastMovieFolder{QString()},
      lastFilterFolder{QString()},
      playTimer{new QTimer(this)},
      scene{new QGraphicsScene(this)},
      frame{nullptr},
      view{new NoScrollQGraphicsView(this)},
      pixmapItem{new QGraphicsPixmapItem()},
      pointItems{new std::vector<QGraphicsEllipseItem*>()},
      innerRectItems{new std::vector<QGraphicsRectItem*>()},
      outerRectItems{new std::vector<QGraphicsRectItem*>()},
      correlationMapItems{new std::vector<QGraphicsPixmapItem*>()},
      fileNameLabel{new QLabel(this)},
      movieSlider{new QSlider(this)},
      playButton{new QPushButton(this)},
      speedValidator{new QDoubleValidator(this)},
      speedCBox{new QComboBox(this)},
      frameIndexLabel{new QLabel(this)}
{
    const int squareButtonWidth = 40;

    pixmapItem->setAcceptHoverEvents(true);
    pixmapItem->setZValue(FRAME_Z_VALUE);

    movieSlider->setOrientation(Qt::Horizontal);

    playButton->setText("►");
    playButton->setCheckable(true);
    playButton->setFixedWidth(squareButtonWidth);

    QStringList speedsList = QStringList()
            << "5" << "10" << "20" << "50" << "100" << "200" << "500" << "1000";
    speedCBox->addItems(speedsList);
    speedCBox->setCurrentIndex(4);
    framerate = speedCBox->currentText().toDouble();
    speedValidator->setRange(constants::MIN_FRAMERATE,
                             constants::MAX_FRAMERATE,
                             constants::FRAMERATE_DECIMALS);
    speedCBox->setValidator(speedValidator);
    speedCBox->setEditable(true);

    QHBoxLayout *sliderLayout = new QHBoxLayout();
    sliderLayout->addWidget(playButton);
    sliderLayout->addWidget(speedCBox);
    sliderLayout->addWidget(movieSlider);
    sliderLayout->addWidget(frameIndexLabel);

    playTimer->setTimerType(Qt::PreciseTimer);
    connect(playTimer, SIGNAL(timeout()),
            this, SLOT(nextPlayedFrame()));

    statusBar()->show();

    scene->installEventFilter(this);
    scene->addItem(pixmapItem);

    view->setScene(scene);

    // Keep view's size policy when hidden
    QSizePolicy sp = view->sizePolicy();
    sp.setRetainSizeWhenHidden(true);
    view->setSizePolicy(sp);

    QString title;
    if (VERSION == TARGET_VERSION)
        title = QString(APP_NAME);
    else
        title = QString(APP_NAME) + QString(" [DEV VERSION]");
    setWindowTitle(title);

    setMinimumSize(160, 160);
    resize(480, 320);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(view);
    layout->addWidget(fileNameLabel);
    layout->addLayout(sliderLayout);

    QWidget *w = new QWidget;
    w->setLayout(layout);
    setCentralWidget(w);

    updateZoom();

    createActions();
    createMenus();
    setMovieRelatedItemsEnabled(false);

    connect(speedCBox, SIGNAL(currentTextChanged(const QString)),
            this, SLOT(onSpeedChanged(const QString)));
    connect(playButton, SIGNAL(clicked(bool)),
            this, SLOT(onPlayButtonClick(const bool)));
    connect(movieSlider, SIGNAL(valueChanged(int)),
            this, SLOT(updateFrameIndex(const int)));

    QTimer::singleShot(0, this, SLOT(onWindowLoaded()));
}

CorrTrackWindow::~CorrTrackWindow()
{
}

int CorrTrackWindow::frameCoordinate(const int coordinate) const
{
    /* Converts a distance in pixels on the zoomed scene to pixels on
     * the frame.*/

    int result = (double) coordinate / pow(constants::ZOOM_BASE, zoomIndex);
    return result;
}

int CorrTrackWindow::zoomedCoordinate(const int coordinate) const
{
    /* Converts a distance in pixels on the frame to pixels on the
     * zoomed scene.*/

    int result = (double) coordinate * pow(constants::ZOOM_BASE, zoomIndex);
    return result;
}

void CorrTrackWindow::onWindowLoaded()
{
    if (QApplication::arguments().size() > 1)
    {
        // The program presumably launched by trying to open a file
        // in the OS. Let's try to open that file.
        openMovie(QApplication::arguments().at(1));
    }
}

bool CorrTrackWindow::eventFilter(QObject *target, QEvent *event)
{
    if (target == scene)
    {
        if (event->type() == QEvent::GraphicsSceneMousePress)
        {
            const QGraphicsSceneMouseEvent* const me = static_cast<const QGraphicsSceneMouseEvent*>(event);
            const QPointF position = me->scenePos();
            int x = frameCoordinate((int) position.x());
            int y = frameCoordinate((int) position.y());
            if (x >= 0 && x < (int) analyser->movie->width
                    && y >= 0 && y < (int) analyser->movie->height)
            {
                Point point(x, y);
                addPoint(point);
            }
        }
        else if (event->type() == QEvent::GraphicsSceneWheel)
        {
            QGraphicsSceneWheelEvent* wheelEvent;
            wheelEvent = static_cast<QGraphicsSceneWheelEvent*>(event);
            const int delta = wheelEvent->delta();
            if (delta > 0)
                zoomIn();
            else
                zoomOut();
        }
        else if (event->type() == QEvent::Leave)
        {
            statusBar()->clearMessage();
        }
        else if (event->type() == QEvent::GraphicsSceneMouseMove)
        {
            const QGraphicsSceneMouseEvent* const me = static_cast<const QGraphicsSceneMouseEvent*>(event);
            const QPointF position = me->scenePos();
            int x = frameCoordinate((int) position.x());
            int y = frameCoordinate((int) position.y());
            if (x >= 0 && x < (int) analyser->movie->width
                    && y >= 0 && y < (int) analyser->movie->height)
            {
                // If this event is triggered, movie is necessarily set.
                uint16_t value;
                if (analyser->movie->bitsPerSample == 8)
                {
                    Frame<uint8_t>* frame = &(analyser->movie->frames8.at(currentFrameIndex - 1));
                    value = (uint16_t) (frame->getPixelIntensity(x, y));
                }
                else // bitsPerSample == 16
                {
                    Frame<uint16_t>* frame = &(analyser->movie->frames16.at(currentFrameIndex - 1));
                    value = frame->getPixelIntensity(x, y);
                }
                statusBar()->showMessage(QString("(%1, %2), value=%3").arg(x + 1).arg(y + 1).arg(value));
            }
            else
            {
                statusBar()->clearMessage();
            }
        }
    }
    return QMainWindow::eventFilter(target, event);
}

void CorrTrackWindow::addPoint(Point point)
{
    // Add point to analyser and draw it.
    analyser->addPoint(point);
    // Add point
    const qreal offset = - POINT_RADIUS + pow(constants::ZOOM_BASE, zoomIndex) / 2.0;
    QGraphicsEllipseItem *ellipse;
    ellipse = scene->addEllipse(0.0, 0.0,
                                2.0 * POINT_RADIUS, 2.0 * POINT_RADIUS,
                                POINT_QPEN, POINT_QBRUSH);
    ellipse->setPos(zoomedCoordinate(point.x) + offset,
                    zoomedCoordinate(point.y) + offset);
    ellipse->setZValue(POINT_Z_VALUE);
    pointItems->push_back(ellipse);
    // Add inner rectangle
    QGraphicsRectItem *innerRect;
    qreal x0, y0;
    unsigned int width, height;
    width = analyser->windowWidth;
    height = analyser->windowHeight;
    x0 = zoomedCoordinate(point.x - (int)(width / 2));
    y0 = zoomedCoordinate(point.y - (int)(height / 2));
    innerRect = new QGraphicsRectItem(0.0, 0.0,
                                      zoomedCoordinate(width),
                                      zoomedCoordinate(height));
    innerRect->setPos(x0, y0);
    innerRect->setPen(INNER_RECT_QPEN);
    innerRect->setZValue(INNER_RECT_Z_VALUE);
    scene->addItem(innerRect);
    innerRectItems->push_back(innerRect);
    // Add outer rectangle
    QGraphicsRectItem *outerRect;
    unsigned int filterWidth = 1;
    unsigned int filterHeight = 1;
    if (analyser->filter->isFilterSet())
    {
        filterWidth = analyser->filter->width;
        filterHeight = analyser->filter->height;
    }
    width = analyser->windowWidth + filterWidth - 1;
    height = analyser->windowHeight + filterHeight - 1;
    x0 = point.x - (int)(analyser->windowWidth / 2) - (int)(filterWidth / 2);
    y0 = point.y - (int)(analyser->windowHeight / 2) - (int)(filterHeight / 2);
    outerRect = new QGraphicsRectItem(0.0, 0.0,
                                      zoomedCoordinate(width),
                                      zoomedCoordinate(height));
    outerRect->setPos(zoomedCoordinate(x0), zoomedCoordinate(y0));
    outerRect->setPen(OUTER_RECT_QPEN);
    outerRect->setZValue(OUTER_RECT_Z_VALUE);
    scene->addItem(outerRect);
    outerRectItems->push_back(outerRect);
    // Add correlation map (but do not display)
    QGraphicsPixmapItem *item = nullptr;
    correlationMapItems->push_back(item);

    updatePointsCtrlMenuItems();
}

void CorrTrackWindow::removeLastPoint()
{
    if (pointItems->size() > 0)
    {
        scene->removeItem(pointItems->back());
        scene->removeItem(innerRectItems->back());
        scene->removeItem(outerRectItems->back());
        pointItems->pop_back();
        innerRectItems->pop_back();
        outerRectItems->pop_back();
        if (correlationMapItems->back() != nullptr)
            scene->removeItem(correlationMapItems->back());

        correlationMapItems->pop_back();
        analyser->getPoints()->pop_back();
    }
    updatePointsCtrlMenuItems();
}

void CorrTrackWindow::removeAllPoints()
{
    for (unsigned int i = 0; i < pointItems->size(); i++)
    {
        scene->removeItem(pointItems->at(i));
        scene->removeItem(innerRectItems->at(i));
        scene->removeItem(outerRectItems->at(i));
        if (correlationMapItems->at(i) != nullptr)
            scene->removeItem(correlationMapItems->at(i));
    }
    pointItems->clear();
    innerRectItems->clear();
    outerRectItems->clear();
    correlationMapItems->clear();
    analyser->getPoints()->clear();
    updatePointsCtrlMenuItems();
}

void CorrTrackWindow::zoomIn()
{
    if (zoomIndex < ZOOM_POW_MAX)
    {
        zoomIndex += 1;
        updateZoom();
    }
}

void CorrTrackWindow::zoomOut()
{
    if (zoomIndex > ZOOM_POW_MIN)
    {
        zoomIndex -= 1;
        updateZoom();
    }
}

void CorrTrackWindow::updateZoom()
{
    if (movieIsSet)
    {
        const double factor = pow(constants::ZOOM_BASE, zoomIndex);
        pixmapItem->setScale(factor);
        view->setSceneRect(0, 0,
                           factor * pixmapItem->pixmap().width(),
                           factor * pixmapItem->pixmap().height());
    }

    // Update drawn points, rectangles and correlation maps
    const qreal offset = - POINT_RADIUS + pow(constants::ZOOM_BASE, zoomIndex) / 2.0;
    QGraphicsEllipseItem *ellipse;
    QGraphicsRectItem *innerRect;
    const unsigned int width = analyser->windowWidth;
    const unsigned int height = analyser->windowHeight;
    for (unsigned int i = 0; i < pointItems->size(); i++)
    {
        ellipse = pointItems->at(i);
        ellipse->setPos(zoomedCoordinate(analyser->getPoints()->at(i).x) + offset,
                        zoomedCoordinate(analyser->getPoints()->at(i).y) + offset);
        innerRect = innerRectItems->at(i);
        innerRect->setPos(zoomedCoordinate(analyser->getPoints()->at(i).x
                                           - (int)(width / 2)),
                          zoomedCoordinate(analyser->getPoints()->at(i).y
                                           - (int)(height / 2)));
        innerRect->setRect(0.0, 0.0,
                           zoomedCoordinate(width),
                           zoomedCoordinate(height));
    }

    updateOuterRectItems();
    updateCorrelationMaps();
}

void CorrTrackWindow::updateOuterRectItems()
{
    unsigned int filterWidth = 1;
    unsigned int filterHeight = 1;
    if (analyser->filter->isFilterSet())
    {
        filterWidth = analyser->filter->width;
        filterHeight = analyser->filter->height;
    }
    const unsigned int width = analyser->windowWidth + filterWidth - 1;
    const unsigned int height = analyser->windowHeight + filterHeight - 1;
    QGraphicsRectItem *outerRect;
    for (unsigned int i = 0; i < outerRectItems->size(); i++)
    {
        const int x0 = analyser->getPoints()->at(i).x
                - (int)(analyser->windowWidth / 2)
                - (int)(filterWidth / 2);
        const int y0 = analyser->getPoints()->at(i).y
                - (int)(analyser->windowHeight / 2)
                - (int)(filterHeight / 2);
        outerRect = outerRectItems->at(i);
        outerRect->setRect(0.0, 0.0,
                           zoomedCoordinate(width),
                           zoomedCoordinate(height));
        outerRect->setPos(zoomedCoordinate(x0),
                          zoomedCoordinate(y0));
    }
}

void CorrTrackWindow::updateCorrelationMaps()
{
    QGraphicsPixmapItem *item;
    for (unsigned int i = 0; i < pointItems->size(); i++)
    {
        item = correlationMapItems->at(i);
        if (item != nullptr)
        {
            const Point point = analyser->getPoints()->at(i);
            item->setPos(zoomedCoordinate(point.x - analyser->windowWidth / 2),
                         zoomedCoordinate(point.y - analyser->windowHeight / 2));
            item->setScale(pow(constants::ZOOM_BASE, zoomIndex));
        }
    }
}

void CorrTrackWindow::updatePointsCtrlMenuItems()
{
    bool enableMenus = (pointItems->size() > 0);
    removeLastAct->setEnabled(enableMenus);
    removeAllAct->setEnabled(enableMenus);
}

void CorrTrackWindow::testCorrelation()
{
    if (!validateCorrelation()) return;

    // Correlation
    std::vector<ImageD*>* correlationMaps;
    analyser->selectImage(movieSlider->value() - 1);
    try
    {
        correlationMaps = analyser->testCorrelation();
    }
    catch (CorrTrackAnalyser::AnalyseException& e)
    {
        QString message("Analyse error: ");
        message += QString::fromStdString(e.what());
        message += QString(" Aborting.");
        displayMessageBox(message);
        return;
    }

    // Clear any old items
    clearCorrelationMaps();
    correlationMapItems->clear();

    for (unsigned int i = 0; i < analyser->getPoints()->size(); i++)
    {
        ImageD* correlationMap = correlationMaps->at(i);
        QImage image = QImage(correlationMap->rescaledPixelsData(),
                              correlationMap->width,
                              correlationMap->height,
                              correlationMap->width,
                              QImage::Format_Indexed8);
        QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
        item->setZValue(CORRELATION_Z_VALUE);
        scene->addItem(item);
        correlationMapItems->push_back(item);
    }
    updateZoom();
}

void CorrTrackWindow::onPlayButtonClick(const bool checked)
{
    // Start/stop playing the movie
    if (checked)
        playTimer->start(50); // 1000.0 / constants::DISPLAY_REFRESH_RATE);
    else
        playTimer->stop();
}

void CorrTrackWindow::nextPlayedFrame()
{
    double newTime = currentTime + 1.0 / constants::DISPLAY_REFRESH_RATE;
    size_t newFrameIndex = 1 + (double)(newTime * framerate);
    if (newFrameIndex > analyser->movie->nFrames)
    {
        playTimer->stop();
        playButton->setChecked(false);
        newFrameIndex = 1;
        newTime = 0.0;
    }
    bool oldState = movieSlider->blockSignals(true);
    movieSlider->setValue((int) newFrameIndex);
    movieSlider->blockSignals(oldState);
    currentFrameIndex = newFrameIndex;
    currentTime = newTime;
    updateFrameDisplay();
}

void CorrTrackWindow::onSpeedChanged(QString text)
{
    int pos = speedCBox->currentIndex();
    QValidator::State state = speedValidator->validate(text, pos);
    if (state == QValidator::State::Acceptable)
    {
        framerate = text.toDouble();
        currentTime = (currentFrameIndex - 1) / framerate;
    }
}

void CorrTrackWindow::updateFrameIndex(const int frame)
{
    currentFrameIndex = frame;
    currentTime = (double) frame / speedCBox->currentText().toDouble();
    updateFrameDisplay();
}

void CorrTrackWindow::updateFrameDisplay()
{
    if (movieIsSet)
    {
        uint8_t *data;
        switch (intensityMode)
        {
        case IntensityMode::BitDepth:
            data = analyser->movie->frameData8(currentFrameIndex - 1, bitDepth);
            break;
        case IntensityMode::AutoVariable:
            analyser->movie->getFrameIntensityMinMax(currentFrameIndex - 1,
                                                     intensityMin, intensityMax);
        default: // MinMax, AutoVariable
            data = analyser->movie->frameData8(currentFrameIndex - 1,
                                               intensityMin, intensityMax);
            break;
        }

        QImage image = QImage(data,
                              analyser->movie->width,
                              analyser->movie->height,
                              analyser->movie->width,
                              QImage::Format_Indexed8);
        pixmapItem->setPixmap(QPixmap::fromImage(image));
        delete[] data;
        frameIndexLabel->setText(QString("%1 / %2").arg(currentFrameIndex).arg(analyser->movie->nFrames));
    }
}

void CorrTrackWindow::initSlider()
{
    movieSlider->setRange(1, (int) analyser->movie->nFrames);
    movieSlider->setValue(1);
    currentTime = 0.0;
    currentFrameIndex = 1;
    updateFrameDisplay();
}

void CorrTrackWindow::openMovie(const QString path)
{
    progressWindow = new ProgressWindow(this);
    progressWindow->setWindowTitle("Opening file...");
    progressWindow->setNStepsPtr(&(analyser->movie->nFrames));
    analyser->movie->currIndex = 0;
    progressWindow->setStepPtr(&(analyser->movie->currIndex));
    progressWindow->open();

    openMovieWorker = new OpenMovieWorker(analyser, path);
    taskThread = new QThread;
    openMovieWorker->moveToThread(taskThread);
    connect(taskThread, &QThread::started,
            openMovieWorker, &OpenMovieWorker::openMovie);
    connect(openMovieWorker, &OpenMovieWorker::finishedWithMessage,
            this, &CorrTrackWindow::onOpenMovieFinished);
    taskThread->start();
}

void CorrTrackWindow::openMovieWithDialog()
{
    QString fileName;

    if (movieIsSet)
    {
        QMessageBox::StandardButton askClosureBox;
          askClosureBox = QMessageBox::question(this, "Close",
                                                "Close the current file?",
                                                QMessageBox::Yes|QMessageBox::No);
          if (askClosureBox == QMessageBox::Yes)
              closeMovie();
          else
              return;
    }

    QString folder = lastMovieFolder.isEmpty() ? lastFolder : lastMovieFolder;
    fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), folder,
        tr("Movie and Image Files (*.rawm *.xiseq *.tif *.tiff *.png *.jpg *.bmp)"));
    if (fileName.isEmpty() || fileName.isNull())
        return;
    lastMovieFolder = QFileInfo(fileName).path();
    lastFolder = lastMovieFolder;

    openMovie(fileName);
}

void CorrTrackWindow::closeMovie()
{
    if (playButton->isChecked())
    {
        playButton->setChecked(false);
        onPlayButtonClick(false);
    }

    analyser->resetMovie();
    movieIsSet = false;

    view->hide();
    movieSlider->setRange(1, 1);
    movieSlider->setValue(1);
    currentTime = 0.0;
    currentFrameIndex = 1;

    intensityMode = IntensityMode::BitDepth;
    bitDepth = 8;
    intensityMin = 0;
    intensityMax = 255;

    // Clear display
    removeAllPoints();
    pixmapItem->setPixmap(QPixmap());

    frameIndexLabel->setText(QString(""));
    fileNameLabel->setText("");

    setMovieRelatedItemsEnabled(false);
}

bool CorrTrackWindow::validateCorrelation() const
{
    // Data validation
    if (!(analyser->filter->isFilterSet()))
    {
        displayMessageBox("No filter set.");
        return false;
    }

    if (pointItems->size() == 0)
    {
        displayMessageBox("No areas selected for analyse.");
        return false;
    }
    return true;
}

void CorrTrackWindow::analyse()
{
    if (!validateCorrelation()) return;

    // Prevent a short display of strange progress values when showing the
    // progress bar:
    analyser->selectImage(0);

    progressWindow = new ProgressWindow(this);
    progressWindow->setWindowTitle("Analysing...");
    progressWindow->setNStepsPtr(&(analyser->movie->nFrames));
    progressWindow->setStepPtr(&(analyser->currFrameIndex));
    progressWindow->open();

    analyseWorker = new AnalyseWorker(analyser);
    taskThread = new QThread;
    analyseWorker->moveToThread(taskThread);
    connect(taskThread, &QThread::started,
            analyseWorker, &AnalyseWorker::analyse);
    connect(analyseWorker, &AnalyseWorker::finishedWithMessage,
            this, &CorrTrackWindow::onAnalyseFinished);
    taskThread->start();
}

void CorrTrackWindow::extractCurrentTiff()
{
    try
    {
        analyser->movie->extractTiff(currentFrameIndex - 1);
    }
    catch (std::ios_base::failure& e)
    {
        QString msg = QString("Error while extracting frames: ");
        msg += QString::fromStdString(e.what());
        msg += QString(" Aborting.");
        displayMessageBox(msg);
        return;
    }
    statusBar()->showMessage("TIFF extracted.");
}

void CorrTrackWindow::extractTiffs()
{
    progressWindow = new ProgressWindow(this);
    progressWindow->setWindowTitle("Extracting TIFF images...");
    progressWindow->setNStepsPtr(&(analyser->movie->nFrames));
    analyser->movie->currIndex = 0;
    progressWindow->setStepPtr(&(analyser->movie->currIndex));
    progressWindow->open();

    extractTiffsWorker = new ExtractTiffsWorker(analyser);
    taskThread = new QThread;
    extractTiffsWorker->moveToThread(taskThread);
    connect(taskThread, &QThread::started,
            extractTiffsWorker, &ExtractTiffsWorker::extractTiffs);
    connect(extractTiffsWorker, &ExtractTiffsWorker::finishedWithMessage,
            this, &CorrTrackWindow::onExtractTiffsFinished);
    taskThread->start();
}

void CorrTrackWindow::onOpenMovieFinished(const QString& fileName,
                                          const QString& msg)
{
    progressWindow->hide();
    delete progressWindow;
    disconnect(taskThread, &QThread::started,
               openMovieWorker, &OpenMovieWorker::openMovie);
    disconnect(openMovieWorker, &OpenMovieWorker::finishedWithMessage,
               this, &CorrTrackWindow::onOpenMovieFinished);
    // Not sure that the following is entirely safe.  For example, what if a new
    // thread is created before the old objects are actually deleted?
    taskThread->quit();
    openMovieWorker->deleteLater();
    taskThread->deleteLater();
    taskThread->wait();

    if (msg.isEmpty())
    {
        movieIsSet = true;
        bitDepth = analyser->movie->bitDepth;
        intensityMode = IntensityMode::BitDepth;
        QFileInfo fileInfo = QFileInfo(fileName);
        fileNameLabel->setText(fileInfo.fileName());
        QString message = QString("Loaded %1.").arg(fileInfo.fileName());
        view->show();
        statusBar()->showMessage(message);
    }

    if (msg.isEmpty())
    {
        bitDepth = analyser->movie->bitDepth;
        intensityMin = 0;
        intensityMax = math::ipow(2, bitDepth) - 1;

        initSlider();
        updateZoom();

        if (analyser->movie->framerate != 0)
            speedCBox->setCurrentText(QString::number(analyser->movie->framerate));
        setMovieRelatedItemsEnabled(true);

        QFileInfo fileInfo = QFileInfo(fileName);
        fileNameLabel->setText(fileInfo.fileName());
        if (analyser->movie->framerate != 0)
            speedCBox->setCurrentText(QString::number(analyser->movie->framerate));
        QString message = QString("Loaded %1.").arg(fileInfo.fileName());
        statusBar()->showMessage(message);

        setMovieRelatedItemsEnabled(true);
    }
    else
    {
        displayMessageBox(msg);
    }
}

void CorrTrackWindow::onAnalyseFinished(const QString& msg)
{
    progressWindow->hide();
    delete progressWindow;
    disconnect(taskThread, &QThread::started,
               analyseWorker, &AnalyseWorker::analyse);
    disconnect(analyseWorker, &AnalyseWorker::finishedWithMessage,
               this, &CorrTrackWindow::onAnalyseFinished);
    // Not sure that the following is entirely safe.  For example, what if a new
    // thread is created before the old objects are actually deleted?
    taskThread->quit();
    analyseWorker->deleteLater();
    taskThread->deleteLater();
    taskThread->wait();

    displayMessageBox(msg);
}

void CorrTrackWindow::onExtractTiffsFinished(const QString& msg)
{
    progressWindow->hide();
    delete progressWindow;
    disconnect(taskThread, &QThread::started,
               extractTiffsWorker, &ExtractTiffsWorker::extractTiffs);
    disconnect(extractTiffsWorker, &ExtractTiffsWorker::finishedWithMessage,
               this, &CorrTrackWindow::onExtractTiffsFinished);
    // Not sure that the following is entirely safe.  For example, what if a new
    // thread is created before the old objects are actually deleted?
    taskThread->quit();
    extractTiffsWorker->deleteLater();
    taskThread->deleteLater();
    taskThread->wait();

    displayMessageBox(msg);
}

void CorrTrackWindow::intensity()
{
    IntensityMode oldIntensityMode = intensityMode;
    unsigned int oldBitDepth = bitDepth;
    unsigned long oldIntensityMin = intensityMin;
    unsigned long oldIntensityMax = intensityMax;
    // Show intensity settings dialog
    IntensityDialog *dialog = new IntensityDialog(intensityMode,
                                                  intensityMin, intensityMax,
                                                  analyser->movie,
                                                  currentFrameIndex - 1,
                                                  this);
    if (dialog->exec() == QDialog::Accepted)
    {
        intensityMode = dialog->getIntensityMode();
        if (intensityMode == IntensityMode::BitDepth)
                bitDepth = dialog->getBitDepth();
        intensityMin = dialog->getIntensityMin();
        intensityMax = dialog->getIntensityMax();
    }

    // Apply new intensity settings
    if (intensityMode != oldIntensityMode || bitDepth != oldBitDepth
            || intensityMin != oldIntensityMin
            || intensityMax != oldIntensityMax)
    {
        statusBar()->showMessage(QString("Intensity settings changed."));
        updateFrameDisplay();
    }
}

void CorrTrackWindow::zoom()
{
    int oldZoomIndex = zoomIndex;
    // Show zoom dialog
    ZoomDialog *dialog = new ZoomDialog(zoomIndex, this);
    if (dialog->exec() == QDialog::Accepted)
        zoomIndex = dialog->getZoomIndex();

    // Apply new zoom value
    if (zoomIndex != oldZoomIndex)
    {
        statusBar()->showMessage(QString("Zoom index set to %1.").arg(zoomIndex));
        updateZoom();
    }
}

void CorrTrackWindow::corrFilter()
{
    unsigned int oldWidth = analyser->windowWidth;
    unsigned int oldHeight = analyser->windowHeight;
    double oldFitRadius = analyser->fitRadius;

    QString oldFilterFile = filterFile;

    // Show correlation filter dialog
    CorrFilterDialog *dialog = new CorrFilterDialog(oldWidth,
                                                    oldHeight,
                                                    filterFile,
                                                    oldFitRadius,
                                                    lastFilterFolder,
                                                    lastFolder,
                                                    this);

    if (dialog->exec() == QDialog::Accepted)
    {
        analyser->windowWidth = dialog->getFilterWindowWidth();
        analyser->windowHeight = dialog->getFilterWindowHeight();
        filterFile = dialog->getFilterFile();
        analyser->fitRadius = dialog->getFitRadius();
        lastFilterFolder = dialog->lastFilterFolder;
        lastFolder = dialog->lastFolder;
    }

    if ((oldWidth != analyser->windowWidth)
        | (oldHeight != analyser->windowHeight))
    {
        clearCorrelationMaps();
    }

    if (oldFilterFile != filterFile)
    {
        // Load filter
        try
        {
            analyser->filter->setFilter(filterFile.toStdString());
            clearCorrelationMaps();
        }
        catch (IOException& e)
        {
            filterFile = oldFilterFile;
            displayMessageBox(e.what());
        }
        catch (CorrFilter::CorrFilterFormatException& e)
        {
            filterFile = oldFilterFile;
            displayMessageBox(e.what());
        }
    }

    updateZoom();
}

void CorrTrackWindow::clearCorrelationMaps()
{
    for (QGraphicsPixmapItem* &item : *correlationMapItems)
    {
        if (item != nullptr)
        {
            scene->removeItem(item);
            item = nullptr;
        }
    }
}

void CorrTrackWindow::displayMessageBox(const QString text) const
{
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

void CorrTrackWindow::about()
{
    QString message("");
    message.append(APP_NAME);
    message.append(" particle tracking software");
    message.append("<br/><br/>Version ");
    message.append(VERSION);
    if (strcmp(VERSION, TARGET_VERSION) != 0)
    {
        message.append(" (target: ");
        message.append(TARGET_VERSION);
        message.append(")");
    }
    message.append("<br/><br/>Copyright (C) 2016, 2017 Nicolas Bruot<br/><br/>"
                   "CorrTrack is released under the terms of the GNU General Public License (GPL) v3.<br/>"
                   "The source code is available at <a href=\"https://github.com/bruot/corrtrack/\">https://github.com/bruot/corrtrack/</a>.<br/><br/>"
                   "This program uses:"
                   "<ul>"
                   "<li><a href=\"https://www.qt.io/\">Qt 5</a><br/>"
                   "GNU Lesser General Public License (LGPL) v3</li>"
                   "<li><a href=\"http://www.boost.org/users/license.html\">Boost</a></li>"
                   "<li><a href=\"https://www.gnu.org/software/gsl/\">Gnu Scientific Library</a><br/>"
                   "GNU General Public License (GPL) v3</li>"
                   "<li><a href=\"http://www.linuxfromscratch.org/blfs/view/svn/general/libtiff.html\">LibTIFF</a><br/>"
                   "Copyright (C) 1988-1997 Sam Leffler<br/>"
                   "Copyright (C) 1991-1997 Silicon Graphics, Inc.</li>"
                   "<li><a href=\"http://www.zlib.net/\">zlib</a><br/>"
                   "Copyright (C) 1995-2013 Jean-loup Gailly and Mark Adler</li>"
                   "</ul>"
                   );

    QMessageBox::about(this, QString("About %1").arg(APP_NAME), message);
}

void CorrTrackWindow::aboutQt() const {}

void CorrTrackWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openMovieWithDialog()));

    testCorrAct = new QAction(tr("&Test correlation"), this);
    testCorrAct->setShortcut(QKeySequence(tr("Ctrl+T")));
    testCorrAct->setStatusTip(tr("Test the analyse on the current frame"));
    connect(testCorrAct, SIGNAL(triggered()), this, SLOT(testCorrelation()));

    analyseAct = new QAction(tr("&Analyse"), this);
    analyseAct->setShortcut(QKeySequence(tr("Ctrl+A")));
    analyseAct->setStatusTip(tr("Analyse the current file"));
    connect(analyseAct, SIGNAL(triggered()), this, SLOT(analyse()));

    extractCurrentTiffAct = new QAction(tr("Save &frame as TIFF"), this);
    extractCurrentTiffAct->setStatusTip(tr("Extract current frame to TIFF image"));
    connect(extractCurrentTiffAct, SIGNAL(triggered()), this, SLOT(extractCurrentTiff()));

    extractTiffsAct = new QAction(tr("&Save movie as TIFFs"), this);
    extractTiffsAct->setStatusTip(tr("Extract movie frames to individual TIFF image files"));
    connect(extractTiffsAct, SIGNAL(triggered()), this, SLOT(extractTiffs()));

    closeAct = new QAction(tr("&Close"), this);
    closeAct->setShortcuts(QKeySequence::Close);
    closeAct->setStatusTip(tr("Close the current file"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(closeMovie()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    intensityAct = new QAction(tr("&Intensity settings"), this);
    intensityAct->setStatusTip(tr("Intensity settings"));
    connect(intensityAct, SIGNAL(triggered()), this, SLOT(intensity()));

    zoomAct = new QAction(tr("&Zoom"), this);
    zoomAct->setStatusTip(tr("Zoom options"));
    connect(zoomAct, SIGNAL(triggered()), this, SLOT(zoom()));

    corrFilterAct = new QAction(tr("&Filter"), this);
    corrFilterAct->setStatusTip(tr("Correlation filter options"));
    connect(corrFilterAct, SIGNAL(triggered()), this, SLOT(corrFilter()));

    removeLastAct = new QAction(tr("&Remove last point"), this);
    removeLastAct->setStatusTip(tr("Remove last point"));
    connect(removeLastAct, SIGNAL(triggered()), this, SLOT(removeLastPoint()));

    removeAllAct = new QAction(tr("&Remove all points"), this);
    removeAllAct->setStatusTip(tr("Remove all points"));
    connect(removeAllAct, SIGNAL(triggered()), this, SLOT(removeAllPoints()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(aboutQtAct, SIGNAL(triggered()), this, SLOT(aboutQt()));
}

void CorrTrackWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(testCorrAct);
    fileMenu->addAction(analyseAct);
    fileMenu->addSeparator();
    fileMenu->addAction(extractCurrentTiffAct);
    fileMenu->addAction(extractTiffsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(intensityAct);
    viewMenu->addAction(zoomAct);

    filterMenu = menuBar()->addMenu(tr("&Filter"));
    filterMenu->addAction(removeLastAct);
    filterMenu->addAction(removeAllAct);
    filterMenu->addSeparator();
    filterMenu->addAction(corrFilterAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void CorrTrackWindow::setMovieRelatedItemsEnabled(const bool state)
{
    movieSlider->setEnabled(state);
    speedCBox->setEnabled(state);
    playButton->setEnabled(state);

    viewMenu->setEnabled(state);
    filterMenu->setEnabled(state);
    if (state && movieIsSet)
    {
        bool enable = analyser->movie->format == Movie::Format::Rawm;
        extractCurrentTiffAct->setEnabled(enable);
        extractTiffsAct->setEnabled(enable);
        view->show();
    }
    else
    {
        extractCurrentTiffAct->setEnabled(false);
        extractTiffsAct->setEnabled(false);
        view->hide();
    }
    testCorrAct->setEnabled(state);
    analyseAct->setEnabled(state);
    closeAct->setEnabled(state);

    updatePointsCtrlMenuItems();
}
