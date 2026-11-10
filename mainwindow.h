#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPropertyAnimation>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <QSpinBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QtSvgWidgets/QGraphicsSvgItem>
#include <QtMath>
#include <QMessageBox>
#include <QInputDialog>
#include <QColorDialog>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>

// Tus includes existentes
#include "sign_up.h"
#include "customheaderbar.h"
#include "edit_profile.h"
#include "problem_list.h"
#include "history.h"
// #include "map.h" // YA NO ES NECESARIO

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// =========================================================
// CLASES AUXILIARES (Compás, Regla, etc.) - COPIADAS DE MAP.H
// =========================================================

// Clase Pata del Compás
class CompassLeg : public QGraphicsObject {
public:
    CompassLeg(QGraphicsItem *parent = nullptr) : QGraphicsObject(parent) {
        setFlag(ItemIsSelectable, false); setFlag(ItemIsMovable, false);
        setAcceptHoverEvents(true); startMouseAngle = 0; startItemRotation = 0;
    }
    QRectF boundingRect() const override { return QRectF(-20, 0, 40, 320); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override {
        painter->setRenderHint(QPainter::Antialiasing);
        QPolygonF legBody; legBody << QPointF(-12, 0) << QPointF(12, 0) << QPointF(8, 270) << QPointF(-8, 270);
        QLinearGradient grad(0, 0, 20, 0); grad.setColorAt(0, QColor(220, 220, 220)); grad.setColorAt(1, QColor(150, 150, 150));
        painter->setBrush(grad); painter->setPen(QPen(Qt::black, 1)); painter->drawPolygon(legBody);
        QPolygonF legTip; legTip << QPointF(-8, 270) << QPointF(8, 270) << QPointF(0, 300);
        painter->setBrush(QColor(80, 80, 80)); painter->drawPolygon(legTip);
    }
protected:
    double getAngle(const QPointF &pos) { return qRadiansToDegrees(qAtan2(pos.y(), pos.x())) - 90; }
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            setCursor(Qt::ClosedHandCursor); startMouseAngle = getAngle(event->pos());
            startItemRotation = rotation(); event->accept();
        }
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->buttons() & Qt::LeftButton) {
            double currentMouseAngle = getAngle(event->pos());
            setRotation(startItemRotation + (currentMouseAngle - startMouseAngle));
        }
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override { setCursor(Qt::OpenHandCursor); QGraphicsObject::mouseReleaseEvent(event); }
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override { setCursor(Qt::OpenHandCursor); QGraphicsObject::hoverEnterEvent(event); }
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override { setCursor(Qt::ArrowCursor); QGraphicsObject::hoverLeaveEvent(event); }
private:
    double startMouseAngle; double startItemRotation;
};

// Clase Pivote Central
class CompassPivot : public QGraphicsObject {
public:
    CompassPivot() {
        setFlags(ItemIsMovable | ItemIsSelectable); setZValue(50);
        leftLeg = new CompassLeg(this); rightLeg = new CompassLeg(this);
        leftLeg->setRotation(25); rightLeg->setRotation(-25);
    }
    ~CompassPivot() { delete leftLeg; delete rightLeg; }
    QRectF boundingRect() const override { return QRectF(-25, -25, 50, 50); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override {
        painter->setRenderHint(QPainter::Antialiasing);
        QRadialGradient grad(0, 0, 25); grad.setColorAt(0, QColor(240, 240, 240)); grad.setColorAt(1, QColor(100, 100, 100));
        painter->setBrush(grad); painter->setPen(QPen(Qt::black, 2)); painter->drawEllipse(boundingRect());
        painter->setBrush(Qt::darkGray); painter->drawEllipse(QRectF(-5, -5, 10, 10));
    }
    CompassLeg *leftLeg; CompassLeg *rightLeg;
};

// Clase Item Rotable SVG
class RotatableSvgItem : public QGraphicsSvgItem {
public:
    RotatableSvgItem(const QString &fileName) : QGraphicsSvgItem(fileName) { setFlags(ItemIsMovable | ItemIsSelectable); isRotating = false; }
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->button() == Qt::LeftButton && (event->modifiers() & Qt::ControlModifier)) {
            isRotating = true; setCursor(Qt::ClosedHandCursor);
            QPointF center = boundingRect().center(); QPointF pos = event->pos();
            startMouseAngle = qRadiansToDegrees(qAtan2(pos.y() - center.y(), pos.x() - center.x()));
            startItemRotation = rotation(); event->accept();
        } else { setCursor(Qt::ArrowCursor); QGraphicsSvgItem::mousePressEvent(event); }
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override {
        if (isRotating) {
            QPointF center = boundingRect().center(); QPointF pos = event->pos();
            double currentMouseAngle = qRadiansToDegrees(qAtan2(pos.y() - center.y(), pos.x() - center.x()));
            setRotation(startItemRotation + (currentMouseAngle - startMouseAngle)); event->accept();
        } else { QGraphicsSvgItem::mouseMoveEvent(event); }
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override {
        if (isRotating) { isRotating = false; setCursor(Qt::ArrowCursor); }
        else { QGraphicsSvgItem::mouseReleaseEvent(event); }
    }
private:
    bool isRotating; double startMouseAngle; double startItemRotation;
};

// =========================================================
// CLASE PRINCIPAL (MainWindow fusionada con Map)
// =========================================================

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Método para seleccionar problema (traído de map)
    void selectProblem(int n_problem);

protected:
    // Eventos gráficos traídos de map
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Slots de MainWindow original
    void handleProfileOption(int optionId);
    void handleRegistrationRequest();
    void handleRegistrationCompleted(const QString &usr);
    void handleEditRequest(QString user_sent);
    void handleHistoryRequest(User *u);
    void handleProblemListRequest(); // Ahora mostrará la ventana de lista

    // Slots traídos de map
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
    void selectRandomProblem();
    void onProblemSelectedFromList(int index);

private:
    Ui::MainWindow *ui;

    // -- Variables de navegación de MainWindow --
    Sign_Up s;
    CustomHeaderBar *headerBar;
    edit_profile *edit_prf_wnd;
    problem_list *problem_list_wnd;
    history *history_wnd;

    // -- Variables Gráficas de MAP --
    QGraphicsView *view;
    QGraphicsScene *scene;
    double m_scaleFactor;

    QPushButton *menuButton;
    QWidget *menuContainer;
    QPropertyAnimation *menuAnimation;
    bool isMenuOpen;

    bool isAddPointMode; bool isDrawLineMode; bool isCompassMode;
    bool isTextMode; bool isEraserMode;

    QGraphicsLineItem *currentLine;
    QGraphicsEllipseItem *currentCircle;
    QPointF compassCenter;

    int currentSize = 10;
    QSpinBox *sizeSpinBox;
    QColor currentColor = Qt::red;

    RotatableSvgItem *protractorItem;
    RotatableSvgItem *rulerItem;
    CompassPivot *compassItem;

    bool isFirstShow = true;

    bool isCoordinateMode = false;
    QGraphicsLineItem *projLineH = nullptr;
    QGraphicsLineItem *projLineV = nullptr;
    QGraphicsTextItem *cursorLabel = nullptr;

    // Constantes de calibración
    const double PIXEL_LEFT = 328.0; const double PIXEL_RIGHT = 8367.0;
    const double PIXEL_TOP = 444.0; const double PIXEL_BOTTOM = 5345.0;
    const double GEO_LAT_TOP = 36.20; const double GEO_LAT_BOTTOM = 35.35;
    const double GEO_LON_LEFT = -6.20; const double GEO_LON_RIGHT = -4.55;

    // -- Variables del Panel de Problemas (Quiz) --
    QPushButton *quizButton;
    QWidget *quizContainer;
    QPropertyAnimation *quizAnimation;
    bool isQuizOpen = false;

    QLabel *questionLabel;
    QRadioButton *rbA; QRadioButton *rbB; QRadioButton *rbC; QRadioButton *rbD;
    QButtonGroup *answerGroup;
    QLabel *feedbackLabel;

    Problem problem;
    int currentProblemIndex = 0;

    // Helper functions
    void updateTextScale();
    void drawProjections(QPointF scenePos);
    void clearProjections();
    QString decimalToDMS(double value, bool isLat);
    QIcon createVerticalTextIcon(const QString &text, const QColor &textColor, const QFont &font);
};

#endif // MAINWINDOW_H
