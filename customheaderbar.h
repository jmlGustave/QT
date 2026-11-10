#ifndef CUSTOMHEADERBAR_H
#define CUSTOMHEADERBAR_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpacerItem>
#include <QMenu>
#include <QAction>
#include <QVector>
#include "navigation.h"

class CustomHeaderBar : public QWidget
{
    Q_OBJECT

public:
    explicit CustomHeaderBar(QWidget *parent = nullptr, QString title = "CARTA NÁUTICA");
    ~CustomHeaderBar(); // Implementación importante en el .cpp

    void hide_logged_actions();
    void update_bar(QString usr);
    static QVector<CustomHeaderBar*> instances;
    static void syncAllBars(User *u);

    QString usr;
    bool logged = false;

    // Métricas (opcional, si las usas)
    int hits = 0;
    int faults = 0;

    void setTitle(QString newTitle);

signals:
    void profileOptionSelected(int optionId);
    void loginOptionSelected(int optionId);
    void registrationRequested();
    void editRequested(QString user_send);
    void historyRequested(User *u);

private slots:
    void showProfileMenu();
    void handleMenuAction(QAction* action);
    void showLoginMenu();
    void handleLoginAction();
    void log_out();
    void showHelp();

private:
    // UI Components
    QPushButton* m_profileButton;
    QLabel* text;
    QPushButton* user;
    QSpacerItem* spacer;

    QMenu* profileMenu;
    QMenu* loginMenu;
    QLineEdit *username;
    QLineEdit *password;

    QAction *m1_option1;
    QAction *m1_option2;
    QAction *m1_option3;
    QAction *m1_option4;
    QPushButton *helpButton;

    User *user_nav = nullptr; // Inicializado a nullptr por seguridad



    void setupUi();

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // CUSTOMHEADERBAR_H
