#include "utils.h"
#include <QLabel>
#include <QMovie>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QEvent>
#include <algorithm>
#include <cctype>

void trim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return s;
}

static QLabel* s_gifLabel = nullptr;
static QMovie* s_movie = nullptr;

class GifResizeFilter : public QObject {
    QWidget* parent; QLabel* label;
public:
    GifResizeFilter(QWidget* p, QLabel* l) : parent(p), label(l) {}
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (obj == parent && event->type() == QEvent::Resize)
            label->setGeometry(parent->rect());
        return false;
    }
};

void setBackgroundGif(QWidget* parentWidget, const QString& filePath) {
    stopBackgroundGif();
    if (!parentWidget) return;
    s_movie = new QMovie(filePath);
    if (!s_movie->isValid()) { delete s_movie; s_movie = nullptr; return; }
    s_gifLabel = new QLabel(parentWidget);
    s_gifLabel->setMovie(s_movie);
    s_gifLabel->setScaledContents(true);
    s_gifLabel->setGeometry(parentWidget->rect());
    s_gifLabel->lower();
    s_gifLabel->show();
    s_movie->start();
    parentWidget->installEventFilter(new GifResizeFilter(parentWidget, s_gifLabel));
}

void stopBackgroundGif() {
    if (s_movie) { s_movie->stop(); delete s_movie; s_movie = nullptr; }
    if (s_gifLabel) { delete s_gifLabel; s_gifLabel = nullptr; }
}

static QMediaPlayer* s_player = nullptr;
static QMediaPlaylist* s_playlist = nullptr;

void playMusic(const QString& filePath, int volume) {
    stopMusic();
    s_player = new QMediaPlayer();
    s_playlist = new QMediaPlaylist();
    s_playlist->addMedia(QUrl::fromLocalFile(filePath));
    s_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    s_player->setPlaylist(s_playlist);
    s_player->setVolume(volume);
    s_player->play();
}

void stopMusic() {
    if (s_player) { s_player->stop(); delete s_player; s_player = nullptr; }
    if (s_playlist) { delete s_playlist; s_playlist = nullptr; }
}

void setVolume(int volume) { if (s_player) s_player->setVolume(volume); }
bool isMusicPlaying() { return s_player && s_player->state() == QMediaPlayer::PlayingState; }