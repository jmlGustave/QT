#include "sign_up.h"
#include "navigation.h" // Necesario para verificar si el usuario existe
#include <QEvent>
#include <QMessageBox>
#include <QRegularExpression>

Sign_Up::Sign_Up(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Crear Cuenta");
    resize(400, 750); // Un poco más alto para que quepan los mensajes de error
    setupUiProgrammatically();

    // Aplicamos estilos base (si no lo tienes en el main)
    this->setStyleSheet("QDialog { background-color: #1e272e; }");
}

Sign_Up::~Sign_Up() {}

// Helper para crear etiquetas de error estandarizadas
QLabel* Sign_Up::createErrorLabel(QWidget *parent) {
    QLabel *lbl = new QLabel(parent);
    lbl->setStyleSheet("color: #e74c3c; font-size: 11px; font-style: italic; margin-top: 0px;");
    lbl->setWordWrap(true); // Para que el texto baje si es muy largo
    lbl->setVisible(false); // Oculto por defecto
    return lbl;
}

void Sign_Up::setupUiProgrammatically() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 20, 0, 20);
    mainLayout->setAlignment(Qt::AlignCenter);

    main_container = new QFrame(this);
    main_container->setObjectName("CardFrame");
    main_container->setFixedWidth(360);
    // Estilo CSS incrustado para asegurar que se vea bien
    main_container->setStyleSheet(
        "#CardFrame { background-color: #2f3640; border-radius: 20px; border: 1px solid #4b5563; }"
        "QLineEdit { background-color: #1e272e; border: 1px solid #576574; border-radius: 8px; padding: 10px; color: white; }"
        "QLineEdit:focus { border: 1px solid #d2b48c; }"
        "QDateEdit { background-color: #1e272e; border: 1px solid #576574; border-radius: 8px; padding: 10px; color: white; }"
        "QLabel { color: white; }"
        );

    QVBoxLayout *cardLayout = new QVBoxLayout(main_container);
    cardLayout->setSpacing(10); // Reducimos un poco el espaciado base
    cardLayout->setContentsMargins(30, 30, 30, 30);

    // Título
    title_label = new QLabel("REGISTRO", main_container);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 10px; color: white;");

    // Foto
    profile_pic_label = new QLabel("Subir Foto", main_container);
    profile_pic_label->setObjectName("ProfilePic");
    profile_pic_label->setFixedSize(100, 100);
    profile_pic_label->setAlignment(Qt::AlignCenter);
    profile_pic_label->setStyleSheet("QLabel#ProfilePic { background-color: #353b48; color: #d2b48c; border: 2px dashed #d2b48c; border-radius: 50px; }");
    profile_pic_label->setCursor(Qt::PointingHandCursor);
    profile_pic_label->installEventFilter(this);

    QHBoxLayout *photoLayout = new QHBoxLayout();
    photoLayout->addStretch();
    photoLayout->addWidget(profile_pic_label);
    photoLayout->addStretch();

    // --- FORMULARIO CON VALIDACIONES ---
    QVBoxLayout *formLayout = new QVBoxLayout();
    formLayout->setSpacing(5); // Espacio pequeño entre elementos

    // 1. Usuario
    username_line = new QLineEdit(main_container);
    username_line->setPlaceholderText("Nombre de Usuario");
    err_username = createErrorLabel(main_container);

    // 2. Email
    email_line = new QLineEdit(main_container);
    email_line->setPlaceholderText("Correo Electrónico");
    err_email = createErrorLabel(main_container);

    // 3. Password
    password_line = new QLineEdit(main_container);
    password_line->setPlaceholderText("Contraseña");
    password_line->setEchoMode(QLineEdit::Password);
    err_password = createErrorLabel(main_container);

    // 4. Repetir Password
    password_repeat_line = new QLineEdit(main_container);
    password_repeat_line->setPlaceholderText("Repetir Contraseña");
    password_repeat_line->setEchoMode(QLineEdit::Password);
    err_repeat = createErrorLabel(main_container);

    // 5. Fecha
    QLabel *dateLabel = new QLabel("Fecha de Nacimiento:", main_container);
    dateLabel->setStyleSheet("font-size: 12px; color: #bdc3c7; margin-top: 5px;");
    date_edit = new QDateEdit(main_container);
    date_edit->setCalendarPopup(true);
    date_edit->setDisplayFormat("dd/MM/yyyy");
    date_edit->setDate(QDate::currentDate());
    err_date = createErrorLabel(main_container);

    // Añadir al layout en orden (Input -> Error Label)
    formLayout->addWidget(username_line);
    formLayout->addWidget(err_username);

    formLayout->addWidget(email_line);
    formLayout->addWidget(err_email);

    formLayout->addWidget(password_line);
    formLayout->addWidget(err_password);

    formLayout->addWidget(password_repeat_line);
    formLayout->addWidget(err_repeat);

    formLayout->addWidget(dateLabel);
    formLayout->addWidget(date_edit);
    formLayout->addWidget(err_date);

    // Botón
    btn_register = new QPushButton("REGISTRARSE", main_container);
    btn_register->setCursor(Qt::PointingHandCursor);
    btn_register->setStyleSheet("QPushButton { background-color: #e67e22; color: white; border-radius: 20px; padding: 12px; font-weight: bold; margin-top: 15px; } QPushButton:hover { background-color: #d35400; }");
    connect(btn_register, &QPushButton::clicked, this, &Sign_Up::sign_up_function);

    // Ensamblaje
    cardLayout->addWidget(title_label);
    cardLayout->addLayout(photoLayout);
    cardLayout->addLayout(formLayout);
    cardLayout->addWidget(btn_register);

    mainLayout->addWidget(main_container);
}

// Helper para validar edad (>16 años)
bool Sign_Up::isOldEnough(QDate birthDate) {
    QDate now = QDate::currentDate();
    QDate sixteenYearsAgo = now.addYears(-16);
    return birthDate <= sixteenYearsAgo;
}

void Sign_Up::sign_up_function() {
    // 1. LIMPIEZA INICIAL: Ocultar errores previos y bordes rojos
    err_username->setVisible(false);
    err_email->setVisible(false);
    err_password->setVisible(false);
    err_repeat->setVisible(false);
    err_date->setVisible(false);

    // Estilo normal
    QString normalStyle = "border: 1px solid #576574;";
    username_line->setStyleSheet(normalStyle);
    email_line->setStyleSheet(normalStyle);
    password_line->setStyleSheet(normalStyle);
    password_repeat_line->setStyleSheet(normalStyle);

    bool isValid = true;
    Navigation &nav = Navigation::instance();

    // --- 2. VALIDACIÓN DE USUARIO ---
    QString user = username_line->text();
    // Regex: Inicio(^), caracteres permitidos [a-z, A-Z, 0-9, -, _], longitud {6,15}, Fin($)
    QRegularExpression userRegex("^[a-zA-Z0-9_-]{6,15}$");

    if (user.isEmpty()) {
        err_username->setText("⚠ El usuario es obligatorio.");
        err_username->setVisible(true);
        username_line->setStyleSheet("border: 1px solid #e74c3c;");
        isValid = false;
    }
    else if (!userRegex.match(user).hasMatch()) {
        err_username->setText("⚠ Debe tener 6-15 caracteres (letras, números, '-' o '_').");
        err_username->setVisible(true);
        username_line->setStyleSheet("border: 1px solid #e74c3c;");
        isValid = false;
    }
    else if (nav.findUser(user)) { // Verificar si ya existe
        err_username->setText("⚠ Este nombre de usuario ya está en uso.");
        err_username->setVisible(true);
        username_line->setStyleSheet("border: 1px solid #e74c3c;");
        isValid = false;
    }

    // --- 3. VALIDACIÓN DE EMAIL ---
    QString email = email_line->text();
    // Regex estándar simple para email
    QRegularExpression mailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
    if (!mailRegex.match(email).hasMatch()) {
        err_email->setText("⚠ Introduce un correo electrónico válido.");
        err_email->setVisible(true);
        email_line->setStyleSheet("border: 1px solid #e74c3c;");
        isValid = false;
    }

    // --- 4. VALIDACIÓN DE CONTRASEÑA ---
    QString pass = password_line->text();
    /* Regex compleja explicada:
       (?=.*[a-z]) -> Al menos una minúscula
       (?=.*[A-Z]) -> Al menos una mayúscula
       (?=.*\\d)   -> Al menos un dígito
       (?=.*[^a-zA-Z\\d]) -> Al menos un caracter especial (no letra ni número)
       .{8,20}     -> Longitud entre 8 y 20
    */
    QRegularExpression passRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[^a-zA-Z\\d]).{8,20}$");

    if (!passRegex.match(pass).hasMatch()) {
        err_password->setText("⚠ 8-20 carac. con: 1 Mayús, 1 Minús, 1 Núm, 1 Especial.");
        err_password->setVisible(true);
        password_line->setStyleSheet("border: 1px solid #e74c3c;");
        isValid = false;
    }

    // --- 5. REPETIR CONTRASEÑA ---
    if (pass != password_repeat_line->text()) {
        err_repeat->setText("⚠ Las contraseñas no coinciden.");
        err_repeat->setVisible(true);
        password_repeat_line->setStyleSheet("border: 1px solid #e74c3c;");
        isValid = false;
    }

    // --- 6. VALIDACIÓN DE EDAD ---
    if (!isOldEnough(date_edit->date())) {
        err_date->setText("⚠ Debes ser mayor de 16 años para registrarte.");
        err_date->setVisible(true);
        isValid = false;
    }

    // SI TODO ESTÁ BIEN, PROCEDEMOS
    if (isValid) {
        // Manejo de imagen (mismo código de antes)
        QImage avatar;
        if(!icon_path.isEmpty()){
            avatar.load(icon_path);
        } else {
            avatar = QImage(100, 100, QImage::Format_RGB32);
            avatar.fill(Qt::lightGray);
        }

        User u(user, email, pass, avatar, date_edit->date());
        nav.addUser(u);

        // Crear sesión inicial vacía
        Session alumno(QDateTime::currentDateTime(), 0, 0);
        nav.addSession(user, alumno);

        QMessageBox::information(this, "Éxito", "Usuario creado correctamente.");
        emit registrationCompleted(user);
        this->accept();
    }
}

// ... El resto de funciones (eventFilter, select_profile_picture) se quedan igual ...
bool Sign_Up::eventFilter(QObject *obj, QEvent *event) {
    if (obj == profile_pic_label && event->type() == QEvent::MouseButtonPress) {
        select_profile_picture();
        return true;
    }
    return QDialog::eventFilter(obj, event);
}

void Sign_Up::select_profile_picture() {
    QString filename = QFileDialog::getOpenFileName(this, "Seleccionar Imagen", "", "Images (*.png *.jpg *.jpeg)");
    if (!filename.isEmpty()) {
        icon_path = filename;
        QString style = QString("QLabel#ProfilePic { "
                                "background-image: url(%1); "
                                "background-position: center; "
                                "background-repeat: no-repeat; "
                                "border: 2px solid #d2b48c; "
                                "border-radius: 50px; "
                                "color: transparent; }").arg(filename);
        profile_pic_label->setStyleSheet(style);
    }
}



