#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <QString>
#include <QWidget>

void trim(std::string& s);
std::string toLower(std::string s);

void setBackgroundGif(QWidget* parentWidget, const QString& filePath);
void stopBackgroundGif();

void playMusic(const QString& filePath, int volume = 50);
void stopMusic();
void setVolume(int volume);
bool isMusicPlaying();

#endif // UTILS_H