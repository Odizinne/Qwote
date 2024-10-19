#ifndef UTILS_H
#define UTILS_H

#include <QIcon>
#include <QString>

namespace Utils {
    bool isWindows10();
    QIcon getIcon(int icon, bool pinned);
}

#endif // UTILS_H
