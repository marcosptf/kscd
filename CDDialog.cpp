/**********************************************************************

	--- Dlgedit generated file ---

	File: CDDialog.cpp
	Last generated: Sun Dec 28 19:48:13 1997

 *********************************************************************/

#include "CDDialog.h"
#include "CDDialog.moc"
#include "CDDialogData.moc"
#include "inexact.h"
#include "version.h"

#include <unistd.h>
#include <qdatetm.h> 
#include <qtstream.h> 
#include <qfile.h>
#include <qdir.h>
#include <qfileinf.h> 
#include <qmsgbox.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <math.h>

#include <kapp.h>

#define Inherited CDDialogData

QTime framestoTime(int frames);
extern void cddb_decode(QString& str);
extern void cddb_encode(QString& str, QStrList &returnlist);
extern void cddb_playlist_encode(QStrList& list,QString& playstr);
extern bool cddb_playlist_decode(QStrList& list, QString& str);

extern KApplication 	*mykapp;

CDDialog::CDDialog
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name )
{
	setCaption( "CD Database Editor" );

	connect(listbox,     SIGNAL(highlighted(int)),this,SLOT(titleselected(int)));
	connect(listbox,     SIGNAL(selected(int)),this,SLOT(play(int)));
	connect(trackedit,   SIGNAL(returnPressed()) ,this,SLOT(trackchanged()));
	connect(save_button, SIGNAL(clicked())       ,this,SLOT(save()));
	connect(upload_button, SIGNAL(clicked())       ,this,SLOT(upload()));
	connect(ok_button, SIGNAL(clicked())       ,this,SLOT(ok()));
	connect(load_button, SIGNAL(clicked())       ,this,SLOT(load()));
	connect(ext_info_title_button, SIGNAL(clicked()) ,this,SLOT(extITB()));
	connect(ext_info_button, SIGNAL(clicked())       ,this,SLOT(extIB()));
	connect(titleedit,   SIGNAL(textChanged(const char*)),
		             this,SLOT(titlechanged(const char*)));
	ext_info_button->setEnabled(false);

        setFixedSize(width(),height());
	
}


CDDialog::~CDDialog()
{
}


void CDDialog::closeEvent(QCloseEvent* ){

}

void CDDialog::ok(){

  emit dialog_done();


}


void CDDialog::play(int i){

  emit play_signal(i);

}


void CDDialog::setData( 
		       struct cdinfo_wm *cd, 
		       QStrList& tracktitlelist,
		       QStrList& extlist, 
		       QStrList& discidl, 
		       QString& _xmcd_data, 
		       QString& cat,
		       int& rev,
		       QStrList& _playlist,
		       QStrList& _pathlist,
		       QString& _mailcmd
		       ){

    int ntr, etr;

    ext_list 	= extlist;
    track_list 	= tracktitlelist;
    xmcd_data   = _xmcd_data.copy();
    cdinfo 	= cd;
    category 	= cat.copy(); 
    discidlist  = discidl;
    revision    = rev;
    playlist	= _playlist;
    pathlist    = _pathlist;
    mailcmd	= _mailcmd;

    ntr = tracktitlelist.count();
    etr = extlist.count();
    if( ntr > 0 )
      titleedit->setText(tracktitlelist.at(0));
    else{
      if(cdinfo->ntracks > 0){
	titleedit->setText("Unknown");
      }
      else {
	titleedit->setText("");
      }
    }

   QString idstr;

   idstr.sprintf("%08x",cd->magicID);
   idstr = category + (QString("\n") + idstr);

   if(cdinfo->ntracks > 0)
     disc_id_label->setText(idstr);
   else
     disc_id_label->setText("");

   QTime   dl;
   dl 	=  dl.addSecs(cdinfo->length);

   QString temp2;
   if(dl.hour() > 0)
     temp2.sprintf("Total Time:\n%02d:%02d:%02d",dl.hour(),dl.minute(),dl.second());
   else
     temp2.sprintf("Total Time:\n %02d:%02d",dl.minute(),dl.second());
   total_time_label->setText(temp2);

   QString 	fmt;
   QTime 	dml;

   listbox->setAutoUpdate(false);
   listbox->clear();
   for(int i = 1; i <= cdinfo->ntracks; i++){

     dml = framestoTime(cdinfo->cddbtoc[i].absframe - cdinfo->cddbtoc[i-1].absframe);

     if((ntr >=  i) && (ntr > 0)){
       fmt.sprintf("%02d   %02d:%02d   %s",i,
		   dml.minute(),dml.second(),tracktitlelist.at(i));
     }
     else
       fmt.sprintf("%02d   %02d:%02d",i,dml.minute(),dml.second());
     
     listbox->insertItem(fmt.data(),-1);

   }

   listbox->setAutoUpdate(true);
   listbox->repaint();

   QString str;
   cddb_playlist_encode(playlist,str);
   progseq_edit->setText(str.data());

}

void CDDialog::extIB(){
  
  int item;
  item = listbox->currentItem();
  if(item == -1)
    return;

  InexactDialog *dialog;
  dialog = new InexactDialog(0,"dialog",false);
  dialog->setTitle("Use this Editor to annotate this track");

  dialog->insertText(ext_list.at(item + 1));
  
  if(dialog->exec() != QDialog::Accepted){
    delete dialog;
    return;
  }
  
  QString text;
  dialog->getSelection(text);

  ext_list.insert( item, text );
  ext_list.remove( item + 1 );

  delete dialog;

}

void CDDialog::extITB(){

  
  InexactDialog *dialog;
  dialog = new InexactDialog(0,"dialog",false);
  dialog->insertText(ext_list.at(0));
  dialog->setTitle("Use this Editor to annotate the title");
  
  if(dialog->exec() != QDialog::Accepted){
    delete dialog;
    return;
  }
  
  QString text;
  dialog->getSelection(text);

  ext_list.insert( 0 , text );
  ext_list.remove( 1 );

  delete dialog;

}

void CDDialog::titleselected(int i){

  ext_info_button->setEnabled(true);
  if(i + 1 < (int)track_list.count())
    trackedit->setText(track_list.at(i+1));

}

void CDDialog::trackchanged(){


  int i;

  i = listbox->currentItem();
  if (i == -1)
    return;
  
  QTime dml = framestoTime(cdinfo->cddbtoc[i+1].absframe - cdinfo->cddbtoc[i].absframe);

  QString fmt;

  fmt.sprintf("%02d   %02d:%02d   %s",i+1,dml.minute(),dml.second(),trackedit->text());

  track_list.insert(i+1,trackedit->text());
  track_list.remove(i+2);

  listbox->setAutoUpdate(false);

  listbox->insertItem(fmt.data(),i);
  listbox->removeItem(i+1);
  listbox->setAutoUpdate(true);
  listbox->repaint();
  if ( i <(int) listbox->count() -1 ){
    listbox->setCurrentItem(i+1);
    listbox->centerCurrentItem();
  }
  
}


QTime framestoTime(int _frames){

  QTime 	dml;
  double 	frames;
  double 	dsecs;
  int 		secs;
  double 	ip;

  frames = (double) _frames;
  dml.setHMS(0,0,0);

  dsecs = frames/ (75.0);

  if(modf(dsecs,&ip) >= 0.5){
    ip = ip + 1.0;
  }
  secs = (int) ip;

  dml = dml.addSecs(secs);
  return dml;
}

void CDDialog::titlechanged(const char* t){


  track_list.remove((uint)0);
  track_list.insert(0,t);


}

void CDDialog::upload(){

  if(!checkit())
    return;

  switch( QMessageBox::information( this,"Message",
      "The submission you are about to make will go to the test server\n"\
      "cddb-test@cddb.com. This is because this is the first release of\n"\
      "the cddb addition to Kscd and we need to avoid corruption\n"\
      "of the remote cddb data bases due to possible bugs in this first\n"\
      "release of the cddb addition to Kscd.\n"\
      "Since we need about a 100 error free test submissions\n"\
      "before we will be granted write acces to the 'true' cddb databases, I would\n"\
      "like you to upload as many test submissions as possible. You don't need to\n"\
      "try to find a CD that is not in the database ( thought that would be helpful )\n"\
      "It suffices to submit existing entries with Kscd.\n"\
      "After submission you should receive a mail notifying you of success or\n"\
      "or failure of your submission. Should you receive a failure notice\n"\
      "please forward the failure report to me: <wuebben@kde.org>\n"\
      "Check back often for the availablity of a fully enabled version of kscd.\n"\
      "Thank you for your support.\n",
				    "OK",
				    "Cancel",
				    0,
				    1 ) ){

  case 0: 
    break;
  case 1: 
    return;
    break;
  }




  InexactDialog *dialog;

  dialog = new InexactDialog(0,"Dialog",true);


  QStrList catlist;
  QString submitcat;

  for(int i = 0; i < (int)pathlist.count();i++){
    QString temp;
    getCategoryFromPathName(pathlist.at(i),temp);
    catlist.append(temp);
  }

  dialog->insertList(catlist);
  dialog->setErrorString("Please select a Category or press Cancel");
  dialog->setTitle("To which category does the CD belong?");
  if(dialog->exec() != QDialog::Accepted){
    delete dialog;
    return;
  }

  dialog->getSelection(submitcat);
  delete dialog;

  QString tempfile;
  tempfile = tempfile.sprintf("%s",tmpnam(0L));

  save_cddb_entry(tempfile,true);

  mykapp->processEvents();
  mykapp->flushX();


  QString subject;
  subject.sprintf("cddb %s %08x",submitcat.data(),cdinfo->magicID);

  FILE* mailpipe;

  QString formatstr;
  formatstr = mailcmd + " cddb-test@cddb.cddb.com";


  QString cmd;
  //  cmd = cmd.sprintf("mail -s \"%s\" cddb-test@cddb.cddb.com",subject.data());
  cmd = cmd.sprintf(formatstr.data(),subject.data());
		    

  mailpipe = popen(cmd.data(),"w");

  if(mailpipe == NULL){
    QString str;
    str.sprintf("Could not pipe contents into:\n %s",cmd.data());

    QMessageBox::critical(
			  this,
			  "Kscd",
			  str.data()
			  );
    pclose(mailpipe);
    return;
    
  }

  QFile file(tempfile);
  file.open(IO_ReadOnly);
  QTextStream ti(&file);

  QTextStream to(mailpipe,IO_WriteOnly );

  QString s;
  while ( !ti.eof() ) {
    s = ti.readLine();
    if(!ti.eof()){
      to << s.data() << '\n';
    }
  }	  

  pclose(mailpipe);

  file.close();

  unlink(tempfile.data());
  printf("DONE SENDING\n");
}


void CDDialog::getCategoryFromPathName(char* pathname, QString& _category){
  
  QString path = pathname;
  path = path.stripWhiteSpace();

  while(path.right(1) == QString("/")){
    path = path.left(path.length() - 1);
  }

  int pos = 0;
  pos  = path.findRev("/",-1,true);
  if(pos == -1)
    _category = path.copy();
  else
    _category = path.mid(pos+1,path.length());

  _category.detach();

}

void CDDialog::save(){

  if(!checkit())
    return;

  QString path;

  InexactDialog *dialog;

  dialog = new InexactDialog(0,"Dialog",true);

  dialog->insertList(pathlist);
  dialog->setErrorString("Please select a Category or press Cancel");
  dialog->setTitle("Under which category would you like to store this CDDB entry?");

  if(dialog->exec() != QDialog::Accepted){
    delete dialog;
    return;
  }

  dialog->getSelection(path);
  QString mag;
  mag.sprintf("%s/%08x",path.data(),cdinfo->magicID);

  save_cddb_entry(mag,false);
  load();
  delete dialog;

}

void CDDialog::save_cddb_entry(QString& path,bool upload){


  QString magic;
  magic.sprintf("%08x",cdinfo->magicID);
  bool have_magic_already = false;

  for(int i = 0 ; i < (int)discidlist.count();i ++){
    if(magic == (QString)discidlist.at(i)){
      have_magic_already = true;
      break;
    }
  }

  if(!have_magic_already)
    discidlist.insert(0,magic.data());

  QFile file(path.data());


  if( !file.open( IO_WriteOnly  )) {
    
    QString str;
    str.sprintf(
		"Unable to write to file:\n%s\nPlease check"\
		"your permissions and make your category directories exist.",
		path.data());

    QMessageBox::warning(this,
			 "Kscd Error",
			 str.data()
			 );
    return;
  }

  QString tmp;
  QTextStream t(&file);

  t << "# xmcd CD database file\n";
  t << "# Copyright (C) 1997 - 1998  Bernd Johannes Wuebben\n";

  QString datestr;
  datestr = QDateTime::currentDateTime().toString();
  tmp = tmp.sprintf("# Generated: %s\n",datestr.data());
  t << tmp.data();

  t << "# \n";
  t << "# Track frame offsets:\n";

  for(int i = 0 ; i < cdinfo->ntracks;i ++){
    tmp = tmp.sprintf("#       %d\n",cdinfo->cddbtoc[i].absframe);
    t << tmp.data();
  }

  t << "#\n";
  tmp = tmp.sprintf("# Disc length: %d seconds\n",cdinfo->length);
  t << tmp.data();
  t << "#\n";
  if(upload)
    tmp = tmp.sprintf("# Revision: %d\n",++revision);
  else
    tmp = tmp.sprintf("# Revision: %d\n",revision);
  t << tmp.data();
  t << "# Submitted via: Kscd "KSCDVERSION"\n";
  t << "#\n";


  tmp = "DISCID=";
  int counter = 0;

  for(int i = 0 ; i < (int)discidlist.count();i ++){

    tmp += discidlist.at(i);

    if( i < (int) discidlist.count() - 1){
      if( counter++ == 3 ){
	tmp += "\nDISCID=";
	counter = 0;
      }
      else
	tmp += ",";
    }
  }

  tmp += "\n";
  t << tmp.data();

  QStrList returnlist;
  QString tmp2;

  tmp2 = track_list.at(0);
  cddb_encode(tmp2,returnlist);  

  for(int i = 0; i < (int) returnlist.count();i++){
    tmp = tmp.sprintf("DTITLE=%s\n",returnlist.at(i));
    t << tmp.data();
  }

  for(int i = 1 ; i < (int)track_list.count();i ++){

    tmp2 = track_list.at(i);
    cddb_encode(tmp2,returnlist);  

    for(int j = 0; j < (int) returnlist.count();j++){
      tmp = tmp.sprintf("TTITLE%d=%s\n",i-1,returnlist.at(j));
      t << tmp.data();
    }
  }

  tmp2 = ext_list.at(0);
  cddb_encode(tmp2,returnlist);  

  for(int i = 0; i < (int) returnlist.count();i++){
    tmp = tmp.sprintf("EXTD=%s\n",returnlist.at(i));
    t << tmp.data();
  }

  for(int i = 1 ; i < (int)ext_list.count();i ++){

    tmp2 = ext_list.at(i);
    cddb_encode(tmp2,returnlist);  

    for(int j = 0; j < (int) returnlist.count();j++){
      tmp = tmp.sprintf("EXTT%d=%s\n",i-1,returnlist.at(j));
      t << tmp.data();
    }
  }

  if(!upload){
    cddb_encode(playorder,returnlist);  

    for(int i = 0; i < (int) returnlist.count();i++){
      tmp = tmp.sprintf("PLAYORDER=%s\n",returnlist.at(i));
      t << tmp.data();
    }
  }
  else{
    tmp = tmp.sprintf("PLAYORDER=\n");
    t << tmp.data();
  }



  file.close();
  chmod(file.name(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
  return;

}


bool CDDialog::checkit(){


  QString title = titleedit->text();
  title = title.stripWhiteSpace();
  if(title.isEmpty()){

    QMessageBox::warning(this,
			 "Invalid Database Entry",
			 "The Disc Artist / Title is not filled in.\n"\
			 "Please correct the entry and try again."
			 );
     return false;
  }


  

  if(track_list.count() < 2){

    QMessageBox::warning(this,
			 "Invalid Database Entry",
			 "Not all track titles can be empty.\n"\
			 "Please correct the entry and try again."
			 );
     return false;
  }


  bool have_nonempty_title = false;
  for(int i = 1; i < (int)track_list.count(); i++){

    title = track_list.at(i);
    title = title.stripWhiteSpace();
    if(!title.isEmpty()){
      have_nonempty_title = true;
      break;
    }
  }

  if(!have_nonempty_title){

    QMessageBox::warning(this,
			 "Invalid Database Entry",
			 "Not all track titles can be empty.\n"\
			 "Please correct the entry and try again."
			 );
     return false;

  }

  if(cdinfo->ntracks +1 != (int)track_list.count() ){

    QMessageBox::critical(this,
			 "Internal Error",
			 "cdinfo->ntracks != title_list->count() + 1\n"
			 "Please email the author."
			 );
     return false;
  }

  QString str;
  QStrList strlist;
  str = progseq_edit->text();

  bool ret;
  ret = cddb_playlist_decode(strlist, str);
  
  QString teststr;
  bool ok;
  int  num;

  for(uint i = 0; i < strlist.count();i++){

    teststr = strlist.at(i);
    num = teststr.toInt(&ok);

    if( num > cdinfo->ntracks || !ok)
      ret = false;
  }

  if(!ret){
      QMessageBox::warning(this,
			 "Error",
			 "Invalid Playlist\n"
			 );
      return false;
  }

  cddb_playlist_encode(strlist,playorder);
  return true;

}


void CDDialog::load(){

  emit cddb_query_signal();

}
