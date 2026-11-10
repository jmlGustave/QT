#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>
#include <QPainter>
#include <QSvgRenderer>
#include <QGraphicsDropShadowEffect> // Necesario para sombras bonitas
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , view(new QGraphicsView(this))
    , scene(new QGraphicsScene(this))
    , isMenuOpen(false)
    , currentProblemIndex(0)
    , s(this)
{
    // --- CONFIGURACIÓN BASE ---
    ui->setupUi(this);
    setWindowTitle("Carta Náutica - Puente de Mando");
    this->setWindowState(Qt::WindowMaximized);
    this->setStyleSheet("QMainWindow { background-color: #1e272e; }");

    // 1. Inicialización
    edit_prf_wnd = new edit_profile(this);
    problem_list_wnd = new problem_list(this);
    history_wnd = new history(this);

    // Variables
    isAddPointMode = false; isDrawLineMode = false; isCompassMode = false;
    isTextMode = false; isEraserMode = false; isCoordinateMode = false;
    isQuizOpen = false;
    m_scaleFactor = 1.0;

    currentLine = nullptr; currentCircle = nullptr;
    protractorItem = nullptr; rulerItem = nullptr; compassItem = nullptr;
    projLineH = nullptr; projLineV = nullptr; cursorLabel = nullptr;

    Navigation &nav = Navigation::instance();
    if (!nav.problems().isEmpty()) problem = nav.problems().first();

    // --- 3. LAYOUT PRINCIPAL ---
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Header Bar + Sombra
    headerBar = new CustomHeaderBar(centralWidget);
    headerBar->setFixedHeight(50);
    QGraphicsDropShadowEffect *headerShadow = new QGraphicsDropShadowEffect();
    headerShadow->setBlurRadius(15); headerShadow->setColor(QColor(0,0,0, 150)); headerShadow->setOffset(0, 5);
    headerBar->setGraphicsEffect(headerShadow);
    mainLayout->addWidget(headerBar);

    // Graphics View
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setScene(scene);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameShape(QFrame::NoFrame);
    mainLayout->addWidget(view);
    setCentralWidget(centralWidget);

    // --- 4. CARGA DEL MAPA ---
    QPixmap map_px(":/map/carta_nautica.jpg");
    if (!map_px.isNull()) {
        QGraphicsPixmapItem *item = scene->addPixmap(map_px);
        item->setZValue(0); scene->setSceneRect(item->boundingRect());
    }
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->viewport()->installEventFilter(this);

    // Estilo Panel Lateral
    QString panelStyle = R"(
        QWidget#SidePanel {
            background-color: rgba(30, 39, 46, 0.98);
            border: 1px solid #d2b48c;
        }
        QLabel { color: #dcdcdc; font-weight: bold; }
        QPushButton {
            background-color: transparent; border: 1px solid #576574; color: #ffffff;
            border-radius: 4px; padding: 6px; text-align: left;
        }
        QPushButton:hover { background-color: #353b48; border-color: #d2b48c; color: #d2b48c; }
        QPushButton:checked, QPushButton:pressed { background-color: #e67e22; color: white; border-color: #e67e22; }
    )";

    // =========================================================
    // 5. MENÚ HERRAMIENTAS (IZQUIERDA) - FINAL
    // =========================================================

    // Botón Pestaña (Trigger)
    menuButton = new QPushButton(view);
    menuButton->setObjectName("sideButton");
    menuButton->setFixedSize(30, 150);
    menuButton->move(0, 100);
    QFont buttonFont = font(); buttonFont.setBold(true); buttonFont.setPointSize(9);
    menuButton->setIcon(createVerticalTextIcon("HERRAMIENTAS", QColor("#d2b48c"), buttonFont));
    menuButton->setIconSize(QSize(24, 140));
    menuButton->setStyleSheet("#sideButton { background-color: #1e272e; border: 2px solid #d2b48c; border-left: none; border-top-right-radius: 10px; border-bottom-right-radius: 10px; } #sideButton:hover { background-color: #2f3640; }");
    menuButton->raise();
    connect(menuButton, &QPushButton::clicked, this, &MainWindow::toggleMenu);

    // Contenedor Principal
    menuContainer = new QWidget(view);
    menuContainer->setObjectName("SidePanel");
    menuContainer->setStyleSheet(panelStyle);
    menuContainer->setVisible(false);

    QVBoxLayout *menuLayout = new QVBoxLayout(menuContainer);
    menuLayout->setContentsMargins(15, 15, 15, 15);
    menuLayout->setSpacing(5);

    // --- CABECERA DEL PANEL ---
    QHBoxLayout *headerToolsLayout = new QHBoxLayout();
    QLabel *menuLabel = new QLabel("PANEL DE CONTROL", menuContainer);
    menuLabel->setStyleSheet("font-size: 16px; color: #d2b48c; font-weight: bold;");

    QPushButton *closeBtn = new QPushButton("✕", menuContainer);
    closeBtn->setFixedSize(30, 30);
    // CAMBIO: Estilo minimalista igual al del Quiz
    closeBtn->setStyleSheet(
        "QPushButton { background-color: transparent; border: none; color: #bdc3c7; font-size: 16px; }"
        "QPushButton:hover { color: #e74c3c; }" // Rojo solo al pasar el mouse
        );
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &MainWindow::toggleMenu);

    headerToolsLayout->addWidget(menuLabel);
    headerToolsLayout->addStretch();
    headerToolsLayout->addWidget(closeBtn);
    menuLayout->addLayout(headerToolsLayout);

    // Separador principal
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #d2b48c;");
    menuLayout->addWidget(line);
    menuLayout->addSpacing(10);

    // --- FUNCIÓN HELPER ---
    auto addSectionTitle = [&](QString title) {
        QLabel *l = new QLabel(title, menuContainer);
        l->setStyleSheet("color: #95a5a6; font-size: 11px; font-weight: bold; letter-spacing: 1px; margin-top: 5px; margin-bottom: 2px; text-transform: uppercase; border-bottom: 1px solid #576574; padding-bottom: 2px;");
        menuLayout->addWidget(l);
    };

    auto createBtn = [&](QString text, int id) -> QPushButton* {
        QPushButton *btn = new QPushButton(text, menuContainer);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setMinimumHeight(32);
        // Conexiones
        if(id==0) connect(btn, &QPushButton::clicked, this, &MainWindow::activatePointTool);
        else if(id==1) connect(btn, &QPushButton::clicked, this, &MainWindow::activateLineTool);
        else if(id==2) connect(btn, &QPushButton::clicked, this, &MainWindow::activateCompassTool);
        else if(id==3) connect(btn, &QPushButton::clicked, this, &MainWindow::activateTextTool);
        else if(id==4) connect(btn, &QPushButton::clicked, this, &MainWindow::activateEraserTool);
        else if(id==7) {
            connect(btn, &QPushButton::clicked, this, &MainWindow::clearAllItems);
            btn->setStyleSheet("QPushButton { border: 1px solid #c0392b; color: #e74c3c; } QPushButton:hover { background-color: #c0392b; color: white; }");
        }
        else if(id==8) connect(btn, &QPushButton::clicked, this, &MainWindow::zoomIn);
        else if(id==9) connect(btn, &QPushButton::clicked, this, &MainWindow::zoomOut);
        else if(id==10) connect(btn, &QPushButton::clicked, this, &MainWindow::toggleProtractor);
        else if(id==11) connect(btn, &QPushButton::clicked, this, &MainWindow::toggleRuler);
        else if(id==12) connect(btn, &QPushButton::clicked, this, &MainWindow::toggleCompass);
        else if(id==13) connect(btn, &QPushButton::clicked, this, &MainWindow::activateProjectionTool);
        return btn;
    };

    // ================= GRUPO 1: TRAZADO (Dibujo) =================
    addSectionTitle("Trazado y Anotación");
    QGridLayout *gridDraw = new QGridLayout();
    gridDraw->setSpacing(8);

    // Fila 0
    gridDraw->addWidget(createBtn("● Punto", 0), 0, 0);
    gridDraw->addWidget(createBtn("/ Línea", 1), 0, 1);
    // Fila 1
    gridDraw->addWidget(createBtn("◎ Compás", 2), 1, 0);
    gridDraw->addWidget(createBtn("T Texto", 3), 1, 1);

    // CAMBIO: Añadimos Color y Grosor AL MISMO GRID para que hereden el ancho total
    // Fila 2: Botón Color (Ocupa 2 columnas -> width completo)
    QPushButton *colorBtn = new QPushButton("🎨 Color del Trazo", menuContainer);
    colorBtn->setCursor(Qt::PointingHandCursor);
    colorBtn->setMinimumHeight(32); // Misma altura que los otros
    connect(colorBtn, &QPushButton::clicked, this, [this, colorBtn](){
        QColor c = QColorDialog::getColor(currentColor, this, "Color del Trazo");
        if (c.isValid()) {
            currentColor = c;
            colorBtn->setStyleSheet(QString("background-color: %1; color: %2; border: 1px solid #d2b48c;").arg(c.name()).arg(c.lightness()>128?"black":"white"));
        }
    });
    // addWidget(widget, fila, columna, rowSpan, colSpan)
    gridDraw->addWidget(colorBtn, 2, 0, 1, 2);

    // Fila 3: SpinBox Grosor (Ocupa 2 columnas -> width completo)
    sizeSpinBox = new QSpinBox(menuContainer);
    sizeSpinBox->setRange(1, 50); sizeSpinBox->setValue(currentSize);
    sizeSpinBox->setPrefix("Grosor: ");
    sizeSpinBox->setSuffix(" px");
    sizeSpinBox->setStyleSheet("QSpinBox { background-color: #1e272e; color: #d2b48c; border: 1px solid #576574; padding: 5px; min-height: 25px; border-radius: 4px; } QSpinBox:hover { border-color: #d2b48c; }");
    connect(sizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int val){ currentSize = val; });

    gridDraw->addWidget(sizeSpinBox, 3, 0, 1, 2);

    menuLayout->addLayout(gridDraw);
    menuLayout->addSpacing(10);

    // ================= GRUPO 2: INSTRUMENTOS =================
    addSectionTitle("Instrumentos");
    QGridLayout *gridTools = new QGridLayout();
    gridTools->setSpacing(8);

    gridTools->addWidget(createBtn("📏 Regla", 11), 0, 0);
    gridTools->addWidget(createBtn("∠ Transp.", 10), 0, 1);
    gridTools->addWidget(createBtn("A Compás V.", 12), 1, 0);
    gridTools->addWidget(createBtn("XY Coord.", 13), 1, 1);

    menuLayout->addLayout(gridTools);
    menuLayout->addSpacing(10);

    // ================= GRUPO 3: VISTA Y EDICIÓN =================
    addSectionTitle("Vista y Edición");
    QGridLayout *gridView = new QGridLayout();
    gridView->setSpacing(8);

    gridView->addWidget(createBtn("+ Zoom", 8), 0, 0);
    gridView->addWidget(createBtn("- Zoom", 9), 0, 1);
    gridView->addWidget(createBtn("⌫ Borrador", 4), 1, 0);
    gridView->addWidget(createBtn("☠ Limpiar", 7), 1, 1);

    menuLayout->addLayout(gridView);

    // Relleno final
    menuLayout->addStretch();

    menuAnimation = new QPropertyAnimation(menuContainer, "geometry", this);
    menuAnimation->setDuration(300); menuAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // =========================================================
    // 6. MENÚ PROBLEMAS (DERECHA) - DISEÑO OPTIMIZADO
    // =========================================================

    quizButton = new QPushButton(view);
    quizButton->setFixedSize(30, 150);
    quizButton->setIcon(createVerticalTextIcon("EJERCICIOS", QColor("#d2b48c"), buttonFont));
    quizButton->setIconSize(QSize(24, 140));
    quizButton->setStyleSheet("QPushButton { background-color: #1e272e; border: 2px solid #d2b48c; border-right: none; border-top-left-radius: 10px; border-bottom-left-radius: 10px; } QPushButton:hover { background-color: #2f3640; }");
    quizButton->raise();
    connect(quizButton, &QPushButton::clicked, this, &MainWindow::toggleQuiz);

    quizContainer = new QWidget(view);
    quizContainer->setStyleSheet("QWidget { background-color: rgba(30, 39, 46, 0.98); border-left: 2px solid #d2b48c; color: white; }");
    quizContainer->setVisible(false);

    QVBoxLayout *quizLayout = new QVBoxLayout(quizContainer);
    quizLayout->setContentsMargins(20, 20, 20, 20);
    quizLayout->setSpacing(5); // Espacio MÍNIMO general

    // --- A. CABECERA (Título y Cerrar) ---
    QHBoxLayout *quizHeaderLayout = new QHBoxLayout();
    QLabel *quizTitle = new QLabel("BITÁCORA", quizContainer);
    quizTitle->setStyleSheet("font-size: 18px; color: #d2b48c; font-weight: bold; border: none;");

    QPushButton *closeQuizBtn = new QPushButton("✕", quizContainer);
    closeQuizBtn->setFixedSize(30, 30);
    closeQuizBtn->setCursor(Qt::PointingHandCursor);
    closeQuizBtn->setStyleSheet("QPushButton { background-color: transparent; border: none; color: #bdc3c7; font-size: 16px; } QPushButton:hover { color: #e74c3c; }");
    connect(closeQuizBtn, &QPushButton::clicked, this, &MainWindow::toggleQuiz);

    quizHeaderLayout->addWidget(quizTitle);
    quizHeaderLayout->addStretch();
    quizHeaderLayout->addWidget(closeQuizBtn);
    quizLayout->addLayout(quizHeaderLayout);

    // --- B. BARRA DE HERRAMIENTAS (Lista / Aleatorio) ---
    QHBoxLayout *navBtnLayout = new QHBoxLayout();
    navBtnLayout->setContentsMargins(0, 10, 0, 10); // Un poco de aire arriba y abajo
    QPushButton *btnList = new QPushButton("Lista", quizContainer);
    QPushButton *btnRandom = new QPushButton("Aleatorio", quizContainer);
    QString secBtnStyle = "QPushButton { background-color: transparent; border: 1px solid #d2b48c; color: #d2b48c; padding: 6px; border-radius: 4px; } QPushButton:hover { background-color: #d2b48c; color: #1e272e; }";
    btnList->setStyleSheet(secBtnStyle);
    btnRandom->setStyleSheet(secBtnStyle);
    connect(btnList, &QPushButton::clicked, this, &MainWindow::handleProblemListRequest);
    connect(btnRandom, &QPushButton::clicked, this, &MainWindow::selectRandomProblem);
    navBtnLayout->addWidget(btnList);
    navBtnLayout->addWidget(btnRandom);
    quizLayout->addLayout(navBtnLayout);

    // --- C. ENUNCIADO (EXPANDIBLE) ---
    questionLabel = new QLabel("Cargando...", quizContainer);
    questionLabel->setWordWrap(true);
    questionLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    // ESTO ES CLAVE: expanding vertical para que empuje las respuestas hacia abajo
    // y ocupe todo el hueco libre.
    questionLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    questionLabel->setStyleSheet("background-color: #111519; color: #ecf0f1; border: 1px solid #576574; border-radius: 6px; padding: 12px; font-size: 14px;");
    quizLayout->addWidget(questionLabel);

    // --- D. ZONA INFERIOR COMPACTA (Respuestas + Botón) ---
    // Usamos un layout vertical interno para pegar las respuestas al botón
    QVBoxLayout *bottomBlock = new QVBoxLayout();
    bottomBlock->setSpacing(8); // Espacio pequeño entre respuestas y botón
    bottomBlock->setContentsMargins(0, 10, 0, 0);

    // D.1 Radio Buttons
    answerGroup = new QButtonGroup(this);
    rbA = new QRadioButton(quizContainer); rbB = new QRadioButton(quizContainer);
    rbC = new QRadioButton(quizContainer); rbD = new QRadioButton(quizContainer);

    QString rbStyle = R"(
        QRadioButton {
            color: #bdc3c7;
            font-size: 13px;
            padding: 4px;
            spacing: 8px; /* Espacio entre bolita y texto */
        }
        QRadioButton:hover {
            color: #ffffff;
        }
        /* El círculo sin marcar */
        QRadioButton::indicator {
            width: 16px;
            height: 16px;
            border-radius: 9px;
            border: 2px solid #7f8c8d; /* Gris medio */
            background-color: transparent;
        }
        /* El círculo al pasar el mouse */
        QRadioButton::indicator:hover {
            border-color: #d2b48c; /* Beige */
        }
        /* El círculo MARCADO (Aquí está la mejora de visibilidad) */
        QRadioButton::indicator:checked {
            background-color: #e67e22; /* Relleno Naranja */
            border: 2px solid #d2b48c; /* Borde Beige brillante */
        }
    )";
    rbA->setStyleSheet(rbStyle); rbB->setStyleSheet(rbStyle);
    rbC->setStyleSheet(rbStyle); rbD->setStyleSheet(rbStyle);

    answerGroup->addButton(rbA, 1); answerGroup->addButton(rbB, 2);
    answerGroup->addButton(rbC, 3); answerGroup->addButton(rbD, 4);

    bottomBlock->addWidget(rbA); bottomBlock->addWidget(rbB);
    bottomBlock->addWidget(rbC); bottomBlock->addWidget(rbD);

    // D.2 Feedback (Oculto por defecto para no ocupar espacio)
    feedbackLabel = new QLabel("", quizContainer);
    feedbackLabel->setAlignment(Qt::AlignCenter);
    feedbackLabel->setVisible(false); // <--- IMPORTANTE: Oculto inicialmente
    bottomBlock->addWidget(feedbackLabel);

    // D.3 Botón Verificar
    QPushButton *checkBtn = new QPushButton("VERIFICAR RUMBO", quizContainer);
    checkBtn->setCursor(Qt::PointingHandCursor);
    checkBtn->setMinimumHeight(40);
    checkBtn->setStyleSheet("QPushButton { background-color: #e67e22; color: white; border: none; border-radius: 20px; font-weight: bold; font-size: 14px; letter-spacing: 1px;} QPushButton:hover { background-color: #d35400; } QPushButton:pressed { background-color: #a04000; }");
    connect(checkBtn, &QPushButton::clicked, this, &MainWindow::checkAnswer);
    bottomBlock->addWidget(checkBtn);

    // Añadimos el bloque inferior al layout principal
    quizLayout->addLayout(bottomBlock);

    // --- E. NAVEGACIÓN (Paginación) ---
    QHBoxLayout *navLayout = new QHBoxLayout();
    navLayout->setContentsMargins(0, 15, 0, 0); // Separación del bloque de arriba
    QPushButton *prevBtn = new QPushButton("◀", quizContainer);
    QPushButton *nextBtn = new QPushButton("▶", quizContainer);
    QString navBotStyle = "QPushButton { background-color: #34495e; color: white; border: none; border-radius: 4px; padding: 8px; min-width: 40px; } QPushButton:hover { background-color: #4b6584; }";
    prevBtn->setStyleSheet(navBotStyle); nextBtn->setStyleSheet(navBotStyle);

    connect(prevBtn, &QPushButton::clicked, this, [this](){ if (currentProblemIndex > 0) selectProblem(currentProblemIndex - 1); });
    connect(nextBtn, &QPushButton::clicked, this, [this](){ Navigation &nav = Navigation::instance(); if (currentProblemIndex < nav.problems().size() - 1) selectProblem(currentProblemIndex + 1); });

    navLayout->addStretch();
    navLayout->addWidget(prevBtn);
    navLayout->addWidget(nextBtn);
    navLayout->addStretch();

    quizLayout->addLayout(navLayout);

    // Animación
    quizAnimation = new QPropertyAnimation(quizContainer, "geometry", this);
    quizAnimation->setDuration(400); quizAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    quizLayout->addLayout(navLayout);

    // Animación
    quizAnimation = new QPropertyAnimation(quizContainer, "geometry", this);
    quizAnimation->setDuration(400); quizAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // --- CONEXIONES FINALES ---
    connect(headerBar, &CustomHeaderBar::registrationRequested, this, &MainWindow::handleRegistrationRequest);
    connect(headerBar, &CustomHeaderBar::editRequested, this, &MainWindow::handleEditRequest);
    connect(headerBar, &CustomHeaderBar::historyRequested, this, &MainWindow::handleHistoryRequest);
    connect(&s, &Sign_Up::registrationCompleted, this, &MainWindow::handleRegistrationCompleted);
    connect(problem_list_wnd, &problem_list::problemSelected, this, &MainWindow::onProblemSelectedFromList);

    selectProblem(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// =========================================================
// SLOTS ORIGINALES DE MAINWINDOW
// =========================================================

void MainWindow::handleProfileOption(int optionId){
    // ... tu lógica existente ...
}

void MainWindow::handleRegistrationRequest(){ s.exec(); }
void MainWindow::handleRegistrationCompleted(const QString &usr){ headerBar->logged = true; headerBar->update_bar(usr); }
void MainWindow::handleEditRequest(QString user_sent){ edit_prf_wnd->setUser(user_sent); /*this->hide();*/ edit_prf_wnd->show(); } // No ocultamos el mapa, solo mostramos ventana modal
void MainWindow::handleProblemListRequest(){ problem_list_wnd->show(); }

void MainWindow::handleHistoryRequest(User *u){
    Navigation &nav = Navigation::instance();
    if(u == nullptr){ u = nav.findUser(headerBar->usr); }
    history_wnd->set_hist_usr(u);
    history_wnd->show();
}

// NUEVO SLOT: Para el botón "Aleatorio" dentro del panel
void MainWindow::selectRandomProblem() {
    Navigation &nav = Navigation::instance();
    int total = nav.problems().size();
    if (total > 0) {
        int n_problema = QRandomGenerator::global()->bounded(0, total);
        selectProblem(n_problema);
    }
}

// =========================================================
// LÓGICA DE MAPA (MOVIMIENTOS, TOOLS, ETC)
// =========================================================

void MainWindow::toggleMenu() {
    QPoint origin(0, 0);
    int buttonHeight = menuButton->height();
    int menuRequiredHeight = menuContainer->layout()->sizeHint().height();
    int finalHeight = qMax(buttonHeight, menuRequiredHeight);
    // AUMENTO: Ancho 240 para que quepa bien el título
    const int expandedWidth = 240;

    if (isMenuOpen) {
        menuAnimation->setStartValue(QRect(origin.x(), origin.y(), expandedWidth, finalHeight));
        menuAnimation->setEndValue(QRect(origin.x(), origin.y(), 0, finalHeight));
        connect(menuAnimation, &QPropertyAnimation::finished, menuContainer, &QWidget::hide, Qt::SingleShotConnection);
    } else {
        menuContainer->setVisible(true); menuContainer->raise();
        menuAnimation->setStartValue(QRect(origin.x(), origin.y(), 0, finalHeight));
        menuAnimation->setEndValue(QRect(origin.x(), origin.y(), expandedWidth, finalHeight));
    }
    menuAnimation->start(); isMenuOpen = !isMenuOpen;
}

void MainWindow::toggleQuiz() {
    const int panelWidth = 400;
    int totalHeight = view->height();
    int totalWidth = view->width();

    if (isQuizOpen) {
        // CERRANDO
        quizAnimation->setStartValue(QRect(totalWidth - panelWidth, 0, panelWidth, totalHeight));
        quizAnimation->setEndValue(QRect(totalWidth, 0, 0, totalHeight));
        connect(quizAnimation, &QPropertyAnimation::finished, quizContainer, &QWidget::hide, Qt::SingleShotConnection);

        // MODIFICACIÓN: Mostrar el botón al terminar de cerrar
        connect(quizAnimation, &QPropertyAnimation::finished, quizButton, &QPushButton::show, Qt::SingleShotConnection);

    } else {
        // ABRIENDO
        // MODIFICACIÓN: Ocultar botón inmediatamente
        quizButton->hide();

        quizContainer->setVisible(true); quizContainer->raise();
        quizAnimation->setStartValue(QRect(totalWidth, 0, 0, totalHeight));
        quizAnimation->setEndValue(QRect(totalWidth - panelWidth, 0, panelWidth, totalHeight));
    }
    quizAnimation->start(); isQuizOpen = !isQuizOpen;
}

void MainWindow::selectProblem(int n_problem) {
    Navigation &nav = Navigation::instance();
    if (nav.problems().isEmpty() || n_problem < 0 || n_problem >= nav.problems().size()) return;

    currentProblemIndex = n_problem;
    this->problem = nav.problems().at(n_problem);

    if (questionLabel) questionLabel->setText(QString::number(n_problem + 1) + ". " + problem.text());

    QVector<Answer> answers = problem.answers();
    if (answerGroup) {
        answerGroup->setExclusive(false);
        if (answerGroup->checkedButton()) answerGroup->checkedButton()->setChecked(false);
        answerGroup->setExclusive(true);
    }

    // --- CAMBIO AQUÍ: Ocultar y limpiar el feedback ---
    if (feedbackLabel) {
        feedbackLabel->setText("");
        feedbackLabel->setVisible(false); // Oculta el espacio vacío
    }

    if (rbA) { rbA->setVisible(answers.size()>0); if(answers.size()>0) rbA->setText(answers[0].text()); }
    if (rbB) { rbB->setVisible(answers.size()>1); if(answers.size()>1) rbB->setText(answers[1].text()); }
    if (rbC) { rbC->setVisible(answers.size()>2); if(answers.size()>2) rbC->setText(answers[2].text()); }
    if (rbD) { rbD->setVisible(answers.size()>3); if(answers.size()>3) rbD->setText(answers[3].text()); }
}

void MainWindow::checkAnswer() {
    int selectedId = answerGroup->checkedId();

    // --- CAMBIO AQUÍ: Hacer visible el label antes de poner texto ---
    feedbackLabel->setVisible(true);

    if (selectedId == -1) {
        feedbackLabel->setText("⚠ Selecciona una opción");
        feedbackLabel->setStyleSheet("color: #F1C40F; font-weight: bold; margin-bottom: 5px;");
        return;
    }
    int answerIndex = selectedId - 1;
    Navigation &nav = Navigation::instance();
    const Problem &currentP = nav.problems().at(currentProblemIndex);
    const QVector<Answer> &answersVec = currentP.answers();

    if (answerIndex >= 0 && answerIndex < answersVec.size()) {
        if (answersVec.at(answerIndex).validity()) {
            feedbackLabel->setText("✅ ¡CORRECTO!");
            feedbackLabel->setStyleSheet("color: #2ECC71; font-weight: bold; font-size: 16px; margin-bottom: 5px;");
            headerBar->hits += 1;
        } else {
            feedbackLabel->setText("❌ Incorrecto");
            feedbackLabel->setStyleSheet("color: #E74C3C; font-weight: bold; font-size: 16px; margin-bottom: 5px;");
            headerBar->faults +=1;
        }
    }
}

// =========================================================
// HERRAMIENTAS GRÁFICAS (Igual que en map.cpp)
// =========================================================

QIcon MainWindow::createVerticalTextIcon(const QString &text, const QColor &textColor, const QFont &font) {
    QImage tempImage(100, 100, QImage::Format_ARGB32); QPainter tempPainter(&tempImage); tempPainter.setFont(font); QFontMetrics fm = tempPainter.fontMetrics();
    int textWidth = fm.horizontalAdvance(text); int textHeight = fm.height(); QPixmap pixmap(textHeight + 10, textWidth + 20); pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap); painter.setRenderHint(QPainter::TextAntialiasing); painter.setFont(font); painter.setPen(textColor);
    painter.translate(pixmap.width() / 2, pixmap.height() / 2); painter.rotate(-90); painter.drawText(-textWidth / 2, textHeight / 4, text); painter.end(); return QIcon(pixmap);
}

void MainWindow::activatePointTool() { isAddPointMode = true; isEraserMode=false; view->setCursor(Qt::CrossCursor); view->setDragMode(QGraphicsView::NoDrag); if(isMenuOpen) toggleMenu(); }
void MainWindow::activateLineTool() { isDrawLineMode = true; isAddPointMode = false; isEraserMode=false; view->setCursor(Qt::CrossCursor); view->setDragMode(QGraphicsView::NoDrag); if(isMenuOpen) toggleMenu(); }
void MainWindow::activateCompassTool() { isCompassMode = true; isAddPointMode = false; isDrawLineMode = false; isEraserMode=false; view->setCursor(Qt::CrossCursor); view->setDragMode(QGraphicsView::NoDrag); if(isMenuOpen) toggleMenu(); }
void MainWindow::activateTextTool() { isTextMode = true; isAddPointMode = false; isDrawLineMode = false; isCompassMode = false; isEraserMode=false; view->setCursor(Qt::IBeamCursor); view->setDragMode(QGraphicsView::NoDrag); if(isMenuOpen) toggleMenu(); }
void MainWindow::activateEraserTool() { isEraserMode = true; isAddPointMode = false; isDrawLineMode = false; isCompassMode = false; isTextMode = false; view->setCursor(Qt::ForbiddenCursor); view->setDragMode(QGraphicsView::NoDrag); if(isMenuOpen) toggleMenu(); }
void MainWindow::activateProjectionTool() {
    isCoordinateMode = !isCoordinateMode;
    if (!isCoordinateMode) { clearProjections(); view->setCursor(Qt::ArrowCursor); }
    else { isAddPointMode=false; isDrawLineMode=false; isCompassMode=false; isTextMode=false; isEraserMode=false; view->setCursor(Qt::CrossCursor); if(isMenuOpen) toggleMenu(); }
}
void MainWindow::clearProjections() {
    if (projLineH) { scene->removeItem(projLineH); delete projLineH; projLineH = nullptr; }
    if (projLineV) { scene->removeItem(projLineV); delete projLineV; projLineV = nullptr; }
    if (cursorLabel) { scene->removeItem(cursorLabel); delete cursorLabel; cursorLabel = nullptr; }
}

void MainWindow::drawProjections(QPointF p) {
    clearProjections();
    double ratioX = (p.x() - PIXEL_LEFT) / (PIXEL_RIGHT - PIXEL_LEFT); double lon = GEO_LON_LEFT + ratioX * (GEO_LON_RIGHT - GEO_LON_LEFT);
    double ratioY = (p.y() - PIXEL_TOP) / (PIXEL_BOTTOM - PIXEL_TOP); double lat = GEO_LAT_TOP + ratioY * (GEO_LAT_BOTTOM - GEO_LAT_TOP);
    QPen pen(Qt::red, 1, Qt::DashLine); pen.setCosmetic(true);
    projLineH = scene->addLine(0, p.y(), scene->sceneRect().width(), p.y(), pen);
    projLineV = scene->addLine(p.x(), 0, p.x(), scene->sceneRect().height(), pen);
    QFont font("Arial", 10, QFont::Bold);
    QString textStr = QString("Lat: %1\nLon: %2").arg(decimalToDMS(lat, true)).arg(decimalToDMS(lon, false));
    cursorLabel = scene->addText(textStr, font);
    cursorLabel->setHtml(QString("<div style='background-color:rgba(255, 255, 255, 0.8); padding: 4px; border: 1px solid black;'>%1</div>").arg(textStr.replace("\n", "<br>")));
    cursorLabel->setZValue(100);
    updateTextScale();
    double offset = 15.0 / m_scaleFactor; cursorLabel->setPos(p.x() + offset, p.y() - (offset * 2));
}

QString MainWindow::decimalToDMS(double value, bool isLat) {
    double absVal = std::abs(value); int deg = static_cast<int>(absVal); double min = (absVal - deg) * 60.0;
    QString dir = isLat ? ((value >= 0) ? "N" : "S") : ((value >= 0) ? "E" : "W");
    return QString("%1° %2' %3").arg(deg).arg(min, 0, 'f', 2).arg(dir);
}

void MainWindow::clearAllItems() {
    if (QMessageBox::question(this, "Limpiar Carta", "¿Borrar todo?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
        QList<QGraphicsItem *> items = scene->items();
        for (QGraphicsItem *item : items) {
            if (item->zValue() > 0) {
                if (item == protractorItem) protractorItem = nullptr; if (item == rulerItem) rulerItem = nullptr;
                if (item == currentLine) currentLine = nullptr; if (item == currentCircle) currentCircle = nullptr;
                if (item == compassItem) compassItem = nullptr;
                scene->removeItem(item); delete item;
            }
        }
    }
}
void MainWindow::zoomIn() { if (m_scaleFactor * 1.25 > 5.0) return; view->scale(1.25, 1.25); m_scaleFactor *= 1.25; updateTextScale(); }
void MainWindow::zoomOut() { if (m_scaleFactor * 0.8 < 0.05) return; view->scale(0.8, 0.8); m_scaleFactor *= 0.8; updateTextScale(); }
void MainWindow::updateTextScale() { if (m_scaleFactor == 0) return; if (cursorLabel) cursorLabel->setScale(1.0 / m_scaleFactor); }

void MainWindow::toggleProtractor() {
    if (protractorItem) { scene->removeItem(protractorItem); delete protractorItem; protractorItem = nullptr; return; }
    protractorItem = new RotatableSvgItem(":/map/transportador.svg");
    if (!protractorItem->renderer()->isValid()) { delete protractorItem; protractorItem = nullptr; return; }

    QPointF c = view->mapToScene(view->viewport()->rect().center());
    QRectF b = protractorItem->boundingRect();
    protractorItem->setPos(c.x() - b.width()/2, c.y() - b.height()/2);
    protractorItem->setTransformOriginPoint(b.center());
    protractorItem->setZValue(5);

    // --- NUEVO: PERMITIR SELECCIÓN ---
    protractorItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    protractorItem->setSelected(true); // Seleccionarlo automáticamente al crearlo
    // --------------------------------

    scene->addItem(protractorItem);
}

void MainWindow::toggleRuler() {
    if (rulerItem) { scene->removeItem(rulerItem); delete rulerItem; rulerItem = nullptr; return; }
    rulerItem = new RotatableSvgItem(":/map/ruler.svg");
    if (!rulerItem->renderer()->isValid()) { delete rulerItem; rulerItem = nullptr; return; }

    QPointF c = view->mapToScene(view->viewport()->rect().center());
    QRectF b = rulerItem->boundingRect();
    rulerItem->setPos(c.x() - b.width()/2, c.y() - b.height()/2);
    rulerItem->setTransformOriginPoint(b.center());
    rulerItem->setZValue(5);

    // --- NUEVO: PERMITIR SELECCIÓN ---
    rulerItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
    rulerItem->setSelected(true); // Seleccionarlo automáticamente al crearlo
    // --------------------------------

    scene->addItem(rulerItem);
}
void MainWindow::toggleCompass() {
    if (compassItem) { scene->removeItem(compassItem); delete compassItem; compassItem = nullptr; return; }
    compassItem = new CompassPivot(); QPointF c = view->mapToScene(view->viewport()->rect().center());
    compassItem->setPos(c); scene->addItem(compassItem); compassItem->setSelected(true);
}

// =========================================================
// EVENTOS Y FILTROS
// =========================================================

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == view->viewport()) {
        if (event->type() == QEvent::Wheel) {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);

            // 1. Verificar si hay items seleccionados (Instrumentos)
            QList<QGraphicsItem *> selectedItems = scene->selectedItems();
            bool instrumentRotated = false;

            for (QGraphicsItem *item : selectedItems) {
                // Verificamos si el item seleccionado es uno de nuestros instrumentos
                if (item == protractorItem || item == rulerItem || item == compassItem) {

                    // Definir paso de rotación (ej: 5 grados)
                    qreal angleStep = 5.0;
                    if (wheelEvent->angleDelta().y() < 0) {
                        angleStep = -angleStep;
                    }

                    // Aplicar rotación sobre su punto de origen (ya configurado al crearlos)
                    item->setRotation(item->rotation() + angleStep);
                    instrumentRotated = true;
                }
            }

            // 2. Si rotamos un instrumento, consumimos el evento y salimos
            if (instrumentRotated) {
                return true;
            }

            // 3. Si NO rotamos instrumentos, hacemos ZOOM al mapa
            // Usamos tus funciones existentes zoomIn() y zoomOut()
            if (wheelEvent->angleDelta().y() > 0) {
                zoomIn();
            } else {
                zoomOut();
            }

            return true; // Importante: Consumir el evento para que no haga scroll nativo
        }

        if (event->type() == QEvent::MouseButtonPress) {
            if (static_cast<QMouseEvent*>(event)->button() == Qt::RightButton) {
                if (isAddPointMode || isDrawLineMode || isCompassMode || isTextMode || isEraserMode || isCoordinateMode) {
                    isAddPointMode = false; isDrawLineMode = false; isCompassMode = false; isTextMode = false; isEraserMode = false; isCoordinateMode = false;
                    if (currentLine) { scene->removeItem(currentLine); delete currentLine; currentLine = nullptr; }
                    if (currentCircle) { scene->removeItem(currentCircle); delete currentCircle; currentCircle = nullptr; }
                    clearProjections(); view->setCursor(Qt::ArrowCursor); view->setDragMode(QGraphicsView::ScrollHandDrag); return true;
                }
            }
        }
        if (event->type() == QEvent::ContextMenu) {
            QContextMenuEvent *ctxEvent = static_cast<QContextMenuEvent*>(event);
            if (!isAddPointMode && !isDrawLineMode && !isCompassMode && !isTextMode && !isEraserMode) {
                QGraphicsItem *item = scene->itemAt(view->mapToScene(ctxEvent->pos()), view->transform());
                if (item && item->zValue() > 0) {
                    QMenu menu; QAction *actColor = menu.addAction("Cambiar Color"); QAction *actSize = menu.addAction("Cambiar Tamaño");
                    QAction *sel = menu.exec(ctxEvent->globalPos());
                    if (sel == actColor) {
                        QColor c = QColorDialog::getColor(Qt::white, this);
                        if (c.isValid()) {
                            if (auto l = dynamic_cast<QGraphicsLineItem*>(item)) { QPen p=l->pen(); p.setColor(c); l->setPen(p); }
                            else if (auto t = dynamic_cast<QGraphicsTextItem*>(item)) { t->setDefaultTextColor(c); }
                            else if (auto e = dynamic_cast<QGraphicsEllipseItem*>(item)) { if (e->brush().style()!=Qt::NoBrush) e->setBrush(c); else { QPen p=e->pen(); p.setColor(c); e->setPen(p); }}
                        }
                    } else if (sel == actSize) {
                        bool ok; int val = QInputDialog::getInt(this, "Tamaño", "Val:", 10, 1, 200, 1, &ok);
                        if (ok) {
                            if (auto l = dynamic_cast<QGraphicsLineItem*>(item)) { QPen p=l->pen(); p.setWidth(val); l->setPen(p); }
                            else if (auto t = dynamic_cast<QGraphicsTextItem*>(item)) { QFont f=t->font(); f.setPointSize(val); t->setFont(f); }
                            else if (auto e = dynamic_cast<QGraphicsEllipseItem*>(item)) { if (e->brush().style()!=Qt::NoBrush) { QRectF r=e->rect(); QPointF c=r.center(); e->setRect(c.x()-val/2.0, c.y()-val/2.0, val, val); } else { QPen p=e->pen(); p.setWidth(val); e->setPen(p); }}
                        }
                    }
                    return true;
                }
            }
            return false;
        }
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                QPointF p = view->mapToScene(me->pos());
                if (isCoordinateMode) { drawProjections(p); return true; }
                if (isEraserMode) {
                    QGraphicsItem *it = scene->itemAt(p, view->transform());
                    if (it && it->zValue() > 0) {
                        if (it == protractorItem) protractorItem = nullptr; if (it == rulerItem) rulerItem = nullptr; if (it == compassItem) compassItem = nullptr;
                        scene->removeItem(it); delete it;
                    }
                    return true;
                }
                if (isAddPointMode) { auto pt = scene->addEllipse(p.x()-currentSize/2, p.y()-currentSize/2, currentSize, currentSize, QPen(Qt::black), QBrush(currentColor)); pt->setFlag(QGraphicsItem::ItemIsSelectable); pt->setZValue(20); return true; }
                if (isDrawLineMode) { currentLine = scene->addLine(p.x(), p.y(), p.x(), p.y(), QPen(currentColor, currentSize, Qt::SolidLine, Qt::RoundCap)); currentLine->setFlag(QGraphicsItem::ItemIsSelectable); currentLine->setZValue(10); return true; }
                if (isCompassMode) { compassCenter = p; currentCircle = scene->addEllipse(p.x(), p.y(), 0, 0, QPen(currentColor, currentSize, Qt::DashLine)); currentCircle->setFlag(QGraphicsItem::ItemIsSelectable); currentCircle->setZValue(10); return true; }
                if (isTextMode) { bool ok; QString txt = QInputDialog::getText(this, "Texto", "Msg:", QLineEdit::Normal, "", &ok); if (ok && !txt.isEmpty()) { auto t = scene->addText(txt); t->setPos(p); t->setFont(QFont("Arial", currentSize, QFont::Bold)); t->setDefaultTextColor(currentColor); t->setFlag(QGraphicsItem::ItemIsSelectable); t->setZValue(20); } return true; }
            }
        }
        else if (event->type() == QEvent::MouseMove) {
            QPointF p = view->mapToScene(static_cast<QMouseEvent*>(event)->pos());
            if (isDrawLineMode && currentLine) { QLineF l=currentLine->line(); l.setP2(p); currentLine->setLine(l); return true; }
            if (isCompassMode && currentCircle) { qreal r=QLineF(compassCenter, p).length(); currentCircle->setRect(compassCenter.x()-r, compassCenter.y()-r, r*2, r*2); return true; }
        }
        else if (event->type() == QEvent::MouseButtonRelease) {
            if (static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton) {
                if (isDrawLineMode) currentLine = nullptr;
                if (isCompassMode) currentCircle = nullptr;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    if (isFirstShow) { view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio); m_scaleFactor = view->transform().m11(); isFirstShow = false; }
    QTimer::singleShot(0, this, [this](){
        if (quizButton && view) { quizButton->move(view->width() - 30, 160); quizButton->raise(); }
        if (isQuizOpen && quizContainer) { quizContainer->setGeometry(view->width() - 400, 0, 400, view->height()); quizButton->raise(); }
    });
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    int w = view->width(); int h = view->height();
    if (quizButton) { quizButton->move(w - 30, 100); quizButton->raise(); }
    if (isQuizOpen && quizContainer) { quizContainer->setGeometry(w - 400, 0, 400, h); quizContainer->raise(); quizButton->raise(); }
}

void MainWindow::onProblemSelectedFromList(int index) {
    // 1. Ocultar la lista de problemas
    problem_list_wnd->hide();

    // 2. Cargar el problema seleccionado en el mapa
    selectProblem(index);

    // 3. Asegurar que el menú lateral (Quiz) esté abierto
    if (!isQuizOpen) {
        toggleQuiz(); // Abre el menú si estaba cerrado
    }

    // 4. Traer el mapa al frente (por si acaso)
    this->raise();
    this->activateWindow();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // Verificamos si la barra existe y si hay un usuario logueado
    if (headerBar && headerBar->logged) {
        Navigation &nav = Navigation::instance();

        // Creamos la sesión con la fecha actual y los contadores acumulados
        // Accedemos a 'hits' y 'faults' que están en tu headerBar
        Session s(QDateTime::currentDateTime(), headerBar->hits, headerBar->faults);

        // Guardamos la sesión usando el nombre de usuario almacenado en la barra
        nav.addSession(headerBar->usr, s);

        qDebug() << "Sesión guardada al cerrar la App para:" << headerBar->usr;
    }

    // Permitimos que la ventana se cierre normalmente
    QMainWindow::closeEvent(event);
}
