#include "customheaderbar.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QPalette>
#include <QPainter>
#include <QWidgetAction>
#include <QStyleOption>
#include <QPainterPath>
#include <QMessageBox> // Importante
#include "navigation.h"

QVector<CustomHeaderBar*> CustomHeaderBar::instances;

CustomHeaderBar::CustomHeaderBar(QWidget *parent, QString title)
    : QWidget(parent)
{
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setObjectName("CustomHeaderBar");

    // ... (Código de Menús y Login igual que antes) ...
    profileMenu = new QMenu(this);
    profileMenu->setObjectName("StyledMenu");
    loginMenu = new QMenu(this);
    loginMenu->setTitle("Iniciar Sesión");
    loginMenu->setObjectName("StyledMenu");

    profileMenu->addMenu(loginMenu);
    profileMenu->addSeparator();
    m1_option1 = profileMenu->addAction("Registrarse"); m1_option1->setObjectName("2");
    profileMenu->addSeparator();
    m1_option2 = profileMenu->addAction("Historial"); m1_option2->setObjectName("3");
    m1_option4 = profileMenu->addAction("Modificar Perfil"); m1_option4->setObjectName("5");
    profileMenu->addSeparator();
    m1_option3 = profileMenu->addAction("Cerrar Sesión"); m1_option3->setObjectName("4");

    // Widgets de Login
    QString inputStyle = "QLineEdit { background-color: #1e272e; border: 1px solid #576574; border-radius: 4px; padding: 6px; color: white; selection-background-color: #d2b48c; } QLineEdit:focus { border: 1px solid #d2b48c; }";
    username = new QLineEdit(loginMenu); username->setPlaceholderText("Usuario"); username->setStyleSheet(inputStyle);
    password = new QLineEdit(loginMenu); password->setPlaceholderText("Contraseña"); password->setEchoMode(QLineEdit::Password); password->setStyleSheet(inputStyle);

    QWidgetAction *username_action = new QWidgetAction(loginMenu); username_action->setDefaultWidget(username); loginMenu->addAction(username_action);
    QWidgetAction *password_action = new QWidgetAction(loginMenu); password_action->setDefaultWidget(password); loginMenu->addAction(password_action);

    // --- WIDGETS DE LA BARRA ---

    // 1. Botón de Ayuda (NUEVO)
    helpButton = new QPushButton("?", this);
    helpButton->setObjectName("HelpBtn"); // ID para CSS
    helpButton->setCursor(Qt::PointingHandCursor);
    helpButton->setFixedSize(30, 30);
    // Tooltip para accesibilidad
    helpButton->setToolTip("Ver controles de la aplicación");

    m_profileButton = new QPushButton(this);
    m_profileButton->setIcon(QIcon(":/icons/default.png"));
    m_profileButton->setObjectName("ProfileIconBtn");
    m_profileButton->setCursor(Qt::PointingHandCursor);

    text = new QLabel(title, this);
    text->setObjectName("HeaderTitle");

    user = new QPushButton("Invitado", this);
    user->setObjectName("UserNameBtn");
    user->setCursor(Qt::PointingHandCursor);

    spacer = new QSpacerItem(20, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);

    setupUi();

    // Conexiones
    connect(profileMenu, &QMenu::triggered, this, &CustomHeaderBar::handleMenuAction);
    connect(m_profileButton, &QPushButton::clicked, this, &CustomHeaderBar::showProfileMenu);
    connect(user, &QPushButton::clicked, this, &CustomHeaderBar::showProfileMenu);
    connect(username, &QLineEdit::returnPressed, this, &CustomHeaderBar::handleLoginAction);
    connect(password, &QLineEdit::returnPressed, this, &CustomHeaderBar::handleLoginAction);

    // Conexión Ayuda (NUEVO)
    connect(helpButton, &QPushButton::clicked, this, &CustomHeaderBar::showHelp);

    instances.append(this);
}

CustomHeaderBar::~CustomHeaderBar() {
    instances.removeAll(this);
}

void CustomHeaderBar::setupUi()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(20, 5, 20, 5);
    layout->setSpacing(10);

    // Izquierda: Marca
    layout->addWidget(text);

    // Centro: Espacio vacío (Figura/Fondo)
    layout->addSpacerItem(spacer);

    // Derecha: Controles (Agrupados por Proximidad)
    layout->addWidget(helpButton); // Botón ayuda primero

    // Pequeño separador vertical visual (opcional, o un spacing fijo)
    QSpacerItem* miniSpacer = new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addSpacerItem(miniSpacer);

    layout->addWidget(user);
    layout->addWidget(m_profileButton);
    m_profileButton->setFixedSize(40, 40);
    user->setMinimumHeight(30);

    // --- HOJA DE ESTILOS ---
    this->setStyleSheet(R"(
        QWidget#CustomHeaderBar {
            background-color: #1e272e;
            border-bottom: 2px solid #2f3640;
        }
        QLabel#HeaderTitle {
            color: #dcdcdc;
            font-size: 18px;
            font-weight: bold;
            letter-spacing: 1px;
            font-family: 'Segoe UI', Arial, sans-serif;
        }
        QPushButton#UserNameBtn {
            color: #ffffff;
            background-color: transparent;
            border: none;
            font-size: 14px;
            font-weight: 600;
            padding: 5px 10px;
            text-align: right;
        }
        QPushButton#UserNameBtn:hover { color: #d2b48c; }

        QPushButton#ProfileIconBtn {
            background-color: transparent;
            border: 2px solid transparent;
            border-radius: 20px;
        }
        QPushButton#ProfileIconBtn:hover { border: 2px solid #e67e22; }

        /* ESTILO BOTÓN DE AYUDA (NUEVO) - Ley de Semejanza (forma circular) */
        QPushButton#HelpBtn {
            background-color: transparent;
            border: 2px solid #576574; /* Gris sutil */
            border-radius: 15px; /* Perfectamente redondo (30x30) */
            color: #bdc3c7;
            font-weight: bold;
            font-size: 16px;
        }
        QPushButton#HelpBtn:hover {
            border-color: #d2b48c; /* Mismo color de acento que el resto */
            color: #d2b48c;
            background-color: rgba(210, 180, 140, 0.1);
        }

        /* Menús */
        QMenu {
            background-color: #2f3640;
            color: white;
            border: 1px solid #d2b48c;
            padding: 5px;
        }
        QMenu::item { padding: 8px 25px; border-radius: 4px; }
        QMenu::item:selected { background-color: #e67e22; color: white; }
        QMenu::separator { height: 1px; background: #576574; margin: 5px 0; }
    )");
}

void CustomHeaderBar::paintEvent(QPaintEvent *event){
    QStyleOption opt;
    // CORRECCIÓN: Usar initFrom(this) en lugar de init(this)
    opt.initFrom(this);

    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CustomHeaderBar::showProfileMenu(){
    // Alineamos el menú debajo del botón de perfil
    QPoint pos = m_profileButton->mapToGlobal(QPoint(0, m_profileButton->height()));
    hide_logged_actions();
    profileMenu->popup(pos);
}

void CustomHeaderBar::handleMenuAction(QAction* action){
    bool ok;
    int id = action->objectName().toInt(&ok);
    if (!ok) return;

    if(id == 1) showLoginMenu();
    else if (id == 2) emit registrationRequested();
    else if (id == 3) emit historyRequested(user_nav);
    else if (id == 4) log_out();
    else if (id == 5 && user_nav) emit editRequested(user_nav->nickName());

    emit profileOptionSelected(id);
}

void CustomHeaderBar::showLoginMenu(){
    // Muestra el submenú de login
    QPoint pos = profileMenu->pos();
    // Ajuste fino para que salga al lado o centrado
    loginMenu->popup(profileMenu->mapToGlobal(QPoint(-200, 0)));
}

void CustomHeaderBar::handleLoginAction(){
    usr = username->text();
    QString pass = password->text();
    if(!usr.isEmpty() && !pass.isEmpty()){
        Navigation &nav = Navigation::instance();
        User* loggedUser = nav.authenticate(usr, pass);

        if (loggedUser != nullptr){
            // Sincronizar todas las barras abiertas
            syncAllBars(loggedUser);
            // Limpiar campos por seguridad
            username->clear();
            password->clear();
            profileMenu->close(); // Cerrar menú al loguear
        } else {
            // Opcional: Feedback visual de error (ej: borde rojo)
            username->setStyleSheet("border: 1px solid red; background-color: #1e272e; color: white;");
        }
    }
}

void CustomHeaderBar::hide_logged_actions(){
    // Lógica para ocultar/mostrar opciones según estado
    bool isLogged = logged;
    if(m1_option1) m1_option1->setVisible(!isLogged); // Registrarse
    if(loginMenu) loginMenu->menuAction()->setVisible(!isLogged); // Login

    if(m1_option2) m1_option2->setVisible(isLogged); // Historial
    if(m1_option3) m1_option3->setVisible(isLogged); // Logout
    if(m1_option4) m1_option4->setVisible(isLogged); // Modificar
}

void CustomHeaderBar::update_bar(QString usr){
    // --- CORRECCIÓN CRÍTICA ---
    // Actualizamos la variable miembro para que 'closeEvent' sepa quién es el usuario
    this->usr = usr;
    // --------------------------

    if(logged){
        Navigation &nav = Navigation::instance();
        User *u = nav.findUser(usr);

        if (u == nullptr) {
            return;
        }

        user_nav = u;

        QImage img = u->avatar();
        if (img.isNull()) {
            img = QImage(50, 50, QImage::Format_ARGB32);
            img.fill(Qt::gray);
        }

        QPixmap pixmap = QPixmap::fromImage(img);
        pixmap = pixmap.scaled(50,50, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        QIcon icon(pixmap);
        m_profileButton->setIcon(icon);
        QSize size(30,30);
        m_profileButton->setIconSize(size);

        user->setText(u->nickName());
    }else{
        QIcon icon(":/icons/default.png");
        if (icon.isNull()) icon = QIcon("default.png");

        m_profileButton->setIcon(icon);
        QSize size(30,30);
        m_profileButton->setIconSize(size);
        user->setText(usr);
    }
    hide_logged_actions();
}

void CustomHeaderBar::log_out(){
    Navigation &nav = Navigation::instance();
    // Guardar sesión si es necesario
    if (user_nav) {
        Session s(QDateTime::currentDateTime(), hits, faults);
        nav.addSession(user->text(), s);
    }
    syncAllBars(nullptr);
}

void CustomHeaderBar::syncAllBars(User *u){
    for(CustomHeaderBar* bar: instances){
        if (u != nullptr){
            bar->logged = true;
            bar->update_bar(u->nickName()); // update_bar actualizará user_nav internamente
        }else{
            bar->logged = false;
            bar->update_bar("Invitado");
        }
    }
}
void CustomHeaderBar::setTitle(QString newTitle) {
    if (text) {
        text->setText(newTitle);
    }
}

void CustomHeaderBar::showHelp() {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Guía de Controles");

    // Usamos HTML para formatear el texto respetando la jerarquía visual
    QString helpText = R"(
    <style>
        h3 { color: #d2b48c; margin-bottom: 5px; }
        ul { margin-top: 0; padding-left: 20px; color: #ecf0f1; }
        li { margin-bottom: 5px; }
        .key { font-weight: bold; color: #e67e22; }
    </style>

    <h3>🖱️ Navegación Básica</h3>
    <ul>
        <li><span class="key">Rueda del Ratón:</span> Zoom In / Zoom Out.</li>
        <li><span class="key">Clic Izquierdo + Arrastrar:</span> Mover el mapa (Pan).</li>
    </ul>

    <h3>📐 Instrumentos (Regla, Compás...)</h3>
    <ul>
        <li><span class="key">Clic Izquierdo:</span> Seleccionar instrumento.</li>
        <li><span class="key">Selección + Rueda:</span> Rotar instrumento (sin zoom).</li>
        <li><span class="key">Clic Derecho:</span> Cancelar herramienta / Soltar instrumento.</li>
    </ul>

    <h3>✏️ Edición</h3>
    <ul>
        <li><span class="key">Borrador:</span> Elimina objetos al hacer clic sobre ellos.</li>
        <li><span class="key">Clic Derecho (en el vacío):</span> Menú contextual.</li>
    </ul>
    )";

    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(helpText);

    // Aplicamos estilo al propio MessageBox para que no rompa la estética Dark
    msgBox.setStyleSheet("QMessageBox { background-color: #2f3640; } QLabel { color: white; } QPushButton { background-color: #1e272e; color: white; border: 1px solid #576574; padding: 5px 15px; border-radius: 4px; } QPushButton:hover { border-color: #d2b48c; }");

    msgBox.exec();
}
