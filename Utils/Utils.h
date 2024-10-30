#ifndef UTILS_H
#define UTILS_H

#include <QIcon>
#include <QString>
#include <QPalette>

namespace Utils {
    QString getTheme();
    QString getAccentColor(const QString &accentKey);
    QIcon getIcon(int icon, bool pinned, const QString &color);
    QString getRandomPlaceholder();
    QPalette setTitleColor(QPalette originalPalette, const QString& color);
}

#endif // UTILS_H
