#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "Game.h"
#include "SynonymIndex.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QComboBox;
class QTableWidget;
class QStatusBar;
class QCompleter;
class QSlider;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onSearchClicked();
    void onAddGameClicked();
    void onSelectGifClicked();
    void onStopGifClicked();
    void onSelectMusicClicked();
    void onStopMusicClicked();
    void onVolumeChanged(int value);

private:
    void setupUI();
    void loadData();
    void saveGamesToJson();
    void displayResults(const QVector<Game>& results);
    void showStatus(const QString& message);

    QLineEdit* searchEdit;
    QPushButton* searchButton;
    QComboBox* platformCombo;
    QTableWidget* resultTable;
    QStatusBar* m_statusBar;
    QPushButton* addGameButton;
    QPushButton* selectGifBtn;
    QPushButton* stopGifBtn;
    QPushButton* selectMusicBtn;
    QPushButton* stopMusicBtn;
    QSlider* volumeSlider;

    QVector<Game> allGames;
    SynonymIndex synonymIndex;
};

#endif // MAINWINDOW_H
