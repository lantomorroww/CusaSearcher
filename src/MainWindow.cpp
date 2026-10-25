#include "MainWindow.h"
#include "FileHandler.h"
#include "utils.h"
#include "AddGameDialog.h"
#include <nlohmann/json.hpp>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTableWidget>
#include <QStatusBar>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QCompleter>
#include <QStringListModel>
#include <QCoreApplication>
#include <QFileDialog>
#include <fstream>
#include <algorithm>
#include <cctype>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUI();
    loadData();
}

MainWindow::~MainWindow()
{
    stopBackgroundGif();
    stopMusic();
}

void MainWindow::setupUI()
{
    auto* central = new QWidget(this);
    setCentralWidget(central);

    searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("Введите название, код или синоним...");
    searchButton = new QPushButton("Найти");
    addGameButton = new QPushButton("Добавить игру");
    platformCombo = new QComboBox;
    platformCombo->addItem("Все платформы");

    resultTable = new QTableWidget;
    resultTable->setColumnCount(3);
    resultTable->setHorizontalHeaderLabels({"Название игры", "Платформа", "Регионы / Коды"});
    resultTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    resultTable->setAlternatingRowColors(true);

    m_statusBar = statusBar();

    selectGifBtn = new QPushButton("Выбрать GIF");
    stopGifBtn = new QPushButton("Убрать GIF");
    selectMusicBtn = new QPushButton("Выбрать музыку");
    stopMusicBtn = new QPushButton("Стоп музыку");
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeSlider->setFixedWidth(100);

    auto* searchLayout = new QHBoxLayout;
    searchLayout->addWidget(platformCombo);
    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(addGameButton);

    auto* mediaLayout = new QHBoxLayout;
    mediaLayout->addWidget(selectGifBtn);
    mediaLayout->addWidget(stopGifBtn);
    mediaLayout->addWidget(selectMusicBtn);
    mediaLayout->addWidget(stopMusicBtn);
    mediaLayout->addWidget(new QLabel("Громкость:"));
    mediaLayout->addWidget(volumeSlider);
    mediaLayout->addStretch();

    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->addLayout(searchLayout);
    mainLayout->addLayout(mediaLayout);
    mainLayout->addWidget(resultTable);

    connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
    connect(searchEdit, &QLineEdit::returnPressed, this, &MainWindow::onSearchClicked);
    connect(addGameButton, &QPushButton::clicked, this, &MainWindow::onAddGameClicked);
    connect(selectGifBtn, &QPushButton::clicked, this, &MainWindow::onSelectGifClicked);
    connect(stopGifBtn, &QPushButton::clicked, this, &MainWindow::onStopGifClicked);
    connect(selectMusicBtn, &QPushButton::clicked, this, &MainWindow::onSelectMusicClicked);
    connect(stopMusicBtn, &QPushButton::clicked, this, &MainWindow::onStopMusicClicked);
    connect(volumeSlider, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);

    QString flatButtonStyle =
        "QPushButton {"
        "   background-color: #000000;"
        "   color: #ffffff;"
        "   border: 1px solid #555555;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #333333; }"
        "QPushButton:pressed { background-color: #111111; }";

    searchButton->setStyleSheet(flatButtonStyle);
    addGameButton->setStyleSheet(flatButtonStyle);
    selectGifBtn->setStyleSheet(flatButtonStyle);
    stopGifBtn->setStyleSheet(flatButtonStyle);
    selectMusicBtn->setStyleSheet(flatButtonStyle);
    stopMusicBtn->setStyleSheet(flatButtonStyle);

    resultTable->setStyleSheet(
        "QTableWidget {"
        "   color: #ffffff;"
        "   alternate-background-color: #1a1a1a;"
        "   gridline-color: #444444;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "}"
        "QTableWidget::item { padding: 8px; }"
        "QTableWidget::item:selected { background-color: #4a6e8c; color: white; }"
        "QHeaderView::section {"
        "   background-color: #000000;"
        "   color: white;"
        "   font-weight: bold;"
        "   padding: 6px;"
        "   border: 1px solid #444444;"
        "}"
    );

    QString lineEditStyle =
        "QLineEdit, QComboBox {"
        "   background-color: #000000;"
        "   color: #ffffff;"
        "   border: 1px solid #555555;"
        "   border-radius: 4px;"
        "   padding: 6px;"
        "   font-weight: bold;"
        "}";
    searchEdit->setStyleSheet(lineEditStyle);
    platformCombo->setStyleSheet(lineEditStyle);

    central->setStyleSheet("background-color: #000000;");
    setStyleSheet("QMainWindow { background-color: #000000; }");
}

void MainWindow::loadData()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QString gamesPathQ = appPath + "/data/gameBase/games_data.json";
    QString synonymsPathQ = appPath + "/data/synonymsBase/synonyms.json";

    showStatus("Загрузка игр...");
    std::vector<Game> tempGames = loadGamesFromFile(gamesPathQ.toStdString());
    if (tempGames.empty()) {
        QMessageBox::critical(this, "Ошибка",
            "Не удалось загрузить игры из файла:\n" + gamesPathQ);
        return;
    }

    allGames.clear();
    allGames.reserve(tempGames.size());
    for (const auto& game : tempGames) {
        allGames.append(game);
    }

    showStatus("Загрузка синонимов...");
    if (!synonymIndex.loadFromFile(synonymsPathQ.toStdString())) {
        QMessageBox::warning(this, "Предупреждение",
            "Не удалось загрузить синонимы из файла:\n" + synonymsPathQ);
    }

    QStringList allSynonyms;
    for (const auto& syn : synonymIndex.getAllSynonyms()) {
        allSynonyms << QString::fromStdString(syn);
    }
    auto* model = new QStringListModel(allSynonyms, this);
    auto* completer = new QCompleter(model, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    searchEdit->setCompleter(completer);

    showStatus(QString("Загружено %1 игр").arg(allGames.size()));
}

void MainWindow::onSearchClicked()
{
    QString query = searchEdit->text().trimmed();
    if (query.isEmpty()) {
        resultTable->setRowCount(0);
        showStatus("Введите запрос для поиска");
        return;
    }

   std::string queryStd = query.toStdString();
trim(queryStd);
queryStd = toLower(queryStd);

    QVector<Game> results;

    std::string code = synonymIndex.findCodeBySynonym(queryStd);
    if (!code.empty()) {
        for (const auto& game : allGames) {
            for (const auto& [_, c] : game.regionCodes) {
                if (toLower(c) == toLower(code)) {
                    results.append(game);
                    break;
                }
            }
            if (!results.isEmpty()) break;
        }
    } else {
        for (const auto& game : allGames) {
            std::string titleLower = toLower(game.title);
            if (titleLower.find(queryStd) != std::string::npos) {
                results.append(game);
            }
        }
        if (results.isEmpty()) {
            bool onlyDigits = !queryStd.empty() &&
                std::all_of(queryStd.begin(), queryStd.end(),
                    [](unsigned char c) { return std::isdigit(c); });
            for (const auto& game : allGames) {
                for (const auto& [_, c] : game.regionCodes) {
                    std::string codeLower = toLower(c);
                    if (codeLower == queryStd) {
                        results.append(game);
                        break;
                    }
                    if (onlyDigits) {
                        std::string numericPart;
                        for (char ch : codeLower) {
                            if (std::isdigit(static_cast<unsigned char>(ch)))
                                numericPart += ch;
                        }
                        if (numericPart == queryStd) {
                            results.append(game);
                            break;
                        }
                    }
                }
            }
        }
    }

    displayResults(results);
    showStatus(results.isEmpty()
        ? "Ничего не найдено"
        : QString("Найдено %1 игр").arg(results.size()));
}

void MainWindow::saveGamesToJson()
{
    QString appPath = QCoreApplication::applicationDirPath();
    QString filenameQ = appPath + "/data/gameBase/games_data.json";
    std::string filename = filenameQ.toStdString();

    nlohmann::json j = nlohmann::json::array();

    for (const auto& game : allGames) {
        nlohmann::json item;
        item["title"] = game.title;
        item["platform"] = game.platform;
        nlohmann::json codes;
        for (const auto& [region, code] : game.regionCodes) {
            std::string val = code;
            auto it = game.regionLocalization.find(region);
            if (it != game.regionLocalization.end()) {
                if (it->second == "full") val += " (full)";
                else if (it->second == "subtitles") val += " (sub)";
            }
            codes[region] = val;
        }
        item["regionCodes"] = codes;
        j.push_back(item);
    }

    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4);
    } else {
        showStatus("Не удалось сохранить в " + filenameQ);
    }
}

void MainWindow::onAddGameClicked()
{
    AddGameDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) return;

    QString title = dialog.getTitle();
    QString code = dialog.getCode();
    QString loc = dialog.getLocalization();

    if (title.isEmpty() || code.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Название и код не могут быть пустыми.");
        return;
    }

    Game newGame;
    newGame.title = title.toStdString();
    newGame.platform = "PS5";
    newGame.regionCodes["Россия"] = code.toStdString();
    if (loc == "full") newGame.regionLocalization["Россия"] = "full";
    else if (loc == "subtitles") newGame.regionLocalization["Россия"] = "subtitles";
    else newGame.regionLocalization["Россия"] = "none";

    newGame.regionCodes["Польша"] = code.toStdString();
    newGame.regionCodes["Турция"] = code.toStdString();
    newGame.regionCodes["Индия"] = code.toStdString();
    newGame.regionCodes["Великобритания"] = code.toStdString();

    allGames.append(newGame);
    saveGamesToJson();

    displayResults(allGames);
    QMessageBox::information(this, "Успех", "Игра добавлена!");
}

void MainWindow::displayResults(const QVector<Game>& games)
{
    resultTable->setRowCount(games.size());
    for (int i = 0; i < games.size(); ++i) {
        const auto& game = games[i];
        auto* titleItem = new QTableWidgetItem(QString::fromStdString(game.title));
        titleItem->setFont(QFont("Arial", 10, QFont::Bold));
        resultTable->setItem(i, 0, titleItem);
        resultTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(game.platform)));

        QString regionsStr;
        for (const auto& [region, code] : game.regionCodes) {
            QString line = QString::fromStdString(region) + ": " + QString::fromStdString(code);
            auto itLoc = game.regionLocalization.find(region);
            if (itLoc != game.regionLocalization.end() && !itLoc->second.empty()) {
                line += " (" + QString::fromStdString(itLoc->second) + ")";
            }
            regionsStr += line + "; ";
        }
        auto* regionItem = new QTableWidgetItem(regionsStr);
        regionItem->setFont(QFont("Arial", 10, QFont::Bold));
        resultTable->setItem(i, 2, regionItem);
    }
    resultTable->resizeColumnsToContents();
}

void MainWindow::onSelectGifClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите GIF", "", "GIF Files (*.gif)");
    if (!filePath.isEmpty()) {
        setBackgroundGif(centralWidget(), filePath);
    }
}

void MainWindow::onStopGifClicked() { stopBackgroundGif(); }

void MainWindow::onSelectMusicClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите аудиофайл", "",
        "Audio Files (*.mp3 *.wav *.ogg)");
    if (!filePath.isEmpty()) {
        playMusic(filePath, volumeSlider->value());
    }
}

void MainWindow::onStopMusicClicked() { stopMusic(); }

void MainWindow::onVolumeChanged(int value) { setVolume(value); }

void MainWindow::showStatus(const QString& message)
{
    if (m_statusBar) m_statusBar->showMessage(message, 3000);
}
