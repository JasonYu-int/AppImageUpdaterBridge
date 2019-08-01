#ifndef APPIMAGE_UPDATER_BRIDGE_INTERFACE_HPP_INCLUDED 
#define APPIMAGE_UPDATER_BRIDGE_INTERFACE_HPP_INCLUDED 
#include <QtPlugin>

class AppImageUpdaterBridgeInterface {
public:
	virtual ~AppImageUpdaterBridgeInterface() {}
public Q_SLOTS:
	virtual void init() = 0;
};

#ifndef AppImageUpdaterBridgeInterface_iid
#define AppImageUpdaterBridgeInterface_iid "com.antony-jr.AppImageUpdaterBridge"
#endif 

Q_DECLARE_INTERFACE(AppImageUpdaterBridgeInterface , AppImageUpdaterBridgeInterface_iid);

#endif // APPIMAGE_UPDATER_BRIDGE_INTERFACE_HPP_INCLUDED
