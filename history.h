#ifndef HISTORY_H
#define HISTORY_H

#include <QMainWindow>
#include <QDate>
#include <QDateEdit>
#include <QListWidget>
#include <QLabel>
#include "navigation.h"
// customheaderbar.h eliminado

class history : public QMainWindow
{
    Q_OBJECT

public:
    history(QWidget *parent = nullptr);
    ~history();
    void set_hist_usr(User *u);

private:
    // HeaderBar eliminado
    QListWidget *listHist;
    QDateEdit *date_filter;

    // Etiquetas para el Resumen
    QLabel *lblTotalSessions;
    QLabel *lblTotalHits;
    QLabel *lblTotalFaults;
    QLabel *lblAvgAccuracy;

    User *usr;

    void setupUi();
    void addStatistics();
    void updateSummary();
    QWidget* createRow(Session session);
    QWidget* createStatCard(QString title, QString value, QString colorInfo);

private slots:
    void filter(const QDate &date);
};
#endif // HISTORY_H
