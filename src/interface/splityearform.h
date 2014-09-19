/***************************************************************************
                          splityearform.h  -  description
                             -------------------
    begin                : 10 Aug 2007
    copyright            : (C) 2007 by Lalescu Liviu
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

#ifndef SPLITYEARFORM_H
#define SPLITYEARFORM_H

#include <QString>

#include "ui_splityearform_template.h"

#include "timetable_defs.h"
#include "timetable.h"
#include "fet.h"

class SplitYearForm : public QDialog, Ui::SplitYearForm_template  {
	Q_OBJECT

private:
	QString _sep;

	int _nCategories;
	
	int _nDiv1;
	int _nDiv2;
	int _nDiv3;
	int _nDiv4;
	
	QString _cat1div1;
	QString _cat1div2;
	QString _cat1div3;
	QString _cat1div4;
	QString _cat1div5;
	QString _cat1div6;
	QString _cat1div7;
	QString _cat1div8;
	QString _cat1div9;
	QString _cat1div10;
	QString _cat1div11;
	QString _cat1div12;
	QString _cat1div13;
	QString _cat1div14;
	QString _cat1div15;
	QString _cat1div16;

	QString _cat2div1;
	QString _cat2div2;
	QString _cat2div3;
	QString _cat2div4;
	QString _cat2div5;
	QString _cat2div6;
	QString _cat2div7;
	QString _cat2div8;
	QString _cat2div9;
	QString _cat2div10;
	QString _cat2div11;
	QString _cat2div12;

	QString _cat3div1;
	QString _cat3div2;
	QString _cat3div3;
	QString _cat3div4;
	QString _cat3div5;
	QString _cat3div6;
	
	QString _cat4div1;
	QString _cat4div2;
	QString _cat4div3;
	QString _cat4div4;
	QString _cat4div5;
	QString _cat4div6;
	
public:
	QString year;
	
	SplitYearForm(QWidget* parent, const QString& _year);
	~SplitYearForm();
	
public slots:
	void ok();
	void numberOfCategoriesChanged();
	void category1Changed();
	void category2Changed();
	void category3Changed();
	void category4Changed();
	
	void help();
	void reset();
};

#endif
