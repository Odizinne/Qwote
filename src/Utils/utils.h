#ifndef UTILS_H
#define UTILS_H

#include <QIcon>
#include <QString>

namespace Utils {
    QString getTheme();
    QString getAccentColor(const QString &accentKey);
    QIcon getIcon(int icon, bool pinned);
}

#endif // UTILS_H
