INCLUDEPATH += $$PWD
QT += core widgets network
HEADERS += \
    $$PWD/include/appimageupdateinformation_p.hpp \
    $$PWD/include/zsyncremotecontrolfileparser_p.hpp \
    $$PWD/include/zsyncinternalstructures_p.hpp \
    $$PWD/include/zsyncwriter_p.hpp \
    $$PWD/include/zsyncblockrangereply_p.hpp \
    $$PWD/include/zsyncblockrangedownloader_p.hpp \
    $$PWD/include/appimagedeltarevisioner_p.hpp \
    $$PWD/include/appimagedeltarevisioner.hpp \
    $$PWD/include/appimageupdaterbridge_enums.hpp \
    $$PWD/include/appimageupdaterbridge.hpp \
    $$PWD/include/appimageupdaterdialog.hpp

SOURCES += \
    $$PWD/src/appimageupdateinformation_p.cc \
    $$PWD/src/zsyncremotecontrolfileparser_p.cc \
    $$PWD/src/zsyncwriter_p.cc \
    $$PWD/src/zsyncblockrangereply_p.cc \
    $$PWD/src/zsyncblockrangedownloader_p.cc \
    $$PWD/src/appimagedeltarevisioner_p.cc \
    $$PWD/src/appimagedeltarevisioner.cc \
    $$PWD/src/appimageupdaterdialog.cc

no_gui {
	QT -= widgets
	HEADERS -= $$PWD/include/appimageupdaterdialog.hpp
	SOURCES -= $$PWD/src/appimageupdaterdialog.cc
}

logging_disabled {
	message(Logging will be disabled for this build.)
	DEFINES += LOGGING_DISABLED
}
