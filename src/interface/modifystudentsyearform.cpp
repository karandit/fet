/***************************************************************************
                          modifystudentsyearform.cpp  -  description
                             -------------------
    begin                : Feb 8, 2005
    copyright            : (C) 2005 by Lalescu Liviu
    email                : Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "modifystudentsyearform.h"

#include <QMessageBox>

ModifyStudentsYearForm::ModifyStudentsYearForm(QWidget* parent, const QString& initialYearName, int initialNumberOfStudents): QDialog(parent)
{
	setupUi(this);
	
	okPushButton->setDefault(true);

	connect(okPushButton, SIGNAL(clicked()), this, SLOT(ok()));
	connect(cancelPushButton, SIGNAL(clicked()), this, SLOT(cancel()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	numberSpinBox->setMaximum(MAX_ROOM_CAPACITY);
	numberSpinBox->setMinimum(0);
	numberSpinBox->setValue(0);

	this->_initialYearName=initialYearName;
	this->_initialNumberOfStudents=initialNumberOfStudents;
	numberSpinBox->setValue(initialNumberOfStudents);
	nameLineEdit->setText(initialYearName);
	nameLineEdit->selectAll();
	nameLineEdit->setFocus();
}

ModifyStudentsYearForm::~ModifyStudentsYearForm()
{
	saveFETDialogGeometry(this);
}

void ModifyStudentsYearForm::cancel()
{
	this->close();
}

void ModifyStudentsYearForm::ok()
{
	if(nameLineEdit->text().isEmpty()){
		QMessageBox::information(this, tr("FET information"), tr("Incorrect name"));
		return;
	}
	if(this->_initialYearName!=nameLineEdit->text() && gt.rules.searchStudentsSet(nameLineEdit->text())!=NULL){
		QMessageBox::information(this, tr("FET information"), tr("Name existing - please choose another"));

		nameLineEdit->selectAll();
		nameLineEdit->setFocus();

		return;
	}
	bool t=gt.rules.modifyStudentsSet(this->_initialYearName, nameLineEdit->text(), numberSpinBox->value());
	assert(t);

	this->close();
}
