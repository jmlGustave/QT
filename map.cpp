#include "map.h"
#include "ui_map.h"
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QInputDialog>
#include <QColorDialog>
#include <QMenu>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QSvgRenderer>
#include <QtSvgWidgets/QGraphicsSvgItem>
#include <QVector>
#include <QTimer>

map::map(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::map)
    , view(new QGraphicsView(this))
    , scene(new QGraphicsScene(this))
    , isMenuOpen(false)
    , currentProblemIndex(0)
{
    ui->setupUi(this);

    // 1. INICIALIZACIÓN DE VARIABLES
    isAddPointMode = false; isDrawLineMode = false; isCompassMode = false;
    isTextMode = false; isEraserMode = false; isCoordinateMode = false;
    isQuizOpen = false;

    currentLine = nullptr; currentCircle = nullptr;
    protractorItem = nullptr; rulerItem = nullptr; compassItem = nullptr;
    projLineH = nullptr; projLineV = nullptr; cursorLabel = nullptr;


    // --- CARGAR PROBLEMA INICIAL ---
    Navigation &nav = Navigation::instance();
    // Accedemos al vector de problemas
    if (!nav.problems().isEmpty()) {
        problem = nav.problems().first(); // Carga el primero
    }
    // --------------------------------

    // --- 2. ESTRUCTURA PRINCIPAL ---
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0); mainLayout->setSpacing(0);

    headerBar = new CustomHeaderBar(centralWidget);
    headerBar->setFixedHeight(50);
    mainLayout->addWidget(headerBar);

    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setScene(scene);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFrameShape(QFrame::NoFrame);
    mainLayout->addWidget(view);

    setCentralWidget(centralWidget);

    // --- 3. CARGA DEL MAPA ---
    QPixmap map_px(":/map/carta_nautica.jpg");
    if (!map_px.isNull()) {
        QGraphicsPixmapItem *item = scene->addPixmap(map_px);
        item->setZValue(0);
        scene->setSceneRect(item->boundingRect());
    }
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->viewport()->installEventFilter(this);

    // =========================================================
    // 4. BARRA DE HERRAMIENTAS (IZQUIERDA)
    // =========================================================

    menuButton = new QPushButton(view);
    menuButton->setObjectName("sideButton");
    menuButton->setFixedSize(30, 150);
    menuButton->move(0, 0);
    QFont buttonFont = font(); buttonFont.setBold(true); buttonFont.setPointSize(8);
    menuButton->setIcon(createVerticalTextIcon("Herramientas", Qt::black, buttonFont));
    menuButton->setIconSize(QSize(24, 140));
    menuButton->setStyleSheet("#sideButton { background-color: #f0f0f0; border: 2px solid black; border-left: none; border-top-right-radius: 10px; border-bottom-right-radius: 10px; } #sideButton:hover { background-color: #e0e0e0; }");
    menuButton->raise();
    connect(menuButton, &QPushButton::clicked, this, &map::toggleMenu);

    menuContainer = new QWidget(view);
    menuContainer->setObjectName("menuContainer");
    menuContainer->setStyleSheet("background-color: #333333; border: 1px solid black; border-left: none;");
    menuContainer->setVisible(false);

    QVBoxLayout *menuLayout = new QVBoxLayout(menuContainer);
    menuLayout->setContentsMargins(10, 10, 10, 10);
    menuLayout->setSpacing(10);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *menuLabel = new QLabel("Panel de Control", menuContainer);
    menuLabel->setStyleSheet("color: white; font-weight: bold; border: none; font-size: 14px;");
    QPushButton *closeBtn = new QPushButton("✕", menuContainer);
    closeBtn->setFixedSize(20, 20); closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet("QPushButton { background-color: transparent; color: #cccccc; border: none; font-weight: bold; } QPushButton:hover { background-color: #cc0000; color: white; border-radius: 3px; }");
    connect(closeBtn, &QPushButton::clicked, this, &map::toggleMenu);
    headerLayout->addWidget(menuLabel); headerLayout->addStretch(); headerLayout->addWidget(closeBtn);
    menuLayout->addLayout(headerLayout);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(8);
    QString gridBtnStyle = "QPushButton { background-color: #555; color: white; border: 1px solid #444; border-radius: 4px; padding: 8px; min-height: 25px; } QPushButton:hover { background-color: #777; border-color: #666; }";

    // GENERACIÓN DE BOTONES DE HERRAMIENTAS
    for(int i = 0; i < 14; ++i) {
        if (i == 5) { // SpinBox
            sizeSpinBox = new QSpinBox(menuContainer);
            sizeSpinBox->setRange(1, 200); sizeSpinBox->setValue(currentSize);
            sizeSpinBox->setPrefix("Tam: "); sizeSpinBox->setAlignment(Qt::AlignCenter);
            sizeSpinBox->setStyleSheet("QSpinBox { background-color: #555; color: white; border: 1px solid #444; border-radius: 4px; padding: 5px; min-height: 25px; }");
            connect(sizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int val){ currentSize = val; });
            gridLayout->addWidget(sizeSpinBox, i/2, i%2); continue;
        }
        if (i == 6) { // Color
            QPushButton *b = new QPushButton("Color", menuContainer);
            b->setCursor(Qt::PointingHandCursor);
            auto updateBtnStyle = [b](const QColor &c) {
                QString tc = (c.lightness() > 128) ? "black" : "white";
                b->setStyleSheet(QString("QPushButton { background-color: %1; color: %2; border: 1px solid #444; border-radius: 4px; font-weight: bold; min-height: 25px; } QPushButton:hover { border: 1px solid white; }").arg(c.name()).arg(tc));
            };
            updateBtnStyle(currentColor);
            connect(b, &QPushButton::clicked, this, [this, updateBtnStyle](){
                QColor c = QColorDialog::getColor(currentColor, this, "Color");
                if (c.isValid()) { currentColor = c; updateBtnStyle(currentColor); }
            });
            gridLayout->addWidget(b, i/2, i%2); continue;
        }

        QPushButton *btn = nullptr;
        if (i == 7) { btn = new QPushButton("Limpiar", menuContainer); connect(btn, &QPushButton::clicked, this, &map::clearAllItems); btn->setStyleSheet("QPushButton { background-color: #8B0000; color: white; border: 1px solid #444; border-radius: 4px; padding: 8px; font-weight: bold; min-height: 25px; }"); }
        else if (i == 8) { btn = new QPushButton("Zoom (+)", menuContainer); connect(btn, &QPushButton::clicked, this, &map::zoomIn); }
        else if (i == 9) { btn = new QPushButton("Zoom (-)", menuContainer); connect(btn, &QPushButton::clicked, this, &map::zoomOut); }
        else if (i == 10) { btn = new QPushButton("Transp.", menuContainer); connect(btn, &QPushButton::clicked, this, &map::toggleProtractor); }
        else if (i == 11) { btn = new QPushButton("Regla", menuContainer); connect(btn, &QPushButton::clicked, this, &map::toggleRuler); }
        else if (i == 12) { btn = new QPushButton("Compas", menuContainer); connect(btn, &QPushButton::clicked, this, &map::toggleCompass); }
        else if (i == 13) { btn = new QPushButton("Coord.", menuContainer); connect(btn, &QPushButton::clicked, this, &map::activateProjectionTool); }
        else {
            QString name;
            if(i==0) name="Punto"; else if(i==1) name="Línea"; else if(i==2) name="Compás"; else if(i==3) name="Texto"; else name="Borrar";
            btn = new QPushButton(name, menuContainer);
            if(i==0) connect(btn, &QPushButton::clicked, this, &map::activatePointTool);
            else if(i==1) connect(btn, &QPushButton::clicked, this, &map::activateLineTool);
            else if(i==2) connect(btn, &QPushButton::clicked, this, &map::activateCompassTool);
            else if(i==3) connect(btn, &QPushButton::clicked, this, &map::activateTextTool);
            else connect(btn, &QPushButton::clicked, this, &map::activateEraserTool);
        }

        if (btn && i != 7) {
            btn->setStyleSheet(gridBtnStyle);
            btn->setCursor(Qt::PointingHandCursor);
        }
        gridLayout->addWidget(btn, i/2, i%2);
    }
    menuLayout->addLayout(gridLayout);
    menuLayout->addStretch();

    menuAnimation = new QPropertyAnimation(menuContainer, "geometry", this);
    menuAnimation->setDuration(300); menuAnimation->setEasingCurve(QEasingCurve::OutCubic);


    // =========================================================
    // 5. IMPLEMENTACIÓN DEL MENÚ DE PROBLEMAS (DERECHA)
    // =========================================================

    // A. BOTÓN LATERAL DEL QUIZ
    quizButton = new QPushButton(view);
    quizButton->setObjectName("quizButton");
    quizButton->setFixedSize(30, 150);
    quizButton->move(view->width() - 30, 160); // Se ajustará en resizeEvent

    quizButton->setIcon(createVerticalTextIcon("Problemas", Qt::white, buttonFont));
    quizButton->setIconSize(QSize(24, 140));
    quizButton->setStyleSheet(
        "#quizButton { background-color: #0078D7; border: 2px solid black; border-right: none; "
        "border-top-left-radius: 10px; border-bottom-left-radius: 10px; }"
        "#quizButton:hover { background-color: #005a9e; }"
        );
    quizButton->raise();
    connect(quizButton, &QPushButton::clicked, this, &map::toggleQuiz);

    // B. CONTENEDOR DEL QUIZ
    quizContainer = new QWidget(view);
    quizContainer->setObjectName("quizContainer");
    quizContainer->setStyleSheet("background-color: rgba(30, 40, 55, 0.95); border-left: 3px solid #0078D7; color: white;");
    quizContainer->setVisible(false);

    QVBoxLayout *quizLayout = new QVBoxLayout(quizContainer);
    quizLayout->setContentsMargins(20, 30, 20, 30);
    quizLayout->setSpacing(15);

    // --- CABECERA ---
    QLabel *quizTitle = new QLabel("Ejercicio Práctico", quizContainer);
    quizTitle->setAlignment(Qt::AlignCenter);
    quizTitle->setStyleSheet("font-weight: bold; font-size: 20px; color: #4FC3F7; border-bottom: 2px solid #4FC3F7; padding-bottom: 10px;");
    quizLayout->addWidget(quizTitle);

    // --- ENUNCIADO (Se llenará en selectProblem) ---
    questionLabel = new QLabel("Cargando...", quizContainer);
    questionLabel->setWordWrap(true);
    questionLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    questionLabel->setMinimumHeight(180);
    questionLabel->setStyleSheet("QLabel { font-size: 13px; line-height: 1.3; background-color: rgba(0, 0, 0, 0.3); border-radius: 6px; padding: 10px; border: 1px solid #555; }");
    quizLayout->addWidget(questionLabel);

    // --- RESPUESTAS ---
    QWidget *answersWidget = new QWidget(quizContainer);
    QVBoxLayout *answersLayout = new QVBoxLayout(answersWidget);
    answersLayout->setContentsMargins(0, 10, 0, 10);
    answersLayout->setSpacing(15);

    answerGroup = new QButtonGroup(this);

    // Creamos los botones vacíos, se llenarán en selectProblem
    rbA = new QRadioButton("", answersWidget);
    rbB = new QRadioButton("", answersWidget);
    rbC = new QRadioButton("", answersWidget);
    rbD = new QRadioButton("", answersWidget);

    QString rbStyle = "QRadioButton { font-weight: bold; font-size: 12px; padding: 4px; color: #E0E0E0; } "
                      "QRadioButton::indicator { width: 16px; height: 16px; border-radius: 8px; border: 2px solid #aaa; } "
                      "QRadioButton::indicator:checked { background-color: #0078D7; border-color: white; }";

    rbA->setStyleSheet(rbStyle); rbB->setStyleSheet(rbStyle);
    rbC->setStyleSheet(rbStyle); rbD->setStyleSheet(rbStyle);

    // ASIGNACIÓN DE IDs PARA EL VECTOR (1->Index 0, 2->Index 1...)
    answerGroup->addButton(rbA, 1);
    answerGroup->addButton(rbB, 2);
    answerGroup->addButton(rbC, 3);
    answerGroup->addButton(rbD, 4);

    answersLayout->addWidget(rbA); answersLayout->addWidget(rbB);
    answersLayout->addWidget(rbC); answersLayout->addWidget(rbD);
    quizLayout->addWidget(answersWidget);

    // --- FEEDBACK ---
    feedbackLabel = new QLabel("", quizContainer);
    feedbackLabel->setAlignment(Qt::AlignCenter);
    feedbackLabel->setStyleSheet("font-weight: bold; font-size: 16px; margin: 5px;");
    quizLayout->addWidget(feedbackLabel);

    // --- BOTÓN COMPROBAR ---
    QPushButton *checkBtn = new QPushButton("Comprobar Respuesta", quizContainer);
    checkBtn->setCursor(Qt::PointingHandCursor);
    checkBtn->setMinimumHeight(45);
    checkBtn->setStyleSheet("QPushButton { background-color: #27AE60; color: white; border: none; padding: 10px; border-radius: 6px; font-weight: bold; font-size: 14px; } QPushButton:hover { background-color: #2ECC71; }");
    connect(checkBtn, &QPushButton::clicked, this, &map::checkAnswer);
    quizLayout->addWidget(checkBtn);

    // --- NAVEGACIÓN ---
    QHBoxLayout *navLayout = new QHBoxLayout();
    QPushButton *prevBtn = new QPushButton("< Anterior", quizContainer);
    QPushButton *nextBtn = new QPushButton("Siguiente >", quizContainer);
    QString navStyle = "QPushButton { background-color: #555; color: white; border: none; padding: 5px; } QPushButton:hover { background-color: #777; }";
    prevBtn->setStyleSheet(navStyle);
    nextBtn->setStyleSheet(navStyle);

    // CONEXIONES USANDO VARIABLES MIEMBRO
    connect(prevBtn, &QPushButton::clicked, this, [this](){
        if (currentProblemIndex > 0) {
            selectProblem(currentProblemIndex - 1);
        }
    });

    connect(nextBtn, &QPushButton::clicked, this, [this](){
        Navigation &nav = Navigation::instance();
        if (currentProblemIndex < nav.problems().size() - 1) {
            selectProblem(currentProblemIndex + 1);
        }
    });

    navLayout->addWidget(prevBtn); navLayout->addWidget(nextBtn);
    quizLayout->addLayout(navLayout);
    quizLayout->addStretch();

    // Animación
    quizAnimation = new QPropertyAnimation(quizContainer, "geometry", this);
    quizAnimation->setDuration(400); quizAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // --- INICIALIZACIÓN FINAL ---
    // ESTO ES LO MÁS IMPORTANTE: Carga el problema 0 usando toda la lógica
    selectProblem(0);
}

map::~map() { delete ui; }

void map::toggleMenu() {
    QPoint origin(0, 0); int buttonHeight = menuButton->height(); int menuRequiredHeight = menuContainer->layout()->sizeHint().height(); int finalHeight = qMax(buttonHeight, menuRequiredHeight); const int expandedWidth = 200;
    if (isMenuOpen) { menuAnimation->setStartValue(QRect(origin.x(), origin.y(), expandedWidth, finalHeight)); menuAnimation->setEndValue(QRect(origin.x(), origin.y(), 0, finalHeight)); connect(menuAnimation, &QPropertyAnimation::finished, menuContainer, &QWidget::hide, Qt::SingleShotConnection); }
    else { menuContainer->setVisible(true); menuContainer->raise(); menuAnimation->setStartValue(QRect(origin.x(), origin.y(), 0, finalHeight)); menuAnimation->setEndValue(QRect(origin.x(), origin.y(), expandedWidth, finalHeight)); }
    menuAnimation->start(); isMenuOpen = !isMenuOpen;
}

QIcon map::createVerticalTextIcon(const QString &text, const QColor &textColor, const QFont &font) {
    QImage tempImage(100, 100, QImage::Format_ARGB32); QPainter tempPainter(&tempImage); tempPainter.setFont(font); QFontMetrics fm = tempPainter.fontMetrics();
    int textWidth = fm.horizontalAdvance(text); int textHeight = fm.height(); QPixmap pixmap(textHeight + 10, textWidth + 20); pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap); painter.setRenderHint(QPainter::TextAntialiasing); painter.setFont(font); painter.setPen(textColor);
    painter.translate(pixmap.width() / 2, pixmap.height() / 2); painter.rotate(-90); painter.drawText(-textWidth / 2, textHeight / 4, text); painter.end(); return QIcon(pixmap);
}

void map::activatePointTool() { isAddPointMode = true; isEraserMode=false; view->setCursor(Qt::CrossCursor); view->setDragMode(QGraphicsView::NoDrag); if(isMenuOpen) toggleMenu(); }
void map::activateLineTool() { isDrawLineMode = true; isAddPointMode = false; isEraserMode=false; view->setCursor(Qt::CrossCursor); view->setDragMode(QGraphicsView::NoDrag); if(isMenuOpen) toggleMenu(); }
void map::activateCompassTool() { isCompassMode = true; isAddPointMode = false; isDrawLineMode = false; isEraserMode=false; view->setCursor(Qt::CrossCursor); view->setDragMode(QGraphicsView::NoDrag); if(isMenuOpen) toggleMenu(); }
void map::activateTextTool() { isTextMode = true; isAddPointMode = false; isDrawLineMode = false; isCompassMode = false; isEraserMode=false; view->setCursor(Qt::IBeamCursor); view->setDragMode(QGraphicsView::NoDrag); if(isMenuOpen) toggleMenu(); }
void map::activateEraserTool() { isEraserMode = true; isAddPointMode = false; isDrawLineMode = false; isCompassMode = false; isTextMode = false; view->setCursor(Qt::ForbiddenCursor); view->setDragMode(QGraphicsView::NoDrag); if(isMenuOpen) toggleMenu(); }
void map::activateProjectionTool() {
    isCoordinateMode = !isCoordinateMode; // Toggle

    if (!isCoordinateMode) {
        clearProjections(); // Ocultar si desactivamos
        view->setCursor(Qt::ArrowCursor);
    } else {
        // Desactivar otras herramientas
        isAddPointMode=false; isDrawLineMode=false; isCompassMode=false; isTextMode=false; isEraserMode=false;
        view->setCursor(Qt::CrossCursor);
        if(isMenuOpen) toggleMenu();
    }
}

void map::clearProjections() {
    if (projLineH) { scene->removeItem(projLineH); delete projLineH; projLineH = nullptr; }
    if (projLineV) { scene->removeItem(projLineV); delete projLineV; projLineV = nullptr; }
    if (cursorLabel) { scene->removeItem(cursorLabel); delete cursorLabel; cursorLabel = nullptr; }

}

// Convierte decimal a Grados° Minutos' (Formato náutico)
QString map::decimalToDMS(double value, bool isLat) {
    double absVal = std::abs(value);
    int deg = static_cast<int>(absVal);
    double min = (absVal - deg) * 60.0;

    QString dir;
    if (isLat) dir = (value >= 0) ? "N" : "S";
    else       dir = (value >= 0) ? "E" : "W";

    // Ej: 36° 15.42' N
    return QString("%1° %2' %3").arg(deg).arg(min, 0, 'f', 2).arg(dir);
}

void map::drawProjections(QPointF p) {
    clearProjections(); // Limpiar anterior

    // 1. CÁLCULO MATEMÁTICO
    double ratioX = (p.x() - PIXEL_LEFT) / (PIXEL_RIGHT - PIXEL_LEFT);
    double lon = GEO_LON_LEFT + ratioX * (GEO_LON_RIGHT - GEO_LON_LEFT);

    double ratioY = (p.y() - PIXEL_TOP) / (PIXEL_BOTTOM - PIXEL_TOP);
    double lat = GEO_LAT_TOP + ratioY * (GEO_LAT_BOTTOM - GEO_LAT_TOP);

    QPen pen(Qt::red, 1, Qt::DashLine);
    // Para que las líneas no se engorden al hacer zoom, usamos Cosmetic Pen
    pen.setCosmetic(true);

    double imgWidth = scene->sceneRect().width();
    double imgHeight = scene->sceneRect().height();
    projLineH = scene->addLine(0, p.y(), imgWidth, p.y(), pen);
    projLineV = scene->addLine(p.x(), 0, p.x(), imgHeight, pen);

    // 3. DIBUJAR TEXTO
    QFont font("Arial", 10, QFont::Bold);
    QString textStr = QString("Lat: %1\nLon: %2")
                          .arg(decimalToDMS(lat, true))
                          .arg(decimalToDMS(lon, false));

    cursorLabel = scene->addText(textStr, font);
    cursorLabel->setHtml(QString("<div style='background-color:rgba(255, 255, 255, 0.8); padding: 4px; border: 1px solid black;'>%1</div>")
                             .arg(textStr.replace("\n", "<br>")));

    cursorLabel->setZValue(100);

    // --- NUEVO: ESCALADO Y POSICIONAMIENTO ---

    // 1. Aplicar la escala inversa
    updateTextScale();

    // 2. Calcular la posición (Offset)
    // Como el ítem está escalado, 15 píxeles de desplazamiento también se verían afectados.
    // Dividimos el desplazamiento por m_scaleFactor para que visualmente siempre esté a la misma distancia.
    double offset = 15.0 / m_scaleFactor;

    cursorLabel->setPos(p.x() + offset, p.y() - (offset * 2));
}

void map::clearAllItems() {
    if (QMessageBox::question(this, "Limpiar Carta", "¿Borrar todo?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
        QList<QGraphicsItem *> items = scene->items();
        for (QGraphicsItem *item : items) {
            if (item->zValue() > 0) {
                if (item == protractorItem) protractorItem = nullptr;
                if (item == rulerItem) rulerItem = nullptr;
                if (item == currentLine) currentLine = nullptr;
                if (item == currentCircle) currentCircle = nullptr;
                if (item == compassItem) compassItem = nullptr;
                scene->removeItem(item); delete item;
            }
        }
    }
}

void map::zoomIn() {
    if (m_scaleFactor * 1.25 > 5.0) return;

    view->scale(1.25, 1.25);
    m_scaleFactor *= 1.25;

    updateTextScale(); // <--- AÑADIR ESTO
}

void map::zoomOut() {
    if (m_scaleFactor * 0.8 < 0.05) return;

    view->scale(0.8, 0.8);
    m_scaleFactor *= 0.8;

    updateTextScale(); // <--- AÑADIR ESTO
}

// --- TRANSPORTADOR CON ROTACIÓN CTRL+CLICK ---
void map::toggleProtractor() {
    if (protractorItem) { scene->removeItem(protractorItem); delete protractorItem; protractorItem = nullptr; return; }

    // Instanciamos la clase personalizada RotatableSvgItem
    protractorItem = new RotatableSvgItem(":/map/transportador.svg");
    if (!protractorItem->renderer()->isValid()) { delete protractorItem; protractorItem = nullptr; return; }

    QPointF c = view->mapToScene(view->viewport()->rect().center());
    QRectF b = protractorItem->boundingRect();
    protractorItem->setPos(c.x() - b.width()/2, c.y() - b.height()/2);
    // IMPORTANTE: Establecer el centro de rotación
    protractorItem->setTransformOriginPoint(b.center());
    protractorItem->setZValue(5);
    scene->addItem(protractorItem);
}

// --- REGLA CON ROTACIÓN CTRL+CLICK ---
void map::toggleRuler() {
    if (rulerItem) { scene->removeItem(rulerItem); delete rulerItem; rulerItem = nullptr; return; }

    // Instanciamos la clase personalizada RotatableSvgItem
    rulerItem = new RotatableSvgItem(":/map/ruler.svg");
    if (!rulerItem->renderer()->isValid()) { delete rulerItem; rulerItem = nullptr; return; }

    QPointF c = view->mapToScene(view->viewport()->rect().center());
    QRectF b = rulerItem->boundingRect();
    rulerItem->setPos(c.x() - b.width()/2, c.y() - b.height()/2);
    // IMPORTANTE: Establecer el centro de rotación
    rulerItem->setTransformOriginPoint(b.center());
    rulerItem->setZValue(5);
    scene->addItem(rulerItem);
}

void map::toggleCompass() {
    // 1. Si ya existe, lo borramos
    if (compassItem) {
        scene->removeItem(compassItem);
        delete compassItem;
        compassItem = nullptr;
        return;
    }

    // 2. Creamos el nuevo compás compuesto
    compassItem = new CompassPivot();

    // 3. Lo colocamos en el centro de la vista
    QPointF c = view->mapToScene(view->viewport()->rect().center());
    compassItem->setPos(c);

    // 4. Añadimos a la escena (las patas se añaden solas porque son hijas)
    scene->addItem(compassItem);

    // 5. Seleccionar para indicar que está activo
    compassItem->setSelected(true);
}

void map::toggleQuiz()
{
    // Anchura del panel (más ancho para leer bien)
    const int panelWidth = 400;

    // Altura completa de la ventana
    int totalHeight = view->height();
    int totalWidth = view->width();

    if (isQuizOpen) {
        // CERRAR: Se mueve hacia la derecha hasta salir de la pantalla
        // Desde (Width - 400) hasta (Width)
        quizAnimation->setStartValue(QRect(totalWidth - panelWidth, 0, panelWidth, totalHeight));
        quizAnimation->setEndValue(QRect(totalWidth, 0, 0, totalHeight));

        connect(quizAnimation, &QPropertyAnimation::finished, quizContainer, &QWidget::hide, Qt::SingleShotConnection);
    } else {
        // ABRIR: Aparece desde la derecha
        quizContainer->setVisible(true);
        quizContainer->raise();
        // El botón también debe estar encima para no quedar tapado si se solapan
        quizButton->raise();

        // Desde (Width) hasta (Width - 400)
        quizAnimation->setStartValue(QRect(totalWidth, 0, 0, totalHeight));
        quizAnimation->setEndValue(QRect(totalWidth - panelWidth, 0, panelWidth, totalHeight));
    }
    quizAnimation->start();
    isQuizOpen = !isQuizOpen;
}

void map::checkAnswer()
{
    // 1. Obtener ID del botón seleccionado (Configuramos 1, 2, 3, 4 en el constructor)
    int selectedId = answerGroup->checkedId();

    if (selectedId == -1) {
        feedbackLabel->setText("Por favor, selecciona una opción.");
        feedbackLabel->setStyleSheet("color: #F1C40F; font-weight: bold; font-size: 14px;");
        return;
    }

    // 2. Convertir ID a Índice del Vector (0, 1, 2, 3)
    int answerIndex = selectedId - 1;

    // 3. Obtener el problema actual desde la navegación (Fuente de la verdad)
    Navigation &nav = Navigation::instance();
    if (currentProblemIndex < 0 || currentProblemIndex >= nav.problems().size()) return;

    const Problem &currentP = nav.problems().at(currentProblemIndex);
    const QVector<Answer> &answersVec = currentP.answers();

    // 4. Comprobación de seguridad de rango
    if (answerIndex >= 0 && answerIndex < answersVec.size()) {
        // 5. Verificar validez
        if (answersVec.at(answerIndex).validity()) {
            feedbackLabel->setText("¡CORRECTO!");
            feedbackLabel->setStyleSheet("color: #2ECC71; font-weight: bold; font-size: 18px;");
            headerBar->hits += 1;

        } else {
            feedbackLabel->setText("Incorrecto. Inténtalo de nuevo.");
            feedbackLabel->setStyleSheet("color: #E74C3C; font-weight: bold; font-size: 14px;");
            headerBar->faults +=1;
        }
    } else {
        feedbackLabel->setText("Error en los datos de la respuesta.");
    }
}
void map::selectProblem(int n_problem) {
    Navigation &nav = Navigation::instance();
    // Validación de seguridad
    if (nav.problems().isEmpty() || n_problem < 0 || n_problem >= nav.problems().size()) return;

    // 1. Actualizar el índice actual y el objeto problema local
    currentProblemIndex = n_problem;
    this->problem = nav.problems().at(n_problem); // Actualizamos la copia local

    // 2. Actualizar Enunciado
    if (questionLabel) {
        // Añadimos el número de pregunta para claridad (Ej: "1. ¿Cuál es...?")
        questionLabel->setText(QString::number(n_problem + 1) + ". " + problem.text());
    }

    // 3. Obtener vector de respuestas
    QVector<Answer> answers = problem.answers();

    // 4. Resetear selección previa y Feedback
    if (answerGroup) {
        // Truco para desmarcar sin que afecte a la lógica exclusiva después
        answerGroup->setExclusive(false);
        if (answerGroup->checkedButton()) answerGroup->checkedButton()->setChecked(false);
        answerGroup->setExclusive(true);
    }
    if (feedbackLabel) feedbackLabel->setText("");

    // 5. Configurar Radio Buttons según el Vector
    // Si hay respuesta en índice 0, mostramos rbA, si no, lo ocultamos.
    if (rbA) {
        if (answers.size() > 0) { rbA->setText(answers[0].text()); rbA->setVisible(true); }
        else rbA->setVisible(false);
    }

    if (rbB) {
        if (answers.size() > 1) { rbB->setText(answers[1].text()); rbB->setVisible(true); }
        else rbB->setVisible(false);
    }

    if (rbC) {
        if (answers.size() > 2) { rbC->setText(answers[2].text()); rbC->setVisible(true); }
        else rbC->setVisible(false);
    }

    if (rbD) {
        if (answers.size() > 3) { rbD->setText(answers[3].text()); rbD->setVisible(true); }
        else rbD->setVisible(false);
    }
}

bool map::eventFilter(QObject *watched, QEvent *event) {
    if (watched == view->viewport()) {

        // 1. CLIC DERECHO
        if (event->type() == QEvent::MouseButtonPress) {
            if (static_cast<QMouseEvent*>(event)->button() == Qt::RightButton) {
                if (isAddPointMode || isDrawLineMode || isCompassMode || isTextMode || isEraserMode || isCoordinateMode) {
                    isAddPointMode = false; isDrawLineMode = false; isCompassMode = false; isTextMode = false; isEraserMode = false; isCoordinateMode = false;
                    if (currentLine) { scene->removeItem(currentLine); delete currentLine; currentLine = nullptr; }
                    if (currentCircle) { scene->removeItem(currentCircle); delete currentCircle; currentCircle = nullptr; }
                    clearProjections();
                    view->setCursor(Qt::ArrowCursor); view->setDragMode(QGraphicsView::ScrollHandDrag);
                    return true;
                }
            }
        }

        // 2. MENU CONTEXTUAL
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

        // 3. CLIC IZQUIERDO
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *me = static_cast<QMouseEvent*>(event);
            if (me->button() == Qt::LeftButton) {
                QPointF p = view->mapToScene(me->pos());

                if (isCoordinateMode) {
                    drawProjections(p);
                    // Opcional: Dibujar también un punto visual en la intersección
                    return true;
                }

                if (isEraserMode) {
                    QGraphicsItem *it = scene->itemAt(p, view->transform());
                    if (it && it->zValue() > 0) {
                        if (it == protractorItem) protractorItem = nullptr;
                        if (it == rulerItem) rulerItem = nullptr;
                        if (it == compassItem) compassItem = nullptr;
                        scene->removeItem(it); delete it;
                    }
                    return true;
                }
                if (isAddPointMode) {
                    auto pt = scene->addEllipse(p.x()-currentSize/2, p.y()-currentSize/2, currentSize, currentSize, QPen(Qt::black), QBrush(currentColor));
                    pt->setFlag(QGraphicsItem::ItemIsSelectable); pt->setZValue(20); return true;
                }
                if (isDrawLineMode) {
                    currentLine = scene->addLine(p.x(), p.y(), p.x(), p.y(), QPen(currentColor, currentSize, Qt::SolidLine, Qt::RoundCap));
                    currentLine->setFlag(QGraphicsItem::ItemIsSelectable); currentLine->setZValue(10); return true;
                }
                if (isCompassMode) {
                    compassCenter = p;
                    currentCircle = scene->addEllipse(p.x(), p.y(), 0, 0, QPen(currentColor, currentSize, Qt::DashLine));
                    currentCircle->setFlag(QGraphicsItem::ItemIsSelectable); currentCircle->setZValue(10); return true;
                }
                if (isTextMode) {
                    bool ok; QString txt = QInputDialog::getText(this, "Texto", "Msg:", QLineEdit::Normal, "", &ok);
                    if (ok && !txt.isEmpty()) {
                        auto t = scene->addText(txt); t->setPos(p); t->setFont(QFont("Arial", currentSize, QFont::Bold));
                        t->setDefaultTextColor(currentColor); t->setFlag(QGraphicsItem::ItemIsSelectable); t->setZValue(20);
                    }
                    return true;
                }
            }
        }

        // 4. MOUSE MOVE
        else if (event->type() == QEvent::MouseMove) {
            QPointF p = view->mapToScene(static_cast<QMouseEvent*>(event)->pos());
            if (isDrawLineMode && currentLine) { QLineF l=currentLine->line(); l.setP2(p); currentLine->setLine(l); return true; }
            if (isCompassMode && currentCircle) { qreal r=QLineF(compassCenter, p).length(); currentCircle->setRect(compassCenter.x()-r, compassCenter.y()-r, r*2, r*2); return true; }
        }

        // 5. MOUSE RELEASE
        else if (event->type() == QEvent::MouseButtonRelease) {
            if (static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton) {
                if (isDrawLineMode) currentLine = nullptr;
                if (isCompassMode) currentCircle = nullptr;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void map::showEvent(QShowEvent *event) {
    // 1. Llamada base obligatoria
    QMainWindow::showEvent(event);

    // 2. Ajuste de zoom inicial (tu código existente)
    if (isFirstShow) {
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        m_scaleFactor = view->transform().m11();
        isFirstShow = false;
    }

    // 3. CORRECCIÓN DE POSICIÓN DEL BOTÓN
    // Usamos un Timer de 0ms para que esto se ejecute al final de la cola de eventos,
    // asegurando que el layout ya haya calculado el ancho final de 'view'.
    QTimer::singleShot(0, this, [this](){
        if (quizButton && view) {
            // Mueve el botón al borde derecho real
            quizButton->move(view->width() - 30, 160);
            quizButton->raise();
        }

        // Si por alguna razón el panel se abriera al inicio, esto también lo corrige
        if (isQuizOpen && quizContainer) {
            quizContainer->setGeometry(view->width() - 400, 0, 400, view->height());
            quizButton->raise();
        }
    });
}

void map::resizeEvent(QResizeEvent *event)
{
    // 1. Llamamos a la clase base para que ajuste el layout principal
    QMainWindow::resizeEvent(event);

    // 2. Obtenemos el nuevo ancho y alto de la vista
    int w = view->width();
    int h = view->height();

    // 3. PEGAR EL BOTÓN AL BORDE DERECHO
    if (quizButton) {
        // X = AnchoTotal - AnchoBoton (30)
        // Y = 160 (Posición vertical fija, o puedes usar h/2 - 75 para centrarlo verticalmente)
        quizButton->move(w - 30, 160);
        quizButton->raise(); // Aseguramos que siempre esté encima del mapa
    }

    // 4. AJUSTAR EL PANEL DESPLEGABLE (Si está abierto)
    if (isQuizOpen && quizContainer) {
        const int panelWidth = 400;
        // Se mantiene pegado a la derecha: (AnchoTotal - 400, 0)
        quizContainer->setGeometry(w - panelWidth, 0, panelWidth, h);
        quizContainer->raise(); // El panel debe estar encima de todo
        quizButton->raise();    // El botón debe estar encima del panel
    }
}

void map::updateTextScale()
{
    // Evitamos división por cero por seguridad
    if (m_scaleFactor == 0) return;

    // Calculamos la escala inversa
    // Si el mapa es gigante (Zoom 5.0), el texto será pequeño (0.2) para compensar.
    double inverseScale = 1.0 / m_scaleFactor;

    // Aplicamos al texto flotante
    if (cursorLabel) {
        cursorLabel->setScale(inverseScale);
    }

    // Opcional: Aplicar también a las etiquetas de los bordes si quieres que no se deformen
    /*
    if (projLabelLat) projLabelLat->setScale(inverseScale);
    if (projLabelLon) projLabelLon->setScale(inverseScale);
    */
}
