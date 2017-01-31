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


#include <QPushButton>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include "okcanceldialog.h"


OKCancelDialog::OKCancelDialog(QWidget *parent)
    : QDialog(parent),
      okButton{new QPushButton(this)},
      cancelButton{new QPushButton(this)}
{
    okButton->setText("OK");
    cancelButton->setText("Cancel");

    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
}

void OKCancelDialog::setLayout(QLayout *layout)
{
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(layout);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    QDialog::setLayout(mainLayout);

}

void OKCancelDialog::ok()
{
    accept();
}

void OKCancelDialog::cancel()
{
    reject();
}
