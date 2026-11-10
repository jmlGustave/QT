#ifndef MAP_H
#define MAP_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QLabel>
#include <QVBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QSpinBox>
#include <QGraphicsObject>
#include <QtMath>
#include <QPen>
#include <QBrush>
#include <QCursor>
#include <QPainter>
#include <QShowEvent>
#include <QRadioButton>
#include <QButtonGroup>

// Includes para eventos de gráficos
#include <QGraphicsSceneMouseEvent>
#include <QtSvgWidgets/QGraphicsSvgItem> // Qt 6
// #include <QGraphicsSvgItem> // Qt 5

#include "customheaderbar.h"

QT_BEGIN_NAMESPACE
namespace Ui { class map; }
QT_END_NAMESPACE

#include <QGraphicsObject> // Necesario para objetos personalizados
#include <QPainter>
#include <QtMath> // Para qAtan2 y qRadiansToDegrees

// =========================================================
// CLASE 1: PATA DEL COMPÁS (Grande y Rotación Suave)
// =========================================================
class CompassLeg : public QGraphicsObject {
public:
    CompassLeg(QGraphicsItem *parent = nullptr) : QGraphicsObject(parent) {
        setFlag(ItemIsSelectable, false);
        setFlag(ItemIsMovable, false);
        setAcceptHoverEvents(true);
        startMouseAngle = 0;
        startItemRotation = 0;
    }

    // AUMENTAMOS EL TAMAÑO DE LA PATA (300px de largo)
    QRectF boundingRect() const override {
        // Ancho 40, Alto 320. Margen de seguridad para el dibujo.
        return QRectF(-20, 0, 40, 320);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option); Q_UNUSED(widget);
        painter->setRenderHint(QPainter::Antialiasing);

        // Cuerpo de la pata (Más ancho y largo)
        QPolygonF legBody;
        // Coordenadas escaladas para hacerlo más grande
        legBody << QPointF(-12, 0)   // Arriba Izq
                << QPointF(12, 0)    // Arriba Der
                << QPointF(8, 270)   // Abajo Der (antes de la punta)
                << QPointF(-8, 270); // Abajo Izq (antes de la punta)

        // Degradado para efecto metálico/3D
        QLinearGradient grad(0, 0, 20, 0);
        grad.setColorAt(0, QColor(220, 220, 220));
        grad.setColorAt(1, QColor(150, 150, 150));
        painter->setBrush(grad);

        painter->setPen(QPen(Qt::black, 1));
        painter->drawPolygon(legBody);

        // Punta (Acero oscuro y afilada)
        QPolygonF legTip;
        legTip << QPointF(-8, 270)
               << QPointF(8, 270)
               << QPointF(0, 300); // Punta final a 300px

        painter->setBrush(QColor(80, 80, 80)); // Gris oscuro casi negro
        painter->drawPolygon(legTip);
    }

protected:
    // --- LÓGICA DE ROTACIÓN SUAVE (DELTA) ---

    // Función auxiliar para calcular el ángulo del ratón
    double getAngle(const QPointF &pos) {
        // qAtan2 devuelve radianes, convertimos a grados.
        // Sumamos 90 grados porque en Qt el eje Y positivo es hacia abajo (90 grados),
        // y nuestra pata se dibuja hacia abajo. Esto alinea el 0 matemático con el 0 visual de la pata.
        return qRadiansToDegrees(qAtan2(pos.y(), pos.x())) - 90;
    }

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            setCursor(Qt::ClosedHandCursor);
            // 1. Guardamos el ángulo donde hicimos clic
            startMouseAngle = getAngle(event->pos());
            // 2. Guardamos la rotación que tenía la pata en ese momento
            startItemRotation = rotation();
            event->accept();
        }
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->buttons() & Qt::LeftButton) {
            // 3. Calculamos el ángulo actual
            double currentMouseAngle = getAngle(event->pos());

            // 4. La diferencia (delta) es cuánto has movido el ratón
            double delta = currentMouseAngle - startMouseAngle;

            // 5. Aplicamos esa diferencia a la rotación original
            // Esto evita saltos bruscos si haces clic lejos del centro de la pata
            setRotation(startItemRotation + delta);
        }
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override {
        setCursor(Qt::OpenHandCursor); // Cambia a mano abierta para indicar que se puede agarrar
        QGraphicsObject::mouseReleaseEvent(event);
    }

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override {
        setCursor(Qt::OpenHandCursor);
        QGraphicsObject::hoverEnterEvent(event);
    }
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override {
        setCursor(Qt::ArrowCursor);
        QGraphicsObject::hoverLeaveEvent(event);
    }

private:
    double startMouseAngle;
    double startItemRotation;
};

// =========================================================
// CLASE 2: PIVOTE CENTRAL (Más grande)
// =========================================================
class CompassPivot : public QGraphicsObject {
public:
    CompassPivot() {
        setFlags(ItemIsMovable | ItemIsSelectable);
        setZValue(50);

        leftLeg = new CompassLeg(this);
        rightLeg = new CompassLeg(this);

        // Ángulos iniciales más abiertos debido al tamaño
        leftLeg->setRotation(25);
        rightLeg->setRotation(-25);
    }

    ~CompassPivot() { delete leftLeg; delete rightLeg; }

    // AUMENTAMOS EL TAMAÑO DEL PIVOTE
    QRectF boundingRect() const override {
        return QRectF(-25, -25, 50, 50); // 50x50 píxeles
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option); Q_UNUSED(widget);
        painter->setRenderHint(QPainter::Antialiasing);

        // Dibujar el círculo del pivote con un degradado radial para que parezca una esfera
        QRadialGradient grad(0, 0, 25);
        grad.setColorAt(0, QColor(240, 240, 240)); // Brillo centro
        grad.setColorAt(1, QColor(100, 100, 100)); // Oscuro bordes

        painter->setBrush(grad);
        painter->setPen(QPen(Qt::black, 2));
        painter->drawEllipse(boundingRect());

        // Un tornillo pequeño en el centro para realismo
        painter->setBrush(Qt::darkGray);
        painter->drawEllipse(QRectF(-5, -5, 10, 10));
    }

    CompassLeg *leftLeg;
    CompassLeg *rightLeg;
};

// =========================================================
// CLASE: ITEM SVG ROTABLE (Con Ctrl + Click)
// =========================================================
class RotatableSvgItem : public QGraphicsSvgItem {
public:
    RotatableSvgItem(const QString &fileName) : QGraphicsSvgItem(fileName) {
        // Permitimos mover y seleccionar por defecto
        setFlags(ItemIsMovable | ItemIsSelectable);
        isRotating = false;
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        // DETECTAR CTRL + CLIC IZQUIERDO
        if (event->button() == Qt::LeftButton && (event->modifiers() & Qt::ControlModifier)) {
            isRotating = true;

            // Cambiamos cursor para indicar rotación
            setCursor(Qt::ClosedHandCursor);

            // Calculamos el ángulo inicial del ratón respecto al centro del objeto
            // IMPORTANTE: setTransformOriginPoint debe estar en el centro (lo hacemos al crearlo)
            QPointF center = boundingRect().center();
            QPointF pos = event->pos();
            startMouseAngle = qRadiansToDegrees(qAtan2(pos.y() - center.y(), pos.x() - center.x()));

            // Guardamos la rotación actual del objeto
            startItemRotation = rotation();

            event->accept(); // Consumimos el evento para que no mueva el objeto
        } else {
            // Comportamiento normal (mover/seleccionar)
            setCursor(Qt::ArrowCursor);
            QGraphicsSvgItem::mousePressEvent(event);
        }
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override {
        if (isRotating) {
            // Lógica de rotación
            QPointF center = boundingRect().center();
            QPointF pos = event->pos();
            double currentMouseAngle = qRadiansToDegrees(qAtan2(pos.y() - center.y(), pos.x() - center.x()));

            // La nueva rotación es la inicial + la diferencia de ángulo del ratón
            setRotation(startItemRotation + (currentMouseAngle - startMouseAngle));

            event->accept();
        } else {
            QGraphicsSvgItem::mouseMoveEvent(event);
        }
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override {
        if (isRotating) {
            isRotating = false;
            setCursor(Qt::ArrowCursor);
            // No llamamos a la base para evitar efectos secundarios al soltar
        } else {
            QGraphicsSvgItem::mouseReleaseEvent(event);
        }
    }

private:
    bool isRotating;
    double startMouseAngle;
    double startItemRotation;
};

// =========================================================
// CLASE PRINCIPAL
// =========================================================
class map : public QMainWindow
{
    Q_OBJECT

public:
    map(QWidget *parent = nullptr);
    ~map();

    void selectProblem(int n_problem = 4);

private slots:
    void toggleMenu();
    void activatePointTool();
    void activateLineTool();
    void activateCompassTool();
    void activateTextTool();
    void activateEraserTool();
    void clearAllItems();
    void zoomIn();
    void zoomOut();
    void toggleProtractor();
    void toggleRuler();
    void toggleCompass();
    void activateProjectionTool();
    void toggleQuiz();
    void checkAnswer();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    Ui::map *ui;

    QGraphicsView *view;
    QGraphicsScene *scene;
    double m_scaleFactor;

    QPushButton *menuButton;
    QWidget *menuContainer;
    QPropertyAnimation *menuAnimation;
    bool isMenuOpen;
    CustomHeaderBar *headerBar;

    bool isAddPointMode;
    bool isDrawLineMode;
    bool isCompassMode;
    bool isTextMode;
    bool isEraserMode;

    QGraphicsLineItem *currentLine;
    QGraphicsEllipseItem *currentCircle;
    QPointF compassCenter;

    int currentSize = 10;
    QSpinBox *sizeSpinBox;
    QColor currentColor = Qt::red;

    // AHORA AMBOS USAN LA NUEVA CLASE ROTABLE
    RotatableSvgItem *protractorItem;
    RotatableSvgItem *rulerItem;
    CompassPivot *compassItem;

    bool isFirstShow = true;

    bool isCoordinateMode = false;
    QGraphicsLineItem *projLineH = nullptr; // Horizontal
    QGraphicsLineItem *projLineV = nullptr; // Vertical
    QGraphicsTextItem *cursorLabel = nullptr;

    // --- DATOS DE CALIBRACIÓN (CONSTANTES) ---
    // NOTA: Debes ajustar estos valores mirando tu imagen real
    const double PIXEL_LEFT   = 328.0;   // Margen izquierdo en px
    const double PIXEL_RIGHT  = 8367.0; // Margen derecho en px (ancho imagen - margen)
    const double PIXEL_TOP    = 444.0;   // Margen superior en px
    const double PIXEL_BOTTOM = 5345.0; // Margen inferior en px

    const double GEO_LAT_TOP    = 36.20; // 36° 30' N
    const double GEO_LAT_BOTTOM = 35.35; // 36° 00' N
    const double GEO_LON_LEFT   = -6.20; // 6° 30' W
    const double GEO_LON_RIGHT  = -4.55; // 6° 00' W

    QPushButton *quizButton;       // Botón lateral para abrir
    QWidget *quizContainer;        // El panel desplegable
    QPropertyAnimation *quizAnimation;
    bool isQuizOpen = false;

    // Componentes internos del Quiz
    QLabel *questionLabel;         // Enunciado
    QRadioButton *rbA;             // Opción A
    QRadioButton *rbB;             // Opción B
    QRadioButton *rbC;             // Opción C
    QRadioButton *rbD;             // Opción D
    QButtonGroup *answerGroup;     // Para agrupar las respuestas
    QLabel *feedbackLabel;

    Problem problem;
    QVector <Answer> answers;
    int currentProblemIndex = 0;

    void updateTextScale();
    void drawProjections(QPointF scenePos);
    void clearProjections();
    QString decimalToDMS(double value, bool isLat);
    QIcon createVerticalTextIcon(const QString &text, const QColor &textColor, const QFont &font);
};

#endif // MAP_H
