#include "utils.h"
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QProcess>
#include <QRandomGenerator>
#include <QPalette>
#include <QApplication>

#ifdef _WIN32
#include <windows.h>
#endif


QString Utils::getTheme()
{
#ifdef _WIN32
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        QSettings::NativeFormat);
    int value = settings.value("AppsUseLightTheme", 1).toInt();

    return (value == 0) ? "light" : "dark";
#elif __linux__
    QPalette palette = QApplication::palette();
    QColor windowColor = palette.color(QPalette::Window);
    QColor textColor = palette.color(QPalette::WindowText);

    if (windowColor.lightness() < 128 && textColor.lightness() > 128) {
        return "light";
    } else {
        return "dark";
    }
#endif

}
#ifdef _WIN32
QString toHex(BYTE value) {
    const char* hexDigits = "0123456789ABCDEF";
    return QString("%1%2")
        .arg(hexDigits[value >> 4])
        .arg(hexDigits[value & 0xF]);
}
#endif

QString Utils::getAccentColor(const QString &accentKey)
{
#ifdef _WIN32
    HKEY hKey;
    BYTE accentPalette[32];  // AccentPalette contains 32 bytes
    DWORD bufferSize = sizeof(accentPalette);

    // Open the Windows registry key for AccentPalette
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        // Read the AccentPalette binary data
        if (RegGetValueW(hKey, NULL, L"AccentPalette", RRF_RT_REG_BINARY, NULL, accentPalette, &bufferSize) == ERROR_SUCCESS) {
            // Close the registry key after reading
            RegCloseKey(hKey);

            // Determine the correct index based on the accentKey
            int index = 0;
            if (accentKey == "dark2") index = 20;   // Index for "dark2"
            else if (accentKey == "light3") index = 0;  // Index for "light3"
            else if (accentKey == "normal") index = 12;
            else {
                qDebug() << "Invalid accentKey provided.";
                return "#FFFFFF";  // Return white if invalid accentKey
            }

            // Extract RGB values and convert them to hex format
            QString red = toHex(accentPalette[index]);
            QString green = toHex(accentPalette[index + 1]);
            QString blue = toHex(accentPalette[index + 2]);

            // Return the hex color code
            return QString("#%1%2%3").arg(red, green, blue);
        } else {
            qDebug() << "Failed to retrieve AccentPalette from the registry.";
        }

        RegCloseKey(hKey);  // Ensure the key is closed
    } else {
        qDebug() << "Failed to open registry key.";
    }

    // Fallback color if registry access fails
    return "#FFFFFF";
#elif __linux__
    return "#FFFFFF";
#endif
}

QPixmap recolorIcon(const QPixmap &originalIcon, const QColor &color, int redReplace, int greenReplace, int blueReplace)
{
    QImage img = originalIcon.toImage();

    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            QColor pixelColor = img.pixelColor(x, y);
            if (pixelColor == QColor(redReplace, greenReplace, blueReplace)) {
                img.setPixelColor(x, y, color);
            }
        }
    }

    return QPixmap::fromImage(img);
}

QIcon Utils::getIcon(int icon, bool pinned) {
    QString theme = getTheme();
    QColor recolor = (theme == "dark") ? QColor(0, 0, 0) : QColor(215, 215, 215);
    QPixmap iconPixmap;

    switch (icon) {
    case 1:
        iconPixmap = QPixmap(":/icons/new_icon.png");
        break;
    case 2:
        iconPixmap = QPixmap(":/icons/pin_icon.png");
        break;
    case 3:
        iconPixmap = QPixmap(":/icons/delete_icon.png");
        break;
    case 4:
        iconPixmap = QPixmap(":/icons/bold.png");
        break;
    case 5:
        iconPixmap = QPixmap(":/icons/italic.png");
        break;
    case 6:
        iconPixmap = QPixmap(":/icons/plus.png");
        break;
    case 7:
        iconPixmap = QPixmap(":/icons/minus.png");
        break;
    case 8:
        iconPixmap = QPixmap(":/icons/editortools.png");
        break;
    case 9:
        iconPixmap = QPixmap(":/icons/underline.png");
        break;
    default:
        return QIcon();
    }

    if (pinned && (icon == 2 || icon == 4 || icon == 5 || icon == 8 || icon == 9)) {
        recolor = (theme == "dark") ? getAccentColor("dark2") : getAccentColor("light3");
    }

    QPixmap recoloredIcon = recolorIcon(iconPixmap, recolor, 0, 0, 0);

    return QIcon(recoloredIcon);
}

QString Utils::getRandomPlaceholder() {
    QStringList placeholders = {
        QObject::tr("Shopping list"),
        QObject::tr("Give it a name"),
        QObject::tr("Name this note"),
        QObject::tr("My super note"),
        QObject::tr("Remember This"),
        QObject::tr("Quick Thoughts"),
        QObject::tr("Note to Self"),
        QObject::tr("Code Snippets"),
        QObject::tr("Work in Progress"),
        QObject::tr("Ideas"),
        QObject::tr("Workflows")
    };

    int randomIndex = QRandomGenerator::global()->bounded(placeholders.size());
    return placeholders[randomIndex];
}

QPalette Utils::setTitleColor(QPalette originalPalette) {
    if (getTheme() == "dark") {
        originalPalette.setColor(QPalette::Text, getAccentColor("dark2"));
    } else {
        originalPalette.setColor(QPalette::Text, getAccentColor("light3"));
    }
    return originalPalette;
}
