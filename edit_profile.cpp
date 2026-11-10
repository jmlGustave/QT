#include "edit_profile.h"
#include "ui_edit_profile.h"
#include <QVBoxLayout>
#include <QGridLayout> // <-- IMPORTANTE: Necesario para el overlay
#include <QFrame>
#include <QDir>
#include <QFileDialog>
#include <QPainter>
#include <QPainterPath>
#include "navigation.h"

edit_profile::edit_profile(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::edit_profile)
{
    ui->setupUi(this);
    setWindowTitle("Editar Perfil");
    resize(500, 700);
    this->setStyleSheet("QMainWindow { background-color: #1e272e; }");
    setupUiProgrammatically();
}

edit_profile::~edit_profile()
{
    delete ui;
}

void edit_profile::setupUiProgrammatically() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addStretch(); // Empuje superior

    QHBoxLayout *centerLayout = new QHBoxLayout();
    centerLayout->addStretch(); // Empuje izquierdo

    // 3. TARJETA CENTRAL
    main_container = new QWidget(centralWidget);
    main_container->setFixedWidth(380);
    main_container->setObjectName("CardFrame");
    main_container->setStyleSheet(R"(
        QWidget#CardFrame { background-color: #2f3640; border: 1px solid #d2b48c; border-radius: 15px; }
        QLabel { color: #dcdcdc; font-size: 13px; font-weight: bold; margin-bottom: 2px; }
        QLineEdit, QDateEdit { background-color: #1e272e; border: 1px solid #576574; border-radius: 6px; padding: 8px; color: white; font-size: 13px; }
        QLineEdit:focus, QDateEdit:focus { border: 1px solid #d2b48c; }
    )");

    QVBoxLayout *cardLayout = new QVBoxLayout(main_container);
    cardLayout->setContentsMargins(30, 40, 30, 40);
    cardLayout->setSpacing(15);

    // --- A. FOTO Y NOMBRE (SECCIÓN MODIFICADA CON OVERLAY) ---
    QVBoxLayout *identityLayout = new QVBoxLayout();
    identityLayout->setAlignment(Qt::AlignCenter);

    // 1. Contenedor para la superposición
    QWidget *photoOverlayContainer = new QWidget(main_container);
    photoOverlayContainer->setFixedSize(120, 120); // Tamaño exacto de la foto

    // Usamos un GridLayout para apilar widgets uno encima del otro
    QGridLayout *overlayGrid = new QGridLayout(photoOverlayContainer);
    overlayGrid->setContentsMargins(0,0,0,0);
    overlayGrid->setSpacing(0);

    // 2. El botón de la foto (Capa inferior)
    prf_button = new QPushButton(photoOverlayContainer);
    prf_button->setFixedSize(120, 120);
    prf_button->setCursor(Qt::PointingHandCursor);
    // Quitamos el borde dashed inicial para que no se pelee con el icono
    prf_button->setStyleSheet(
        "QPushButton { background-color: transparent; border: 2px solid #d2b48c; border-radius: 60px; }"
        "QPushButton:hover { border-color: #e67e22; }" // Al pasar el mouse, el borde principal cambia a naranja
        );
    // Añadimos en posición 0,0
    overlayGrid->addWidget(prf_button, 0, 0);

    // 3. El icono del lápiz (Capa superior)
    // 3. El icono del lápiz (Capa superior) - VERSIÓN MÁS PEQUEÑA Y CIRCULAR
    pencil_icon_lbl = new QLabel(photoOverlayContainer);

    // CAMBIO 1: Tamaño total más pequeño (antes 36x36)
    int iconWidgetSize = 28;
    pencil_icon_lbl->setFixedSize(iconWidgetSize, iconWidgetSize);

    // Estilo profesional: Círculo oscuro con borde beige fino
    pencil_icon_lbl->setStyleSheet(QString(
                                       "QLabel { "
                                       "background-color: #1e272e; " // Fondo oscuro del tema
                                       "border: 1px solid #d2b48c; " // CAMBIO 2: Borde más fino (1px) para el tamaño reducido
                                       "border-radius: %1px; "       // CAMBIO 3: Radio exacto (mitad del tamaño) para círculo perfecto
                                       "padding: 2px; "              // CAMBIO 4: Menos padding
                                       "}"
                                       ).arg(iconWidgetSize / 2)); // Calcula 14px automáticamente

    // Cargar y escalar el icono
    QPixmap pencilPix(":/icons/pencil.png");
    if (!pencilPix.isNull()) {
        // CAMBIO 5: Imagen interna más pequeña (antes 20x20, ahora 16x16) para que quepa bien
        pencil_icon_lbl->setPixmap(pencilPix.scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        pencil_icon_lbl->setAlignment(Qt::AlignCenter);
    }
    // Hacemos que el label sea transparente a los clicks del ratón
    pencil_icon_lbl->setAttribute(Qt::WA_TransparentForMouseEvents);

    // Añadimos en posición 0,0, alineado abajo a la derecha con un pequeño margen
    overlayGrid->addWidget(pencil_icon_lbl, 0, 0, Qt::AlignBottom | Qt::AlignRight);
    // Ajustamos el margen del grid para que el icono pequeño no quede muy pegado al borde
    overlayGrid->setContentsMargins(0, 0, 4, 4);


    // Añadimos el contenedor combinado al layout principal
    identityLayout->addWidget(photoOverlayContainer);

    username_lbl = new QLabel("Usuario", main_container);
    username_lbl->setStyleSheet("font-size: 20px; color: #e67e22; font-weight: bold; margin-top: 10px;");
    username_lbl->setAlignment(Qt::AlignCenter);
    identityLayout->addWidget(username_lbl);
    cardLayout->addLayout(identityLayout);

    // Separador
    QFrame *line = new QFrame(); line->setFrameShape(QFrame::HLine); line->setStyleSheet("color: #576574;"); cardLayout->addWidget(line);

    // --- B. FORMULARIO ---
    cardLayout->addWidget(new QLabel("Correo Electrónico:", main_container));
    email_edit = new QLineEdit(main_container); email_edit->setPlaceholderText("nuevo@correo.com"); cardLayout->addWidget(email_edit);
    cardLayout->addWidget(new QLabel("Nueva Contraseña:", main_container));
    password_edit = new QLineEdit(main_container); password_edit->setPlaceholderText("••••••••"); password_edit->setEchoMode(QLineEdit::Password); cardLayout->addWidget(password_edit);
    cardLayout->addWidget(new QLabel("Fecha de Nacimiento:", main_container));
    birth_edit = new QDateEdit(main_container); birth_edit->setDisplayFormat("dd/MM/yyyy"); birth_edit->setCalendarPopup(true);
    birth_edit->setStyleSheet(birth_edit->styleSheet() + "QCalendarWidget QWidget { background-color: #2f3640; color: white; selection-background-color: #e67e22; }");
    cardLayout->addWidget(birth_edit);
    cardLayout->addSpacing(10);

    // --- C. BOTÓN GUARDAR ---
    save_changes_button = new QPushButton("GUARDAR CAMBIOS", main_container);
    save_changes_button->setCursor(Qt::PointingHandCursor);
    save_changes_button->setMinimumHeight(45);
    save_changes_button->setStyleSheet("QPushButton { background-color: #e67e22; color: white; border: none; border-radius: 22px; font-weight: bold; font-size: 14px; letter-spacing: 1px;} QPushButton:hover { background-color: #d35400; } QPushButton:pressed { background-color: #a04000; padding-top: 2px; }");
    cardLayout->addWidget(save_changes_button);

    centerLayout->addWidget(main_container); centerLayout->addStretch();
    mainLayout->addLayout(centerLayout); mainLayout->addStretch(); // Empuje inferior

    // Conexiones
    connect(prf_button, &QPushButton::clicked, this, &edit_profile::change_prf);
    connect(save_changes_button, &QPushButton::clicked, this, &edit_profile::save_changes);
    setCentralWidget(centralWidget);
}

// Función auxiliar para recortar la imagen en círculo
void edit_profile::updateProfileIcon(const QImage &img) {
    if (img.isNull()) return;
    QPixmap pixmap = QPixmap::fromImage(img);
    pixmap = pixmap.scaled(120, 120, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QPixmap rounded(120, 120); rounded.fill(Qt::transparent);
    QPainter painter(&rounded); painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path; path.addEllipse(0, 0, 120, 120); painter.setClipPath(path);
    painter.drawPixmap(-(pixmap.width()-120)/2, -(pixmap.height()-120)/2, pixmap);
    prf_button->setIcon(QIcon(rounded)); prf_button->setIconSize(QSize(120, 120));
    // Ya no necesitamos cambiar el estilo aquí, el icono del lápiz siempre está visible
}

void edit_profile::setUser(QString &usr){
    user_nick = usr;
    Navigation &nav = Navigation::instance();
    User *u = nav.findUser(user_nick);

    if (u) {
        // Actualizamos Foto usando la función helper
        updateProfileIcon(u->avatar());

        // Datos
        username_lbl->setText(u->nickName()); // Nombre en grande y naranja

        email_edit->setText("");
        email_edit->setPlaceholderText(u->email());

        password_edit->setText("");
        password_edit->setPlaceholderText("••••••••"); // No mostramos la pass real por seguridad visual

        birth_edit->setDate(u->birthdate());

        prf_pic_path.clear();
    }
}

void edit_profile::change_prf(){
    QString filename = QFileDialog::getOpenFileName(this, "Seleccionar Imagen", QDir::homePath(), "Images (*.png *.jpg *.jpeg)");
    if (!filename.isEmpty()) {
        prf_pic_path = filename;
        QImage newImg(filename);
        updateProfileIcon(newImg); // Previsualizar al instante
    }
}

void edit_profile::save_changes(){
    Navigation &nav = Navigation::instance();
    User *originalUser = nav.findUser(user_nick);

    if (!originalUser) return;

    // Validación inteligente: Si está vacío, mantenemos el anterior
    QString newEmail = email_edit->text().isEmpty() ? originalUser->email() : email_edit->text();
    QString newPass = password_edit->text().isEmpty() ? originalUser->password() : password_edit->text();

    QImage newAvatar;
    if (prf_pic_path.isEmpty()) newAvatar = originalUser->avatar();
    else newAvatar = QImage(prf_pic_path);

    QDate newDate = birth_edit->date();

    User updatedUser(user_nick, newEmail, newPass, newAvatar, newDate);
    nav.updateUser(updatedUser);

    // Opcional: Mostrar mensaje de éxito
    // QMessageBox::information(this, "Perfil Actualizado", "Los cambios se han guardado correctamente.");

    this->close();
}

void edit_profile::closeEvent(QCloseEvent *event)
{
    if (this->parentWidget()) {
        this->parentWidget()->show();
    }
    event->accept();
}
