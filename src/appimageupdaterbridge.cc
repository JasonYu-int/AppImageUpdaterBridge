#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QPushButton>
#include <QScreen>
#include <QDebug>
#include <QRegExp>
#include <QProcessEnvironment>
#include "../include/appimageupdaterbridge.hpp"
#include "../include/appimageupdaterdialog.hpp"

/*
 * Settings on how the bridge should behave.
 *
 * interval - The interval to init the update check on startup of the 
 *            application if auto update check is enabled given 
 *            in the booleans string.
 * 
 * qmenu_name - The QObject name of the QMenu to integrate the update check.
 *
 * qmenubar_name - The QObject name of the QMenuBar to integrate the update check.
 *
 * qpushbutton_name - The QObject name of the QPushButton to connect the clicked signal
 *                    to the update check.
 *
 * qpushbutton_text - The text of any QPushButton with this text will have its clicked signal 
 *                    connected to the update check.
 *
 * qaction_to_override_qobject_name - The QObject name of the QAction to integrate the update check.
 *
 * qaction_to_override_text - The text of any QAction with this text will be integrated with the update check.
 *
*/
/*------------------------------------------------------------------------------------------*/
/* | */const char *interval = "75629552e6e8286442676be60e7da67d";                        /*|*/
/* | */const char *qmenu_name = "871abbc22416bb25429594dec45caf1f";                      /*|*/
/* | */const char *qmenu_text = "97393fe3f5e452adfc36db9dfaff5628";                      /*|*/
/* | */const char *qmenubar_name = "bfa40825ef36e05bbc2c561595829a92";                   /*|*/
/* | */const char *qpushbutton_name = "930b29debfb164461b39342d59e2565c";                /*|*/
/* | */const char *qpushbutton_text = "130047834d253af84d40d1c0fb52f02d";                /*|*/
/* | */const char *qaction_to_override_qobject_name = "0b6e66aa3800ad9cad94fe41984b9b56";/*|*/
/* | */const char *qaction_to_override_text = "78c9a822db28277212b8be5d73764c7f";        /*|*/
/* | */const char *booleans = "4c6160c2d6bfeba1";                                        /*|*/
/*------------------------------------------------------------------------------------------*/


/*
 * Representation of the boolean string.
 * ------------------------------------------------------------------------------------*
 * booleans[0] - If set then auto update check is enabled
 * booleans[1] - If set then auto update check should occur on startup of the 
 *               application.
 * booleans[2] - If set then manual update check is enabled.
 * booleans[3] - If set then QMenu Object name is available
 * booleans[4] - If set then QMenuBar QObject name is available.
 * booleans[5] - If set then QPushButton QObject name is available.
 * booleans[6] - If set then QPushButton Substring text is available.
 * booleans[7] - If set then QAction QObject name is available.
 * booleans[8] - If set then QAction Substring text is available.
 * booleans[9] - If set then Interval is given.
 * booleans[10] - If set then QMenu title is available. 
 * -------------------------------------------------------------------------------------*
*/

#define IS_BOOLEAN_TRUE(n) ((booleans[n] != 1) ? false : true)
#define AUTO_UPDATE_ENABLED IS_BOOLEAN_TRUE(0)
#define AUTO_UPDATE_AT_STARTUP IS_BOOLEAN_TRUE(1)
#define MANUAL_UPDATE_ENABLED IS_BOOLEAN_TRUE(2)
#define QMENU_QOBJECT_NAME_GIVEN IS_BOOLEAN_TRUE(3)
#define QMENUBAR_QOBJECT_NAME_GIVEN IS_BOOLEAN_TRUE(4)
#define QPUSHBUTTON_QOBJECT_NAME_GIVEN IS_BOOLEAN_TRUE(5)
#define QPUSHBUTTON_TEXT_GIVEN IS_BOOLEAN_TRUE(6)
#define QACTION_QOBJECT_NAME_GIVEN IS_BOOLEAN_TRUE(7)
#define QACTION_TEXT_GIVEN IS_BOOLEAN_TRUE(8)
#define INTERVAL_GIVEN IS_BOOLEAN_TRUE(9)
#define QMENU_TEXT_GIVEN IS_BOOLEAN_TRUE(10)

/* --------------------------------------------------------------------------------------*/

typedef AppImageUpdaterBridge::AppImageUpdaterBridge ClassAppImageUpdaterBridge;

using AppImageUpdaterBridge::AppImageDeltaRevisioner;

ClassAppImageUpdaterBridge::AppImageUpdaterBridge(QObject *parent)
	: QObject(parent)
{
	auto instance = qobject_cast<QApplication*>(QApplication::instance());
	QPixmap icon = QApplication::windowIcon().pixmap(100 , 100);
   
	if(!instance||
	   (QProcessEnvironment::systemEnvironment().value("APPIMAGE")).isEmpty()){
		qDebug() << "AppImageUpdaterBridge:: INFO: no gui instance found or not appimage, giving up.";
		return;
	}

	/* We don't want to crash the entire payload just so that our plugin 
	 * is not working. 
	 * This will not happen in most cases but just to be safe. */
	try{
		m_Updater = new AppImageDeltaRevisioner(instance);
		m_Dialog = new AppImageUpdaterDialog(icon,nullptr,
				AppImageUpdaterDialog::Default ^ 
				AppImageUpdaterDialog::NoRemindMeLaterButton ^ 
				AppImageUpdaterDialog::NoSkipThisVersionButton);
	}catch( ... ){
		qDebug() << "AppImageUpdaterBridge:: FATAL: cannot allocate space for delta revisioner, giving up.";
		return;
	}
	connect(m_Dialog , &AppImageUpdaterDialog::quit , instance , &QApplication::quit , Qt::QueuedConnection);
}

ClassAppImageUpdaterBridge::~AppImageUpdaterBridge(){
	if(m_Dialog){
		qDebug() << "AppImageUpdaterBridge:: INFO: destructing AppImageUpdaterBridge.";
		m_Dialog->disconnect();
		m_Dialog->hide();
		m_Dialog->deleteLater();
	}
}

/* Public function which will be called first. */
void ClassAppImageUpdaterBridge::initAppImageUpdaterBridge()
{
	if(!m_Updater){
		return;
	}

	// We want to start the updater right now after the 
	// given interval.
	if(AUTO_UPDATE_ENABLED && !MANUAL_UPDATE_ENABLED){
		m_Timer.setSingleShot(true);
		if(INTERVAL_GIVEN){
			m_Timer.setInterval((QString::fromUtf8(interval)).toInt());
		}else{
			m_Timer.setInterval(10 * 1000 /* 10 seconds in miliseconds */);
		}
		
		connect(&m_Timer , &QTimer::timeout , this , &ClassAppImageUpdaterBridge::initAutoUpdateCheck);
		m_Timer.start();
		return;
	}

	if(!MANUAL_UPDATE_ENABLED){
		// Nothing is enabled, just exit.
		return; 
	}

	// Manual update is enabled, we have to integrate into any menus or buttons.
	connect(&m_Timer , &QTimer::timeout , this , &ClassAppImageUpdaterBridge::tryIntegrate);
	tryIntegrate();
	return;
}

/* Private slots. */
void ClassAppImageUpdaterBridge::initAutoUpdateCheck(){
	m_Timer.stop();
	m_Updater->disconnect(); // Disconnect all connections, just to be safe.
	m_Dialog->move(QGuiApplication::primaryScreen()->geometry().center() - m_Dialog->rect().center());	 
	m_Dialog->init(m_Updater);
}

void ClassAppImageUpdaterBridge::handleUpdateCheck(){
	m_Dialog->move(QGuiApplication::primaryScreen()->geometry().center() - m_Dialog->rect().center());	 
	m_Dialog->init(m_Updater); // should be ignored if its already busy.
}

void ClassAppImageUpdaterBridge::tryIntegrate(){
	m_Timer.stop();
	bool integrated = false;

	foreach (QWidget *widget, QApplication::allWidgets()){
		if((QMENU_QOBJECT_NAME_GIVEN || QMENU_TEXT_GIVEN) && !b_IntegratedQMenu){
			qDebug() << "AppImageUpdaterBridge::INFO: QMenu object name or text given.";
			integrated = b_IntegratedQMenu = integrateQMenu(widget);
		}

		if(QMENUBAR_QOBJECT_NAME_GIVEN && !b_IntegratedQMenuBar){
			qDebug() << "AppImageUpdaterBridge::INFO: QMenuBar object name given.";
			integrated = b_IntegratedQMenuBar = integrateQMenuBar(widget);	
		}

		if((QPUSHBUTTON_QOBJECT_NAME_GIVEN || QPUSHBUTTON_TEXT_GIVEN) && !b_IntegratedQPushButton){
			qDebug() << "AppImageUpdaterBridge::INFO: QPushButton object name given.";
			integrated = b_IntegratedQPushButton = integrateQPushButton(widget);
		}

		if((QACTION_QOBJECT_NAME_GIVEN || QACTION_TEXT_GIVEN) && !b_IntegratedQAction){
			qDebug() << "AppImageUpdaterBridge::INFO: QAction object name given.";
			integrated = b_IntegratedQAction = integrateQAction(widget);
		}

		if(m_Dialog->windowIcon().isNull() && !((widget->windowIcon()).isNull())){
			m_Dialog->setWindowIcon(widget->windowIcon().pixmap(100 , 100));
		}	
		QCoreApplication::processEvents();
		if(integrated){
			break;
		}
	}

	if(!integrated){
		m_Timer.setInterval(5000);
		m_Timer.setSingleShot(true);
		m_Timer.start();
	}
	return;
}

bool ClassAppImageUpdaterBridge::integrateQAction(QWidget *widget){
	auto action = qobject_cast<QAction*>(widget);
	if(!action){
		return false;
	}

	// to debug python and qml apps.
#ifndef LOGGING_DISABLED
	qDebug() << "||||||----QMENU ACTION----";
	qDebug() << "||||||QObject name: " << action->objectName();
	qDebug() << "||||||Text: " << action->text();
	qDebug() << "||||||--------------------";	
#endif

	if((action->text().contains(qaction_to_override_text , Qt::CaseInsensitive) && QACTION_TEXT_GIVEN) ||
	    action->objectName() == qaction_to_override_qobject_name){
		action->disconnect(); // disconnect all slots connected to this.
		QObject::connect(action , &QAction::triggered , this , &ClassAppImageUpdaterBridge::handleUpdateCheck);
		return true;
	}
	return false;
}

bool ClassAppImageUpdaterBridge::integrateQMenu(QWidget *widget){
	auto menu = qobject_cast<QMenu*>(widget);
	if(!menu){
		return false;
	}

#ifndef LOGGING_DISABLED
	qDebug() << "----QMENU WIDGET----";  // for debugging in pythong apps.
	qDebug() << "QObject name: " << menu->objectName();
	qDebug() << "Menu Title: " << menu->title();
	qDebug() << "--------------------";
#endif

	if(menu->objectName() == qmenu_name ||
	   ((menu->title()).contains(QString::fromUtf8(qmenu_text) , Qt::CaseInsensitive) && QMENU_TEXT_GIVEN)){
		auto checkForUpdateAction = menu->addAction(QString::fromUtf8("Check for Update"));
		QObject::connect(checkForUpdateAction , &QAction::triggered , this , &ClassAppImageUpdaterBridge::handleUpdateCheck);
		return true;
	}
	return false;
}

bool ClassAppImageUpdaterBridge::integrateQMenuBar(QWidget *widget){
	auto menubar = qobject_cast<QMenuBar*>(widget);
	if(!menubar){
		return false;
	}

#ifndef LOGGING_DISABLED
	qDebug() << "----QMENUBAR WIDGET----";  // for debugging in pythong apps.
	qDebug() << "QObject name: " << menubar->objectName();
	qDebug() << "--------------------";
#endif

	if(menubar->objectName() != qmenubar_name){	
	    return false;
	}
	
	auto checkForUpdateAction = menubar->addAction(QString::fromUtf8("Check for Update"));
	QObject::connect(checkForUpdateAction , &QAction::triggered , this , &ClassAppImageUpdaterBridge::handleUpdateCheck);
	return true;
}

bool ClassAppImageUpdaterBridge::integrateQPushButton(QWidget *widget){
	auto pushbutton = qobject_cast<QPushButton*>(widget);
	if(!pushbutton){
		return false;
	}

#ifndef LOGGING_DISABLED
	qDebug() << "----QPUSHBUTTON WIDGET----";  // for debugging in pythong apps.
	qDebug() << "QObject name: " << pushbutton->objectName();
	qDebug() << "Text: " << pushbutton->text();
	qDebug() << "--------------------";
#endif

	if(pushbutton->objectName() == qpushbutton_name ||
	(pushbutton->text() == qpushbutton_text && QPUSHBUTTON_TEXT_GIVEN)){
		QObject::connect(pushbutton , &QPushButton::clicked , this , &ClassAppImageUpdaterBridge::handleUpdateCheck,
			Qt::QueuedConnection);
		return true;	
	}
	return false;
}
