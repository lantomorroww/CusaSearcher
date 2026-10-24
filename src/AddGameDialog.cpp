#include "AddGameDialog.h"
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

AddGameDialog::AddGameDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("Add Game");
    setModal(true);

    titleEdit = new QLineEdit;
    codeEdit = new QLineEdit;
    locCombo = new QComboBox;
    locCombo->addItem("No Russian", "none");
    locCombo->addItem("Subtitles only", "subtitles");
    locCombo->addItem("Full voiceover", "full");

    auto* formLayout = new QFormLayout;
    formLayout->addRow("Game title:", titleEdit);
    formLayout->addRow("Code (PPSA/CUSA):", codeEdit);
    formLayout->addRow("Localization:", locCombo);

    auto* okButton = new QPushButton("OK");
    auto* cancelButton = new QPushButton("Cancel");
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    auto* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
}

QString AddGameDialog::getTitle() const { return titleEdit->text().trimmed(); }
QString AddGameDialog::getCode() const { return codeEdit->text().trimmed(); }
QString AddGameDialog::getLocalization() const { return locCombo->currentData().toString(); }
