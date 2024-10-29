#ifndef UTILS_H
#define UTILS_H

#include <QIcon>
#include <QString>
#include <QPalette>

namespace Utils {
    QString getTheme();
    QString getAccentColor(const QString &accentKey);
    QIcon getIcon(int icon, bool pinned);
    QString getRandomPlaceholder();
    QPalette setTitleColor(QPalette originalPalette);
}

#endif // UTILS_H
