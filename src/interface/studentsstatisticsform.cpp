/***************************************************************************
                          studentsstatisticform.cpp  -  description
                             -------------------
    begin                : March 25, 2006
    copyright            : (C) 2006 by Lalescu Liviu
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

#include "studentsstatisticsform.h"

#include "timetable_defs.h"
#include "timetable.h"

#include "fet.h"

#include <QString>
#include <QStringList>

#include <QHash>

#include <QProgressDialog>

#include "longtextmessagebox.h"

#include <QMessageBox>
#include <QApplication>

#include <QHeaderView>
#include <QTableWidget>

extern QApplication* pqapplication;

//QHash<QString, int> onlyExactHours; //for each students set, only the hours from each activity which has exactly the same set
//does not include related sets.

//QHash<QString, int> onlyExactActivities;

//QHash<QString, int> allStudentsSets; //int is type
QSet<QString> allStudentsSets;

QHash<QString, int> allHours; //for each students set, only the hours from each activity which has exactly the same set
//does not include related sets.

QHash<QString, int> allActivities;

QSet<QString> related; //related to current set

QSet<QString> relatedSubgroups;
QSet<QString> relatedGroups;
QSet<QString> relatedYears;

StudentsStatisticsForm::StudentsStatisticsForm(QWidget* parent): QDialog(parent)
{
	setupUi(this);
	
	closeButton->setDefault(true);

	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	centerWidgetOnScreen(this);
	restoreFETDialogGeometry(this);
	
	connect(showYearsCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));
	connect(showGroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));
	connect(showSubgroupsCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));

	connect(showCompleteStructureCheckBox, SIGNAL(toggled(bool)), this, SLOT(checkBoxesModified()));

	allStudentsSets.clear();
	foreach(StudentsYear* year, gt.rules.yearsList){
		allStudentsSets.insert(year->name);
		foreach(StudentsGroup* group, year->groupsList){
			allStudentsSets.insert(group->name);
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				allStudentsSets.insert(subgroup->name);
			}
		}
	}
	///////////
	
	///////////
	allHours.clear();
	allActivities.clear();	
	
	QProgressDialog progress(this);
	progress.setWindowTitle(tr("Computing students statistics", "Title of a progress dialog"));
	progress.setLabelText(tr("Computing ... please wait"));
	progress.setRange(0, allStudentsSets.count());
	progress.setModal(true);
						
	int ttt=0;
							
	foreach(QString set, allStudentsSets){
		progress.setValue(ttt);
		//pqapplication->processEvents();
		if(progress.wasCanceled()){
			QMessageBox::information(this, tr("FET information"), tr("Canceled"));
			showYearsCheckBox->setDisabled(true);
			showGroupsCheckBox->setDisabled(true);
			showSubgroupsCheckBox->setDisabled(true);
			showCompleteStructureCheckBox->setDisabled(true);
			return;
		}
		ttt++;
	
		relatedSubgroups.clear();
		
		relatedGroups.clear();
		
		relatedYears.clear();
		
		related.clear();
		
		foreach(StudentsYear* year, gt.rules.yearsList){
			bool y=false;
			if(year->name==set)
				y=true;
			if(y)
				relatedYears.insert(year->name);
			foreach(StudentsGroup* group, year->groupsList){
				bool g=false;
				if(group->name==set)
					g=true;
				if(y || g)
					relatedGroups.insert(group->name);
				foreach(StudentsSubgroup* subgroup, group->subgroupsList){
					if(y || g || subgroup->name==set)
						relatedSubgroups.insert(subgroup->name);
				}
			}
		}
		
		foreach(StudentsYear* year, gt.rules.yearsList){
			if(relatedYears.contains(year->name))
				related.insert(year->name);
			foreach(StudentsGroup* group, year->groupsList){
				if(relatedGroups.contains(group->name)){
					related.insert(year->name);
					related.insert(group->name);
				}
				foreach(StudentsSubgroup* subgroup, group->subgroupsList){
					if(relatedSubgroups.contains(subgroup->name)){
						related.insert(year->name);
						related.insert(group->name);
						related.insert(subgroup->name);
					}
				}
			}
		}
		
		int nh=0;
		int na=0;
		
		foreach(Activity* act, gt.rules.activitiesList) if(act->active){
			foreach(QString _students, act->studentsNames){
				if(related.contains(_students)){
					nh += act->duration;
					na ++;
					
					break;
				}
			}
		}
		
		allHours.insert(set, nh);
		allActivities.insert(set, na);
	}
	////////////

	progress.setValue(allStudentsSets.count());
	
	checkBoxesModified();	
}

StudentsStatisticsForm::~StudentsStatisticsForm()
{
	saveFETDialogGeometry(this);
}

void StudentsStatisticsForm::checkBoxesModified()
{
	bool complete=showCompleteStructureCheckBox->isChecked();
	
	QSet<QString> studs;

	studs.clear();
	int nStudentsSets=0;
	foreach(StudentsYear* year, gt.rules.yearsList){
		bool sy=true;
		if(!complete){
			if(studs.contains(year->name))
				sy=false;
			else
				studs.insert(year->name);
		}
		if(showYearsCheckBox->isChecked() && sy)
			nStudentsSets++;
		foreach(StudentsGroup* group, year->groupsList){
			bool sg=true;
			if(!complete){
				if(studs.contains(group->name))
					sg=false;
				else
					studs.insert(group->name);
			}
			if(showGroupsCheckBox->isChecked() && sg)
				nStudentsSets++;
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				bool ss=true;
				if(!complete){
					if(studs.contains(subgroup->name))
						ss=false;
					else
						studs.insert(subgroup->name);
				}
				if(showSubgroupsCheckBox->isChecked() && ss)
					nStudentsSets++;
			}
		}
	}
	
	tableWidget->clear();
	tableWidget->setColumnCount(3);
	tableWidget->setRowCount(nStudentsSets);
	
	QStringList columns;
	columns<<tr("Students set");
	columns<<tr("No. of activities");
	columns<<tr("Duration");
	
	tableWidget->setHorizontalHeaderLabels(columns);
	
	studs.clear();
	
	int currentStudentsSet=-1;
	foreach(StudentsYear* year, gt.rules.yearsList){
		bool sy=true;
		if(!complete){
			if(studs.contains(year->name))
				sy=false;
			else
				studs.insert(year->name);
		}

		if(showYearsCheckBox->isChecked() && sy){
			currentStudentsSet++;		
			insertStudentsSet(year, currentStudentsSet);
		}
				
		foreach(StudentsGroup* group, year->groupsList){
			bool sg=true;
			if(!complete){
				if(studs.contains(group->name))
					sg=false;
				else
					studs.insert(group->name);
			}

			if(showGroupsCheckBox->isChecked() && sg){
				currentStudentsSet++;
				insertStudentsSet(group, currentStudentsSet);
			}
			
			foreach(StudentsSubgroup* subgroup, group->subgroupsList){
				bool ss=true;
				if(!complete){
					if(studs.contains(subgroup->name))
						ss=false;
					else
						studs.insert(subgroup->name);
				}

				if(showSubgroupsCheckBox->isChecked() && ss){
					currentStudentsSet++;
					insertStudentsSet(subgroup, currentStudentsSet);
				}
			}	
		}
	}
	
	tableWidget->resizeColumnsToContents();
	tableWidget->resizeRowsToContents();
}

void StudentsStatisticsForm::insertStudentsSet(StudentsSet* set, int row)
{
	QTableWidgetItem* newItem=new QTableWidgetItem(set->name);
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tableWidget->setItem(row, 0, newItem);

	int nSubActivities=0;
	int nHours=0;
	
	if(allHours.contains(set->name))
		nHours=allHours.value(set->name);
	else{
		assert(0);
	}
		
	if(allActivities.contains(set->name))
		nSubActivities=allActivities.value(set->name);
	else
		assert(0);
		
	newItem=new QTableWidgetItem(CustomFETString::number(nSubActivities));
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tableWidget->setItem(row, 1, newItem);

	newItem=new QTableWidgetItem(CustomFETString::number(nHours));
	newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	tableWidget->setItem(row, 2, newItem);
}

void StudentsStatisticsForm::on_helpPushButton_clicked()
{
	QString s;
	
	s+=tr("The check boxes '%1', '%2' and '%3': they permit you to show/hide information related to years, groups or subgroups")
	 .arg(tr("Show years"))
	 .arg(tr("Show groups"))
	 .arg(tr("Show subgroups"));
	
	s+="\n\n";
	
	s+=tr("The check box '%1': it has effect only if you have overlapping groups/years, and means that FET will show the complete tree structure"
	 ", even if that means that some subgroups/groups will appear twice or more in the table, with the same information."
	 " For instance, if you have year Y1, groups G1 and G2, subgroups S1, S2, S3, with structure: Y1 (G1 (S1, S2), G2 (S1, S3)),"
	 " S1 will appear twice in the table with the same information attached").arg(tr("Show duplicates"));
	
	LongTextMessageBox::largeInformation(this, tr("FET help"), s);
}
