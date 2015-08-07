//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2003 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "timetable_defs.h"
#include "fet.h"
#include "roomsform.h"
#include "addroomform.h"
#include "modifyroomform.h"

#include <QMessageBox>

#include <QListWidget>
#include <QScrollBar>
#include <QAbstractItemView>

#include <QSplitter>
#include <QSettings>
#include <QObject>
#include <QMetaObject>

extern const QString COMPANY;
extern const QString PROGRAM;

RoomsForm::RoomsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	currentRoomTextEdit->setReadOnly(true);
	
	modifyRoomPushButton->setDefault(true);
	
	roomsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(addRoomPushButton, SIGNAL(clicked()), this, SLOT(addRoom()));
	connect(removeRoomPushButton, SIGNAL(clicked()), this, SLOT(removeRoom()));
	connect(roomsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(roomChanged(int)));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(modifyRoomPushButton, SIGNAL(clicked()), this, SLOT(modifyRoom()));
	connect(sortRoomsPushButton, SIGNAL(clicked()), this, SLOT(sortRooms()));
	connect(roomsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyRoom()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	
	this->filterChanged();
}


RoomsForm::~RoomsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

bool RoomsForm::filterOk(Room* rm)
{
	Q_UNUSED(rm);

	bool ok=true;
	return ok;
}

void RoomsForm::filterChanged()
{
	QString s;
	roomsListWidget->clear();
	visibleRoomsList.clear();
	for(int i=0; i<gt.rules.roomsList.size(); i++){
		Room* rm=gt.rules.roomsList[i];
		if(this->filterOk(rm)){
			s=rm->getDescription();
			visibleRoomsList.append(rm);
			roomsListWidget->addItem(s);
		}
	}
	
	if(roomsListWidget->count()>0)
		roomsListWidget->setCurrentRow(0);
	else
		roomChanged(-1);
}

void RoomsForm::addRoom()
{
	AddRoomForm addRoomForm(this);
	setParentAndOtherThings(&addRoomForm, this);
	addRoomForm.exec();
	
	filterChanged();
	
	roomsListWidget->setCurrentRow(roomsListWidget->count()-1);
}

void RoomsForm::removeRoom()
{
	int ind=roomsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=visibleRoomsList.at(ind);
	assert(rm!=NULL);

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete this room and all related constraints?"),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;

	bool tmp=gt.rules.removeRoom(rm->name);
	assert(tmp);

	visibleRoomsList.removeAt(ind);
	roomsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=roomsListWidget->takeItem(ind);
	delete item;

	if(ind>=roomsListWidget->count())
		ind=roomsListWidget->count()-1;
	if(ind>=0)
		roomsListWidget->setCurrentRow(ind);
	else
		currentRoomTextEdit->setPlainText(QString(""));
}

void RoomsForm::roomChanged(int index)
{
	if(index<0){
		currentRoomTextEdit->setPlainText("");
		return;
	}

	QString s;
	Room* room=visibleRoomsList.at(index);

	assert(room!=NULL);
	s=room->getDetailedDescriptionWithConstraints(gt.rules);
	currentRoomTextEdit->setPlainText(s);
}

void RoomsForm::sortRooms()
{
	gt.rules.sortRoomsAlphabetically();

	filterChanged();
}

void RoomsForm::modifyRoom()
{
	int valv=roomsListWidget->verticalScrollBar()->value();
	int valh=roomsListWidget->horizontalScrollBar()->value();

	int ci=roomsListWidget->currentRow();
	if(ci<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected room"));
		return;
	}
	
	Room* rm=visibleRoomsList.at(ci);
	ModifyRoomForm form(this, rm->name, rm->building, rm->capacity);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	roomsListWidget->verticalScrollBar()->setValue(valv);
	roomsListWidget->horizontalScrollBar()->setValue(valh);

	if(ci>=roomsListWidget->count())
		ci=roomsListWidget->count()-1;

	if(ci>=0)
		roomsListWidget->setCurrentRow(ci);
}
