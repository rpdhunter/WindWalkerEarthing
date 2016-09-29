#-------------------------------------------------
#
# Project created by QtCreator 2012-07-14T18:37:19
#
#-------------------------------------------------

QT          += core gui
QT          += opengl

TARGET      = WindWalkerEarthing
TEMPLATE    = app

CONFIG      += qwt

INCLUDEPATH += D:/Qt/Mylib/include \
               D:/Qt/myWork/qwtplot3d/include \
               D:/Qt/mingw/include/GL \
#               D:/Qt/MinGW32/include/GL \
#               D:/Qt/mylib/Qwt-6.0.2/include

#win32:CONFIG(release, debug|release): LIBS += D:/Qt/mylib/qwtplot3d/lib/qwtplot3d.dll
#else:win32:CONFIG(debug, debug|release): LIBS += D:/Qt/mylib/qwtplot3d/lib/qwtplot3dd.dll

#LIBS += D:/Qt/mylib/qwtplot3d/lib/qwtplot3d.dll
#LIBS += D:/Qt/mylib/qwtplot3d/lib/qwtplot3d.dll

#LIBS += D:/Qt/myWork/build-qwtplot3d-Desktop_Qt4_8_4_MinGW_4_4_32bit-Release/lib/qwtplot3d.dll
LIBS += D:/Qt/MyLibs/qwtplot3d.dll
#LIBS += D:/Qt/myWork/qwtplot3d-build-desktop-Qt_4_8_2__sdk____/lib/qwtplot3d.dll


SOURCES += main.cpp\
    windwalkerearthingmaindlg.cpp \
    earthdialog.cpp \
    Compute/cutthread.cpp \
    Compute/computdialog.cpp \
    Base/conductor.cpp \
    Base/point.cpp \
    Base/compute.cpp \    
    GroundCAD/GroundingCADWindow.cpp \
    GroundCAD/Graphics/graphicsview.cpp \
    GroundCAD/Graphics/graphicsscene.cpp \
    GroundCAD/Graphics/graphicslineitem.cpp \
    GroundCAD/Graphics/datamodel.cpp \
    Base/observedline.cpp \
    GroundCAD/Graphics/oblinelistmodle.cpp \
    Output/ObservedRect/plotoptiondialog.cpp \
    Output/ObservedRect/plot3dwindow.cpp \
    Output/ObservedRect/plot2d.cpp \
    Output/ObservedRect/functions.cpp \
    Output/oblinemainwindow.cpp \
    Output/oblineplot.cpp \
    Compute/DianzuCompute/dawalibi.cpp \
    Compute/DianzuCompute/kouteynikoff.cpp \
    Compute/DianzuCompute/heppe.cpp \
    Compute/DianzuCompute/jifenbase.cpp \
    Compute/DianzuCompute/jifenfa.cpp \
    Compute/DianzuCompute/dianzucompute.cpp \
    Compute/DianzuCompute/secoundsubdivision.cpp \
    Compute/SpeedUp/speedup.cpp \
    Compute/SpeedUp/conductorpair.cpp \
    Compute/OptimizedDesign/optimizeddesigndlg.cpp \
    GroundCAD/OtherDlg/translatedialog.cpp \
    GroundCAD/OtherDlg/newgrid.cpp \
    GroundCAD/OtherDlg/featuresdialog.cpp \
    GroundCAD/OtherDlg/check.cpp \
    GroundCAD/Power/powerdialog.cpp \
    GroundCAD/Power/addnewpowerdialog.cpp   \
    GroundCAD/undocommand.cpp \
    Base/currentsource.cpp \
    GroundCAD/Power/creatpointdialog.cpp \
    GroundCAD/Graphics/powerlistmodel.cpp \
    Compute/ParameterCompute/parametercomputebase.cpp \
    Compute/ParameterCompute/stepcompute.cpp \
    Compute/ParameterCompute/touchcompute.cpp \
    Compute/ParameterCompute/potentialcompute.cpp \
    Compute/ParameterCompute/observedlinecompute.cpp \
    Compute/ParameterCompute/cutcompute.cpp \
    Compute/SpeedUp/speedupcx.cpp \
    Compute/ParameterCompute/CurrentCompute/equal.cpp \
    Compute/ParameterCompute/CurrentCompute/unequal_r.cpp \
    Compute/ParameterCompute/CurrentCompute/unequal_z_v1.cpp \
    Compute/ParameterCompute/CurrentCompute/unequal_z_v2.cpp \
    Compute/ParameterCompute/CurrentCompute/currentcompute.cpp \
    Output/ObservedRect/plot2ditem/pointvoltageitem.cpp \
    Output/ObservedRect/plot2ditem/conductoritem.cpp \
    Output/ObservedRect/plot2ditem/plot2ditembase.cpp


HEADERS  += \
    windwalkerearthingmaindlg.h \
    earthdialog.h \
    Compute/cutthread.h \
    Compute/computdialog.h \
    Base/conductor.h \
    Base/point.h \
    Base/compute.h \
    GroundCAD/GroundingCADWindow.h \
    GroundCAD/Graphics/graphicsview.h \
    GroundCAD/Graphics/graphicsscene.h \
    GroundCAD/Graphics/graphicslineitem.h \
    GroundCAD/Graphics/datamodel.h \
    Base/observedline.h \
    GroundCAD/Graphics/oblinelistmodle.h \
    Output/ObservedRect/plotoptiondialog.h \
    Output/ObservedRect/plot3dwindow.h \
    Output/ObservedRect/plot2d.h \
    Output/ObservedRect/functions.h \
    Output/oblinemainwindow.h \
    Output/oblineplot.h \
    Compute/DianzuCompute/dawalibi.h \
    Compute/DianzuCompute/kouteynikoff.h \
    Compute/DianzuCompute/heppe.h \
    Compute/DianzuCompute/jifenbase.h \
    Compute/DianzuCompute/jifenfa.h \
    Compute/DianzuCompute/dianzucompute.h \
    Compute/DianzuCompute/secoundsubdivision.h \
    Compute/SpeedUp/speedup.h \
    Compute/SpeedUp/conductorpair.h \
    Compute/OptimizedDesign/optimizeddesigndlg.h \
    GroundCAD/OtherDlg/translatedialog.h \
    GroundCAD/OtherDlg/newgrid.h \
    GroundCAD/OtherDlg/featuresdialog.h \
    GroundCAD/OtherDlg/check.h \
    GroundCAD/Power/powerdialog.h \
    GroundCAD/Power/addnewpowerdialog.h \
    GroundCAD/undocommand.h \
    Base/currentsource.h \
    GroundCAD/Power/creatpointdialog.h \
    GroundCAD/Graphics/powerlistmodel.h \
    Compute/ParameterCompute/parametercomputebase.h \
    Compute/ParameterCompute/stepcompute.h \
    Compute/ParameterCompute/touchcompute.h \
    Compute/ParameterCompute/potentialcompute.h \
    Compute/ParameterCompute/observedlinecompute.h \
    Compute/ParameterCompute/cutcompute.h \
    Compute/SpeedUp/speedupcx.h \
    Compute/ParameterCompute/CurrentCompute/equal.h \
    Compute/ParameterCompute/CurrentCompute/unequal_r.h \
    Compute/ParameterCompute/CurrentCompute/unequal_z_v1.h \
    Compute/ParameterCompute/CurrentCompute/unequal_z_v2.h \
    Compute/ParameterCompute/CurrentCompute/currentcompute.h \
    Output/ObservedRect/plot2ditem/pointvoltageitem.h \
    Output/ObservedRect/plot2ditem/conductoritem.h \
    Output/ObservedRect/plot2ditem/plot2ditembase.h

FORMS    += \
    windwalkerearthingmaindlg.ui \
    earthdialog.ui \
    Compute/computdialog.ui \
    GroundCAD/GroundingCADWindow.ui \
    Output/ObservedRect/plotoptiondialog.ui \
    Output/ObservedRect/plot3dwindow.ui \
    Output/oblinemainwindow.ui \
    Compute/OptimizedDesign/optimizeddesigndlg.ui \
    GroundCAD/OtherDlg/translatedialog.ui \
    GroundCAD/OtherDlg/scaledialog.ui \
    GroundCAD/OtherDlg/rotatedialog.ui \
    GroundCAD/OtherDlg/oblineeditdialog.ui \
    GroundCAD/OtherDlg/newgrid.ui \
    GroundCAD/OtherDlg/featuresdialog.ui \
    GroundCAD/OtherDlg/editconductordialog.ui \
    GroundCAD/OtherDlg/check.ui \
    GroundCAD/Power/powerdialog.ui \
    GroundCAD/Power/addnewpowerdialog.ui \
    GroundCAD/Power/creatpointdialog.ui

RESOURCES += windwalker.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/GroundCAD/lib_win32/ -llapack_win32_MT
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/GroundCAD/lib_win32/ -llapack_win32_MT
else:unix: LIBS += -L$$PWD/GroundCAD/lib_win32/ -llapack_win32_MT



OTHER_FILES += \
    icon.rc
RC_FILE = icon.rc




