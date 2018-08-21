/*
 * This file is part of the particle tracking software CorrTrack.
 *
 * Copyright 2018 Nicolas Bruot and CNRS
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


#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QStyleOptionMenuItem>
#include <QWidget>
#include <QProxyStyle>
#include <QRect>

#include "nomenuiconsstyle.h"


void NoMenuIconsStyle::drawControl(ControlElement element, const QStyleOption *opt,
                                   QPainter *p, const QWidget *w) const
{
    QStyleOptionMenuItem myMenuOption;
    const QStyleOptionMenuItem *menuOption =
            qstyleoption_cast<const QStyleOptionMenuItem *>(opt);
    if (menuOption) {
        const int width = pixelMetric(PM_SmallIconSize);
        myMenuOption = *menuOption;
        QRect r(myMenuOption.rect);
        r.setLeft(-width);
        myMenuOption.rect = r;
    }
    QProxyStyle::drawControl(element, &myMenuOption, p, w);
}
