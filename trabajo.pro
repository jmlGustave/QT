# sign_up.pro - Contenido Final Corregido

# Módulos de Qt
QT       += core gui sql svg svgwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# ... (rest of standard configurations)

SOURCES += \
    customheaderbar.cpp \
    edit_profile.cpp \
    history.cpp \
    main.cpp \
    mainwindow.cpp \
    problem_list.cpp \
    sign_up.cpp

HEADERS += \
    customheaderbar.h \
    edit_profile.h \
    history.h \
    mainwindow.h \
    problem_list.h \
    sign_up.h


FORMS += \
    mainwindow.ui \


# Reglas predeterminadas para el despliegue.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# --------------------------------------------------------------------------
# CORRECCIÓN DEFINITIVA DE ENLAZADO DE LIBRERÍA ESTÁTICA Y SUS DEPENDENCIAS
# --------------------------------------------------------------------------

# 1. Incluir las cabeceras de la librería
INCLUDEPATH += $$PWD/lib/include

# 2. Enlazar la librería estática (libnavlib.a)
# Se define la ruta completa para usar --whole-archive
NAV_LIB_PATH = $$PWD/lib/libnavlib.a

# 3. **PASO CRÍTICO:** Usar --whole-archive. Esto fuerza al enlazador a incluir
# todos los objetos de la librería, no solo los directamente referenciados.
# Esto es necesario porque tu librería estática (navlib.a) tiene referencias internas
# a funciones de Qt que de otra forma se perderían.
LIBS += -Wl,--whole-archive $$NAV_LIB_PATH -Wl,--no-whole-archive

DISTFILES += \
    default.png

RESOURCES += \
    resources.qrc
