/**********************************************************************

	--- Dlgedit generated file ---

	File: CDDBSetupData.h
	Last generated: Thu Jan 1 13:42:16 1998

	DO NOT EDIT!!!  This file will be automatically
	regenerated by dlgedit.  All changes will be lost.

 *********************************************************************/

#ifndef CDDBSetupData_included
#define CDDBSetupData_included

#include <qwidget.h>
#include <qchkbox.h>
#include <qlabel.h>
#include <qlined.h>
#include <qlistbox.h>
#include <qpushbt.h>

class CDDBSetupData : public QWidget
{
    Q_OBJECT

public:

    CDDBSetupData
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~CDDBSetupData();

public slots:


protected slots:


protected:
    QLineEdit* basedir_edit;
    QLineEdit* submit_edit;
    QListBox* server_listbox;
    QPushButton* update_button;
    QLabel* current_server_label;
    QCheckBox* remote_cddb_cb;
    QPushButton* defaults_button;
    QPushButton* help_button;

};

#endif // CDDBSetupData_included
