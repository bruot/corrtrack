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


#pragma once

#include <QMenu>
#include <QStyle>
#include <QProxyStyle>
#include <QStyleOptionMenuItem>

class NoMenuIconsStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;

    void drawControl(QStyle::ControlElement element, const QStyleOption *opt,
                     QPainter *p, const QWidget *w) const override;
};
