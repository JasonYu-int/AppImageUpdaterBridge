#include <QMainWindow>
#include <QApplication>
#include <QMenuBar>
#include <QScreen>
#include <QDebug>
#include <QRegExp>
#include <QProcessEnvironment>
#include "../include/appimageupdaterbridge.hpp"

typedef AppImageUpdaterBridge::AppImageUpdaterBridge ClassAppImageUpdaterBridge;

using AppImageUpdaterBridge::AppImageDeltaRevisioner;

ClassAppImageUpdaterBridge::AppImageUpdaterBridge(QObject *parent)
	: QObject(parent)
{
	m_Updater.reset(new AppImageDeltaRevisioner);
}

void ClassAppImageUpdaterBridge::init()
{
	qInfo().noquote() << "INIT() called";
	QTimer::singleShot(1000 , QApplication::instance() , &QApplication::quit);
	return;
}
