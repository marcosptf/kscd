/*
 * This file was initialy generated by QTArchitect, but was heavily modified
 * afterwards.
 */

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include <qvalidator.h>

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <kdebug.h>
#include <klineedit.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <kurlrequester.h>

#include "../config.h"

#include "CDDBSetup.h"
#include "cddb.h"

CDDBSetup::CDDBSetup
(
    QWidget* parent,
    const char* name
)
    :
    CDDBSetupData( parent, name )
{

    connect(basedir_edit->lineEdit(),SIGNAL(textChanged(const QString &)),
            this,SLOT(basedir_changed(const QString &)));

    connect(update_button,SIGNAL(clicked()),
            this,SLOT(serverlist_update()));

    connect(defaults_button,SIGNAL(clicked()),
            this,SLOT(set_defaults()));

    connect(help_button,SIGNAL(clicked()),
            this,SLOT(help()));

    connect(server_listbox, SIGNAL(highlighted(int)), this,
            SLOT(set_current_server(int)));

    connect(submission_listbox,SIGNAL(highlighted(int)),
            this,SLOT(set_current_submission_address(int)));

    connect(remote_cddb_cb,SIGNAL(toggled(bool)),
            this,SLOT(enable_remote_cddb(bool)));

    cddb_timeout_ef->setEnabled(remote_cddb_cb->isChecked());

    connect(currentServerAddPB, SIGNAL(clicked()), this, SLOT(insertSL()));
    connect(currentServerDelPB, SIGNAL(clicked()), this, SLOT(removeSL()));

    connect(currentSubmitAddPB, SIGNAL(clicked()), this, SLOT(insertSUL()));
    connect(currentSubmitDelPB, SIGNAL(clicked()), this, SLOT(removeSUL()));

    QIntValidator *port_validator=new QIntValidator(this,"HTTP Port Validator");
    port_validator->setRange(0, INT_MAX );
    proxy_port_ef->setValidator(port_validator);

    proxy_port_ef->setEnabled(cddb_http_cb->isChecked());
    proxy_host_ef->setEnabled(cddb_http_cb->isChecked());
} // CDDBSetup

CDDBSetup::~CDDBSetup()
{
} // ~CDDBSetup

void
CDDBSetup::insertSL(void)
{
    uint i;

    current_server_string = currentServerLE->text();
    for(i = 0; i < server_listbox->count(); i++)
      {
        if(current_server_string == server_listbox->text(i))
          {
            server_listbox->setCurrentItem(i);
            server_listbox->centerCurrentItem();
            return;
          }
      }
    server_listbox->insertItem(current_server_string);
    server_listbox->setCurrentItem(server_listbox->count()-1);
    server_listbox->centerCurrentItem();
} // insertSL

void
CDDBSetup::removeSL(void)
{
    int it = server_listbox->currentItem();
    if(it == -1)
        return;
    server_listbox->removeItem(it);
} // removeSL

void
CDDBSetup::insertSUL(void)
{
    uint i;

    submitaddressstring = currentSubmitLE->text();
    for(i = 0; i < submission_listbox->count(); i++)
      {
        if(submitaddressstring == submission_listbox->text(i))
          {
            submission_listbox->setCurrentItem(i);
            submission_listbox->centerCurrentItem();
            return;
          }
      }
    submission_listbox->insertItem(submitaddressstring);
    submission_listbox->setCurrentItem(submission_listbox->count()-1);
    submission_listbox->centerCurrentItem();
} // insertSUL

void
CDDBSetup::removeSUL(void)
{
    int it = submission_listbox->currentItem();
    if(it == -1)
        return;
    submission_listbox->removeItem(it);
} // removeSUL


void
CDDBSetup::set_current_server(int i)
{
    current_server_string = server_listbox->text(i);
    currentServerLE->setText(current_server_string);
    emit updateCurrentServer();
} // set_current_server

void
CDDBSetup::set_current_submission_address(int i)
{
    submitaddressstring = submission_listbox->text(i);
    currentSubmitLE->setText(submitaddressstring);
} // set_current_submission_address

void
CDDBSetup::basedir_changed(const QString &str)
{
    basedirstring = str;
} // basedir_changed

void
CDDBSetup::enable_remote_cddb(bool state)
{
     cddb_timeout_ef->setEnabled(state);
} // enable_remote_cddb

void
CDDBSetup::help()
{
     kapp->invokeHelp( QString::null,"kscd" );
} // help

void
CDDBSetup::insertData(const QStringList& _serverlist,
                      const QStringList& _submitlist,
                      const QString& _basedir,
                      const QString& _submitaddress,
                      const QString& _current_server,
                      const bool&    auto_save_enabled,
                      const bool&    remote_enabled,
                      const unsigned short int& cddb_timeout,
                      const bool&    http_proxy_enabled,
                      const QString& http_proxy_host,
                      const unsigned short int& http_proxy_port)
{

    current_server_string = _current_server.copy();
    currentServerLE->setText(current_server_string);

    submitaddressstring = _submitaddress.copy();
    currentSubmitLE->setText(submitaddressstring);

    //submitlist.clear();
    //for(uint i = 0; i < _submitlist.count(); i++)
    //  {
    //    submitlist.append(*_submitlist.at(i));
    //  }
    submitlist = _submitlist;

    insertServerList(_serverlist);
//    server_listbox->setCurrentItem(_serverlist.find(_current_server.data()));
//    server_listbox->find(_current_server.data());
//    server_listbox->centerCurrentItem();

    kdDebug() << "check point: server_listbox->centerCurrentItem()\n" << endl;

    basedirstring = _basedir.copy();
    basedir_edit->lineEdit()->setText(basedirstring);
    enable_auto_save_cddb->setChecked(auto_save_enabled);
    remote_cddb_cb->setChecked(remote_enabled);

    char timeout_str[40];
    sprintf(timeout_str,"%d",cddb_timeout);
    cddb_timeout_ef->setText(timeout_str);

    cddb_http_cb->setChecked(http_proxy_enabled);
    proxy_host_ef->setText(http_proxy_host);
    char port_str[40];
    sprintf(port_str,"%d",http_proxy_port);
    proxy_port_ef->setText(port_str);
    kdDebug() << "check point: ::insertData{...;return;}\n" << endl;
} // insertData

void
CDDBSetup::set_defaults()
{
    server_listbox->clear();
    server_listbox->insertItem(DEFAULT_CDDB_SERVER, -1);
    // We should provide at least one entry
    // with HTTP protocol so people behind firewals can get servers list
    server_listbox->insertItem(DEFAULT_CDDBHTTP_SERVER, -1);
    server_listbox->repaint();
    server_listbox->setCurrentItem(0);

    basedirstring = KGlobal::dirs()->resourceDirs("cddb").last();
    basedir_edit->lineEdit()->setText(basedirstring);

    submission_listbox->clear();
    submission_listbox->insertItem(DEFAULT_SUBMIT_EMAIL,-1);
    submission_listbox->insertItem(DEFAULT_TEST_EMAIL, -1);
    submission_listbox->repaint();
    submission_listbox->setCurrentItem(0);

    remote_cddb_cb->setChecked(true);
    cddb_http_cb->setChecked(false);
    // Leave proxy host and port values unchanged, just disable them

    emit updateCurrentServer();
} // set_defaults

void
CDDBSetup::getData(QStringList& _serverlist,
                   QStringList& _submitlist,
                   QString& _basedir,
                   QString& _submitaddress,
                   QString& _current_server,
		   bool&    auto_save_enabled,
                   bool&    remote_enabled,
                   unsigned short int &cddb_timeout,
                   bool&    http_proxy_enabled,
                   QString  &http_proxy_host,
                   unsigned short int &http_proxy_port)
{
    uint i;

    _serverlist.clear();
    _submitlist.clear();
    for(i = 0; i < server_listbox->count();i++)
      {
        _serverlist.append( server_listbox->text(i) );
      }
    for(i = 0; i < submission_listbox->count(); i++)
      {
        _submitlist.append( submission_listbox->text(i) );
      }
    _basedir = basedirstring.copy();
    _submitaddress = submitaddressstring.copy();

    _current_server     = current_server_string.copy();
    remote_enabled      = remote_cddb_cb->isChecked();
    auto_save_enabled   = enable_auto_save_cddb->isChecked();
    cddb_timeout        = atoi(cddb_timeout_ef->text().ascii());
    http_proxy_enabled  = cddb_http_cb->isChecked();
    http_proxy_host     = proxy_host_ef->text();
    http_proxy_port     = atoi(proxy_port_ef->text().ascii());
} // getData

void
CDDBSetup::getCurrentServer(QString& ser)
{
    ser = current_server_string.copy();
} // getCurrentServer

void
CDDBSetup::serverlist_update()
{
    emit updateCDDBServers();
} // serverlist_update

void
CDDBSetup::insertServerList(const QStringList& list)
{
    QString current_server_string_backup;
    uint i;

    current_server_string_backup = current_server_string.copy();
    server_listbox->clear();
    submission_listbox->clear();

    bool have_email = false;
    bool have_srv   = false;

    for ( QStringList::ConstIterator it = list.begin();
          it != list.end();
          ++it )
      {
        char ser   [CDDB_FIELD_BUFFER_LEN];
        char por   [CDDB_FIELD_BUFFER_LEN];
        char proto [CDDB_FIELD_BUFFER_LEN];
        char extra [CDDB_FIELD_BUFFER_LEN];
        char email [CDDB_FIELD_BUFFER_LEN];

        QString srv=*it;
        sscanf(srv.ascii(),"%s %s %s %s",ser,proto,por,extra);
        CDDB::transport t=CDDB::decodeTransport(proto);
        if(t==CDDB::UNKNOWN)
          {
            continue;
          } else {
            if(t==CDDB::SMTP)
              {
                snprintf(email, sizeof(email),"%s@%s",extra,ser);
                have_email=true;
                submission_listbox->insertItem(email, -1);
              } else {
                have_srv=true;
                server_listbox->insertItem(srv, -1);
              }
          }
      }

    if(!have_srv)
      {
        server_listbox->insertItem(DEFAULT_CDDB_SERVER, -1);
        server_listbox->insertItem(DEFAULT_CDDBHTTP_SERVER, -1);
      }

    if(!have_email)
      {
        submission_listbox->insertStringList(submitlist);

//        submission_listbox->insertItem(DEFAULT_SUBMIT_EMAIL,-1);
//        submission_listbox->insertItem(DEFAULT_TEST_EMAIL, -1);
      }

    server_listbox->repaint();
    submission_listbox->repaint();

    bool found = 0;

    current_server_string = current_server_string_backup.copy();
    //current_server_string = currentServerLE->text();
    kdDebug() << "current_server_string: " << current_server_string << "\n" << endl;
    for(i = 0; i < server_listbox->count(); i++)
      {
        if(current_server_string == server_listbox->text(i))
          {
            server_listbox->setCurrentItem(i);
            server_listbox->centerCurrentItem();
            found = 1;
            break;
          }
      }

    if(!found)
      {
        server_listbox->setCurrentItem(0);
        server_listbox->centerCurrentItem();
      }

//    submitaddressstring = currentSubmitLE->text();
    for(i = 0; i < submission_listbox->count(); i++)
      {
        if(submitaddressstring == submission_listbox->text(i))
          {
            submission_listbox->setCurrentItem(i);
            submission_listbox->centerCurrentItem();
            found = 1;
            break;
          }
      }
    if(!found)
      {
        submission_listbox->setCurrentItem(0);
        submission_listbox->centerCurrentItem();
      }
//    server_listbox->setCurrentItem(0);
//    submission_listbox->setCurrentItem(0);
} // insertServerList

void
CDDBSetup::http_access_toggled(bool state)
{
    proxy_port_ef->setEnabled(state);
    proxy_host_ef->setEnabled(state);
} // http_access_toggled

#include "CDDBSetup.moc"
#include "CDDBSetupData.moc"
