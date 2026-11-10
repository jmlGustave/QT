#ifndef EDIT_PROFILE_H
#define EDIT_PROFILE_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel> // Necesario
#include <QLineEdit>
#include <QDateEdit>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class edit_profile; }
QT_END_NAMESPACE

class edit_profile : public QMainWindow
{
    Q_OBJECT

public:
    edit_profile(QWidget *parent = nullptr);
    ~edit_profile();

    void setUser(QString &usr);

private:
    Ui::edit_profile *ui;

    QString user_nick;
    QString prf_pic_path;

    // UI Elements
    QWidget *main_container;
    QPushButton *prf_button;
    QLabel *pencil_icon_lbl; // <-- NUEVO ELEMENTO
    QPushButton *save_changes_button;
    QLabel *username_lbl;
    QLineEdit *email_edit;
    QLineEdit *password_edit;
    QDateEdit *birth_edit;

    void setupUiProgrammatically();
    void updateProfileIcon(const QImage &img);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void save_changes();
    void change_prf();
};
#endif // EDIT_PROFILE_H
