#ifndef PROBLEM_LIST_H
#define PROBLEM_LIST_H

#include <QMainWindow>
#include <QWidget>
#include "navigation.h"
#include "edit_profile.h"
// customheaderbar.h eliminado

QT_BEGIN_NAMESPACE
namespace Ui {
class problem_list;
}
QT_END_NAMESPACE

class problem_list : public QMainWindow
{
    Q_OBJECT

public:
    problem_list(QWidget *parent = nullptr);
    ~problem_list();

signals:
    void problemSelected(int index);

private:
    Ui::problem_list *ui;

    edit_profile *edit_prf_wnd;
    // Header eliminado

    QWidget *createProblemWidget(Problem problem, int index);
    int n_problema_display = 1;

private slots:
    void handleEditRequest(QString user_sent);
};
#endif // PROBLEM_LIST_H
