/*
File solution.h
*/

/*
Copyright 2002, 2003 Lalescu Liviu.

This file is part of FET.

FET is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

FET is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FET; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef SOLUTION_H
#define SOLUTION_H

#include "timetable_defs.h"

#include "matrix.h"

#include <QString>
#include <QList>
#include <QTextStream>

class Rules;

/**
This class represents a solution (time and space allocation for the activities).
*/
class Solution{
public:
	QList<double> conflictsWeightList;
	QList<QString> conflictsDescriptionList;
	double conflictsTotal;
	
	bool teachersMatrixReady;
	bool subgroupsMatrixReady;
	bool roomsMatrixReady;
	
	int nPlacedActivities;

	/*
	You will need to set this to true if altering the times array values.
	The conflicts calculating routine will reset this to false
	at the first teachers matrix and subgroups matrix calculation.
	*/
	bool changedForMatrixCalculation;

	/**
	This array represents every activity's start time
	(time is a unified representation of hour and day,
	stored as an integer value). We have a special value here:
	UNALLOCATED_TIME, which is a large number.
	*/
	int times[MAX_ACTIVITIES];
	
	int rooms[MAX_ACTIVITIES];

	/**
	Fitness; it is calculated only at the initialization or
	at the modification.
	Important assumption: the rules have to ramain the same;
	otherwise the user has to reset this value to -1
	*/
	double _fitness;

	/**
	Assignment method. We need to have access to the Rules instantiation
	to know the number of activities.
	*/
	void copy(Rules& r, Solution& c);

	/**
	Initializes, marking all activities as unscheduled (time)
	*/
	void init(Rules& r);

	/**
	Marks the starting time of all the activities as undefined
	(all activities are unallocated).
	*/
	void makeUnallocated(Rules& r);

	/**
	ATTENTION: if the rules change, the user has to reset _fitness to -1
	<p>
	If conflictsString is not null, then this function will
	append at this string an explanation of the conflicts.
	*/
	double fitness(Rules& r, QString* conflictsString=NULL);

	void getTeachersTimetable(Rules& r, Matrix3D<int>& a, Matrix3D<QList<int> >& b);
	//return value is the number of conflicts, which must be 0

	void getSubgroupsTimetable(Rules& r, Matrix3D<int>& a);
	//return value is the number of conflicts, which must be 0

	void getRoomsTimetable(Rules& r, Matrix3D<int>& a);
	//return value is the number of conflicts, which must be 0

	int getSubgroupsMatrix(Rules& r, Matrix3D<int>& a);

	int getTeachersMatrix(Rules& r, Matrix3D<int>& a);

	int getRoomsMatrix(Rules& r, Matrix3D<int>& a);
};

#endif
