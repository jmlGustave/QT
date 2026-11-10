#ifndef SIGN_UP_H
#define SIGN_UP_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFrame>
#include <QFileDialog>
#include <QRegularExpression> // Necesario para validar formatos

QT_BEGIN_NAMESPACE
namespace Ui { class Sign_Up; }
QT_END_NAMESPACE

class Sign_Up : public QDialog
{
    Q_OBJECT

public:
    Sign_Up(QWidget *parent = nullptr);
    ~Sign_Up();

signals:
    void registrationCompleted(const QString &usr);

private slots:
    void select_profile_picture();
    void sign_up_function();

private:
    void setupUiProgrammatically();

    // Función auxiliar para crear etiquetas de error
    QLabel* createErrorLabel(QWidget *parent);
    // Función para validar edad
    bool isOldEnough(QDate birthDate);

    // UI Elements
    QFrame *main_container;
    QLabel *title_label;
    QLabel *profile_pic_label;

    QLineEdit *username_line;
    QLabel *err_username; // Etiqueta de error para usuario

    QLineEdit *email_line;
    QLabel *err_email;    // Etiqueta de error para email

    QLineEdit *password_line;
    QLabel *err_password; // Etiqueta de error para pass

    QLineEdit *password_repeat_line;
    QLabel *err_repeat;   // Etiqueta de error para repetir pass

    QDateEdit *date_edit;
    QLabel *err_date;     // Etiqueta de error para fecha

    QPushButton *btn_register;

    QString icon_path;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};
#endif // SIGN_UP_H
