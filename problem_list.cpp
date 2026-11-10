#include "problem_list.h"
#include "ui_problem_list.h"
#include "navigation.h"
// #include "customheaderbar.h" // Eliminado
#include <QVector>
#include <QListWidget>
#include <QScrollArea>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QGraphicsDropShadowEffect>

problem_list::problem_list(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::problem_list)
{
    ui->setupUi(this);

    // Configuración base de la ventana (Estilo Oscuro)
    setWindowTitle("Ejercicios de Navegación");
    resize(600, 800); // Formato vertical cómodo
    this->setStyleSheet("QMainWindow { background-color: #1e272e; }");

    // Inicializamos ventana de edición (aunque sin header no se llamará desde aquí,
    // lo mantenemos por si la lógica externa lo requiere)
    edit_prf_wnd = new edit_profile(this);

    QWidget *centralWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. TÍTULO DE LA SECCIÓN (Sustituye al Header)
    // Creamos un pequeño encabezado integrado en el contenido
    QWidget *titleContainer = new QWidget();
    titleContainer->setStyleSheet("background-color: #1e272e; border-bottom: 2px solid #2f3640;");
    titleContainer->setFixedHeight(60);

    QHBoxLayout *titleLayout = new QHBoxLayout(titleContainer);
    QLabel *lblTitle = new QLabel("LISTA DE PROBLEMAS");
    lblTitle->setStyleSheet("color: #d2b48c; font-size: 18px; font-weight: bold; letter-spacing: 2px;");
    titleLayout->addWidget(lblTitle);
    titleLayout->setAlignment(Qt::AlignCenter);

    mainLayout->addWidget(titleContainer);

    // 2. ÁREA DE SCROLL
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    // Estilo del ScrollArea y la Barra de Desplazamiento
    scrollArea->setStyleSheet(
        "QScrollArea { background-color: transparent; border: none; }"
        "QScrollBar:vertical { border: none; background: #2f3640; width: 10px; margin: 0px; border-radius: 5px; }"
        "QScrollBar::handle:vertical { background: #576574; min-height: 20px; border-radius: 5px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        );

    QWidget *listContainer = new QWidget();
    listContainer->setStyleSheet("background-color: transparent;");

    QVBoxLayout *listLayout = new QVBoxLayout(listContainer);
    listLayout->setAlignment(Qt::AlignTop);
    listLayout->setSpacing(15); // Espacio entre tarjetas
    listLayout->setContentsMargins(20, 20, 20, 20); // Margen externo

    // 3. CARGA DE PROBLEMAS
    Navigation &nav = Navigation::instance();
    QVector<Problem> problems = nav.problems();

    n_problema_display = 1; // Reiniciamos contador visual

    for(int i = 0; i < problems.size(); ++i){
        QWidget *problemWidget = createProblemWidget(problems[i], i);
        listLayout->addWidget(problemWidget);
    }

    listLayout->addStretch(); // Empuje final
    scrollArea->setWidget(listContainer);

    mainLayout->addWidget(scrollArea);
    setCentralWidget(centralWidget);
}

problem_list::~problem_list()
{
    delete ui;
}

// --- GENERADOR DE TARJETAS DE PROBLEMA ---
QWidget* problem_list::createProblemWidget(Problem problem, int index){
    QFrame *problemFrame = new QFrame();
    problemFrame->setFrameShape(QFrame::NoFrame);

    // Estilo "Card" (Tarjeta)
    problemFrame->setStyleSheet(
        "QFrame { "
        "   background-color: #2f3640; "    // Fondo gris azulado
        "   border: 1px solid #d2b48c; "    // Borde Beige
        "   border-radius: 10px; "
        "}"
        "QFrame:hover { "
        "   border: 1px solid #e67e22; "    // Borde Naranja al pasar mouse
        "   background-color: #353b48; "
        "}"
        );

    // Sombra para dar profundidad
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10);
    shadow->setColor(QColor(0,0,0, 80));
    shadow->setOffset(0, 4);
    problemFrame->setGraphicsEffect(shadow);

    QVBoxLayout *frameLayout = new QVBoxLayout(problemFrame);
    frameLayout->setContentsMargins(15, 15, 15, 15);
    frameLayout->setSpacing(10);

    // Cabecera de la tarjeta: "Problema X"
    QLabel *n_problem_label = new QLabel("PROBLEMA " + QString::number(n_problema_display));
    n_problem_label->setStyleSheet("color: #d2b48c; font-weight: bold; font-size: 14px; border: none; background: transparent;");

    // Cuerpo: Texto del problema
    QLabel *questionLabel = new QLabel(problem.text());
    questionLabel->setWordWrap(true);
    questionLabel->setStyleSheet("color: #ecf0f1; font-size: 13px; border: none; background: transparent; margin-bottom: 5px;");

    // Botón de Acción
    QPushButton *viewButton = new QPushButton("IR AL EJERCICIO");
    viewButton->setCursor(Qt::PointingHandCursor);
    viewButton->setMinimumHeight(35);
    viewButton->setStyleSheet(
        "QPushButton { "
        "   background-color: #e67e22; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 17px; " /* Píldora */
        "   font-weight: bold; "
        "   font-size: 12px; "
        "   letter-spacing: 1px;"
        "}"
        "QPushButton:hover { background-color: #d35400; }"
        "QPushButton:pressed { background-color: #a04000; padding-top: 2px; }"
        );

    // Conexión
    connect(viewButton, &QPushButton::clicked, this, [this, index](){
        emit problemSelected(index);
        this->close(); // Opcional: Cerrar la lista al seleccionar
    });

    n_problema_display++;

    frameLayout->addWidget(n_problem_label);
    frameLayout->addWidget(questionLabel);
    frameLayout->addWidget(viewButton, 0, Qt::AlignRight); // Botón alineado a la derecha queda pro

    return problemFrame;
}

void problem_list::handleEditRequest(QString user_sent){
    edit_prf_wnd->setUser(user_sent);
    this->hide();
    edit_prf_wnd->show();
}
