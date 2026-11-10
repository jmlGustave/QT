#include "history.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>

history::history(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Historial");
    resize(900, 700); // Un poco más ancha para que respiren las tarjetas
    this->setStyleSheet("QMainWindow { background-color: #1e272e; }");

    setupUi();
}

history::~history() {
}

void history::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. TÍTULO DE LA SECCIÓN (Sustituye al HeaderBar)
    QWidget *titleContainer = new QWidget();
    titleContainer->setStyleSheet("background-color: #1e272e; border-bottom: 2px solid #2f3640;");
    titleContainer->setFixedHeight(60);

    QHBoxLayout *titleLayout = new QHBoxLayout(titleContainer);
    QLabel *lblTitle = new QLabel("Historial");
    lblTitle->setStyleSheet("color: #d2b48c; font-size: 18px; font-weight: bold; letter-spacing: 2px;");
    titleLayout->addWidget(lblTitle);
    titleLayout->setAlignment(Qt::AlignCenter);

    mainLayout->addWidget(titleContainer);

    // CONTENEDOR PRINCIPAL
    QWidget *contentContainer = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentContainer);
    contentLayout->setContentsMargins(30, 20, 30, 30); // Ajustamos margen superior
    contentLayout->setSpacing(20);

    // 2. DASHBOARD DE RESUMEN
    QHBoxLayout *dashboardLayout = new QHBoxLayout();
    dashboardLayout->setSpacing(15);

    QWidget *cardSessions = createStatCard("SESIONES", "0", "#d2b48c"); // Beige
    QWidget *cardHits     = createStatCard("ACIERTOS", "0", "#2ecc71"); // Verde
    QWidget *cardFaults   = createStatCard("FALLOS",   "0", "#e74c3c"); // Rojo
    QWidget *cardAvg      = createStatCard("EFICACIA", "0%", "#3498db"); // Azul

    lblTotalSessions = cardSessions->findChild<QLabel*>("ValueLabel");
    lblTotalHits     = cardHits->findChild<QLabel*>("ValueLabel");
    lblTotalFaults   = cardFaults->findChild<QLabel*>("ValueLabel");
    lblAvgAccuracy   = cardAvg->findChild<QLabel*>("ValueLabel");

    dashboardLayout->addWidget(cardSessions);
    dashboardLayout->addWidget(cardHits);
    dashboardLayout->addWidget(cardFaults);
    dashboardLayout->addWidget(cardAvg);

    contentLayout->addLayout(dashboardLayout);

    // 3. BARRA DE FILTRO
    QHBoxLayout *toolsLayout = new QHBoxLayout();

    QLabel *tableTitle = new QLabel("REGISTRO DETALLADO");
    tableTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #bdc3c7; letter-spacing: 1px;");

    toolsLayout->addWidget(tableTitle);
    toolsLayout->addStretch();

    QLabel *lblFilter = new QLabel("Filtrar desde:", this);
    lblFilter->setStyleSheet("color: #bdc3c7; font-weight: bold; margin-right: 5px;");

    date_filter = new QDateEdit(QDate::currentDate().addDays(-7), this);
    date_filter->setCalendarPopup(true);
    date_filter->setDisplayFormat("dd/MM/yyyy");
    date_filter->setFixedWidth(120);
    date_filter->setStyleSheet(
        "QDateEdit { background-color: #2f3640; color: white; border: 1px solid #576574; border-radius: 4px; padding: 5px; }"
        "QDateEdit::drop-down { border: none; width: 20px; }"
        "QCalendarWidget QWidget { background-color: #2f3640; color: white; selection-background-color: #e67e22; }"
        );

    toolsLayout->addWidget(lblFilter);
    toolsLayout->addWidget(date_filter);

    contentLayout->addLayout(toolsLayout);

    // 4. CABECERA DE LA LISTA
    QWidget *listHeader = new QWidget();
    listHeader->setStyleSheet("background-color: #2f3640; border-top-left-radius: 4px; border-top-right-radius: 4px; border-bottom: 2px solid #576574;");
    listHeader->setFixedHeight(40);
    QHBoxLayout *headerLayout = new QHBoxLayout(listHeader);
    headerLayout->setContentsMargins(20, 0, 20, 0);

    auto createHeaderLbl = [](QString text, int width = 0) {
        QLabel *l = new QLabel(text);
        l->setStyleSheet("color: #d2b48c; font-weight: bold; font-size: 13px; text-transform: uppercase;");
        if (width > 0) l->setFixedWidth(width);
        return l;
    };

    headerLayout->addWidget(createHeaderLbl("Fecha", 120));
    headerLayout->addWidget(createHeaderLbl("Hora", 100));
    headerLayout->addStretch();
    headerLayout->addWidget(createHeaderLbl("Eficacia", 80));
    headerLayout->addWidget(createHeaderLbl("Aciertos", 80));
    headerLayout->addWidget(createHeaderLbl("Fallos", 80));

    contentLayout->addWidget(listHeader);

    // 5. LISTA
    listHist = new QListWidget(this);
    listHist->setFrameShape(QFrame::NoFrame);
    // Ajuste de estilo para scrollbar y items
    listHist->setStyleSheet(
        "QListWidget { background-color: rgba(0,0,0,0.2); border-bottom-left-radius: 4px; border-bottom-right-radius: 4px; outline: none; }"
        "QListWidget::item { background-color: transparent; border-bottom: 1px solid #353b48; }"
        "QListWidget::item:hover { background-color: rgba(255,255,255,0.05); }"
        "QScrollBar:vertical { border: none; background: #2f3640; width: 10px; margin: 0px; }"
        "QScrollBar::handle:vertical { background: #576574; min-height: 20px; border-radius: 5px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        );
    listHist->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    contentLayout->addWidget(listHist);

    mainLayout->addWidget(contentContainer);
    setCentralWidget(centralWidget);

    connect(date_filter, &QDateEdit::dateChanged, this, &history::filter);
}

// Función auxiliar para crear las tarjetas del Dashboard
QWidget* history::createStatCard(QString title, QString value, QString colorInfo) {
    QWidget *card = new QWidget();
    card->setObjectName("StatCard");
    card->setStyleSheet(QString(
                            "QWidget#StatCard { background-color: #2f3640; border-left: 4px solid %1; border-radius: 6px; }"
                            ).arg(colorInfo));

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(5);

    QLabel *lblTitle = new QLabel(title);
    lblTitle->setStyleSheet("color: #95a5a6; font-size: 11px; font-weight: bold; letter-spacing: 1px;");

    QLabel *lblValue = new QLabel(value);
    lblValue->setObjectName("ValueLabel"); // Para encontrarlo después
    lblValue->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");

    layout->addWidget(lblTitle);
    layout->addWidget(lblValue);

    // Sombra sutil
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10); shadow->setColor(QColor(0,0,0, 80)); shadow->setOffset(0, 3);
    card->setGraphicsEffect(shadow);

    return card;
}

void history::set_hist_usr(User *u){
    usr = u;
    addStatistics();
    updateSummary(); // Calcular totales al cargar usuario
}

void history::updateSummary() {
    if (!usr) return;

    QVector<Session> sessions = usr->sessions();
    int totalS = sessions.size();
    int totalH = 0;
    int totalF = 0;

    for(const auto& s : sessions) {
        totalH += s.hits();
        totalF += s.faults();
    }

    // Calcular eficacia global
    float globalAcc = 0;
    if ((totalH + totalF) > 0) {
        globalAcc = (float)totalH / (float)(totalH + totalF) * 100.0f;
    }

    // Actualizar etiquetas
    if (lblTotalSessions) lblTotalSessions->setText(QString::number(totalS));
    if (lblTotalHits) lblTotalHits->setText(QString::number(totalH));
    if (lblTotalFaults) lblTotalFaults->setText(QString::number(totalF));
    if (lblAvgAccuracy) {
        lblAvgAccuracy->setText(QString::number(globalAcc, 'f', 1) + "%");
        // Color dinámico para el porcentaje
        QString color = (globalAcc >= 50) ? "#2ecc71" : "#e74c3c";
        lblAvgAccuracy->setStyleSheet(QString("color: %1; font-size: 24px; font-weight: bold;").arg(color));
    }
}

void history::addStatistics(){
    listHist->clear();
    // Invertimos la lista para mostrar lo más reciente primero (muy común en historiales)
    QVector<Session> sessions = usr->sessions();

    // Recorremos a la inversa
    for (int i = sessions.size() - 1; i >= 0; --i){
        const auto& session = sessions[i];
        QListWidgetItem *item = new QListWidgetItem(listHist);
        QWidget *row = createRow(session);

        item->setSizeHint(row->sizeHint());
        listHist->setItemWidget(item, row);
        item->setData(Qt::UserRole, session.timeStamp().date());
    }
}

QWidget *history::createRow(Session session){
    int hits = session.hits();
    int faults = session.faults();
    float percent_f = (hits + faults != 0) ? (static_cast<float>(hits) / (hits + faults)) * 100.0f : 0.0f;

    // Widget fila
    QWidget *rowWidget = new QWidget();
    // Estilo: Fondo oscuro ligeramente distinto, borde inferior sutil
    rowWidget->setStyleSheet(
        "QWidget { background-color: rgba(47, 54, 64, 0.6); border-bottom: 1px solid #353b48; border-radius: 4px; }"
        "QWidget:hover { background-color: #353b48; border-left: 2px solid #d2b48c; }" // Efecto hover
        );

    QHBoxLayout *layout = new QHBoxLayout(rowWidget);
    layout->setContentsMargins(20, 15, 20, 15); // Padding generoso
    layout->setSpacing(10);

    // 1. Fecha y Hora
    QDateTime dt = session.timeStamp();
    QLabel *lblDate = new QLabel(dt.toString("dd/MM/yyyy"));
    lblDate->setStyleSheet("color: white; font-weight: bold; background: transparent; border: none;");
    lblDate->setFixedWidth(120);

    QLabel *lblTime = new QLabel(dt.toString("HH:mm"));
    lblTime->setStyleSheet("color: #95a5a6; background: transparent; border: none;");
    lblTime->setFixedWidth(100);

    // 2. Porcentaje (Visualmente destacado)
    QLabel *lblPercent = new QLabel(QString::number(percent_f, 'f', 1) + "%");
    QString pColor = (percent_f >= 50) ? "#2ecc71" : "#e74c3c"; // Verde o Rojo
    lblPercent->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 14px; background: transparent; border: none;").arg(pColor));
    lblPercent->setAlignment(Qt::AlignCenter);
    lblPercent->setFixedWidth(80);

    // 3. Aciertos y Fallos
    auto createBadge = [](int val, QString color) {
        QLabel *l = new QLabel(QString::number(val));
        // Estilo "Badge" (Pastilla)
        l->setStyleSheet(QString("background-color: transparent; color: %1; font-weight: bold; border: none;").arg(color));
        l->setAlignment(Qt::AlignCenter);
        l->setFixedWidth(80);
        return l;
    };

    QLabel *lblHits = createBadge(hits, "#2ecc71"); // Verde
    QLabel *lblFaults = createBadge(faults, "#e74c3c"); // Rojo

    // Ensamblaje
    layout->addWidget(lblDate);
    layout->addWidget(lblTime);
    layout->addStretch();
    layout->addWidget(lblPercent);
    layout->addWidget(lblHits);
    layout->addWidget(lblFaults);

    return rowWidget;
}

void history::filter(const QDate &date){
    for (int i = 0; i < listHist->count(); i++){
        QListWidgetItem *item = listHist->item(i);
        QDate dateItem = item->data(Qt::UserRole).toDate();
        // Filtro simple: Mostrar si es posterior o igual a la fecha seleccionada
        bool show = (dateItem >= date);
        item->setHidden(!show);
    }
}
