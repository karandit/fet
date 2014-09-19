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
#include "buildingsform.h"
#include "addbuildingform.h"
#include "modifybuildingform.h"

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

BuildingsForm::BuildingsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	currentBuildingTextEdit->setReadOnly(true);

	modifyBuildingPushButton->setDefault(true);

	buildingsListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(addBuildingPushButton, SIGNAL(clicked()), this, SLOT(addBuilding()));
	connect(removeBuildingPushButton, SIGNAL(clicked()), this, SLOT(removeBuilding()));
	connect(buildingsListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(buildingChanged(int)));
	connect(closePushButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(modifyBuildingPushButton, SIGNAL(clicked()), this, SLOT(modifyBuilding()));
	connect(sortBuildingsPushButton, SIGNAL(clicked()), this, SLOT(sortBuildings()));
	connect(buildingsListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(modifyBuilding()));

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	//restore splitter state
	QSettings settings(COMPANY, PROGRAM);
	if(settings.contains(this->metaObject()->className()+QString("/splitter-state")))
		splitter->restoreState(settings.value(this->metaObject()->className()+QString("/splitter-state")).toByteArray());
	
	this->filterChanged();
}

BuildingsForm::~BuildingsForm()
{
	saveFETDialogGeometry(this);
	//save splitter state
	QSettings settings(COMPANY, PROGRAM);
	settings.setValue(this->metaObject()->className()+QString("/splitter-state"), splitter->saveState());
}

bool BuildingsForm::filterOk(Building* bu)
{
	Q_UNUSED(bu);

	bool ok=true;

	return ok;
}

void BuildingsForm::filterChanged()
{
	QString s;
	buildingsListWidget->clear();
	visibleBuildingsList.clear();
	for(int i=0; i<gt.rules.buildingsList.size(); i++){
		Building* bu=gt.rules.buildingsList[i];
		if(this->filterOk(bu)){
			s=bu->name;
			visibleBuildingsList.append(bu);
			buildingsListWidget->addItem(s);
		}
	}
	
	if(buildingsListWidget->count()>0)
		buildingsListWidget->setCurrentRow(0);
	else
		buildingChanged(-1);
}

void BuildingsForm::addBuilding()
{
	AddBuildingForm addBuildingForm(this);
	setParentAndOtherThings(&addBuildingForm, this);
	addBuildingForm.exec();
	
	filterChanged();
	
	buildingsListWidget->setCurrentRow(buildingsListWidget->count()-1);
}

void BuildingsForm::removeBuilding()
{
	int ind=buildingsListWidget->currentRow();
	if(ind<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected building"));
		return;
	}
	
	Building* bu=visibleBuildingsList.at(ind);
	assert(bu!=NULL);

	if(QMessageBox::warning( this, tr("FET"),
		tr("Are you sure you want to delete this building?"),
		tr("Yes"), tr("No"), 0, 0, 1 ) == 1)
		return;
		
	bool tmp=gt.rules.removeBuilding(bu->name);
	assert(tmp);
	
	visibleBuildingsList.removeAt(ind);
	buildingsListWidget->setCurrentRow(-1);
	QListWidgetItem* item=buildingsListWidget->takeItem(ind);
	delete item;
	
	if(ind>=buildingsListWidget->count())
		ind=buildingsListWidget->count()-1;
	if(ind>=0)
		buildingsListWidget->setCurrentRow(ind);
	else
		currentBuildingTextEdit->setPlainText(QString(""));
}

void BuildingsForm::buildingChanged(int index)
{
	if(index<0){
		currentBuildingTextEdit->setPlainText("");
		return;
	}

	QString s;
	Building* building=visibleBuildingsList.at(index);

	assert(building!=NULL);
	s=building->getDetailedDescriptionWithConstraints(gt.rules);
	currentBuildingTextEdit->setPlainText(s);
}

void BuildingsForm::sortBuildings()
{
	gt.rules.sortBuildingsAlphabetically();

	filterChanged();
}

void BuildingsForm::modifyBuilding()
{
	int valv=buildingsListWidget->verticalScrollBar()->value();
	int valh=buildingsListWidget->horizontalScrollBar()->value();

	int ci=buildingsListWidget->currentRow();
	if(ci<0){
		QMessageBox::information(this, tr("FET information"), tr("Invalid selected building"));
		return;
	}
	
	Building* bu=visibleBuildingsList.at(ci);
	ModifyBuildingForm form(this, bu->name);
	setParentAndOtherThings(&form, this);
	form.exec();

	filterChanged();
	
	buildingsListWidget->verticalScrollBar()->setValue(valv);
	buildingsListWidget->horizontalScrollBar()->setValue(valh);

	if(ci>=buildingsListWidget->count())
		ci=buildingsListWidget->count()-1;

	if(ci>=0)
		buildingsListWidget->setCurrentRow(ci);
}
