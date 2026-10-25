#ifndef ADDGAMEDIALOG_H
#define ADDGAMEDIALOG_H

#include <QDialog>
#include <QString>

class QLineEdit;
class QComboBox;

class AddGameDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddGameDialog(QWidget* parent = nullptr);

    QString getTitle() const;
    QString getCode() const;
    QString getLocalization() const;

private:
    QLineEdit* titleEdit;
    QLineEdit* codeEdit;
    QComboBox* locCombo;
};

#endif 
