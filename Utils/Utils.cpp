#include "Utils.h"
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

    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegGetValueW(hKey, NULL, L"AccentPalette", RRF_RT_REG_BINARY, NULL, accentPalette, &bufferSize) == ERROR_SUCCESS) {
            RegCloseKey(hKey);

            int index = 0;
            if (accentKey == "dark2") index = 20;
            else if (accentKey == "light3") index = 0;
            else if (accentKey == "light2") index = 4;
            else if (accentKey == "light1") index = 8;
            else if (accentKey == "normal") index = 12;
            else {
                qDebug() << "Invalid accentKey provided.";
                return "#FFFFFF";
            }

            QString red = toHex(accentPalette[index]);
            QString green = toHex(accentPalette[index + 1]);
            QString blue = toHex(accentPalette[index + 2]);

            return QString("#%1%2%3").arg(red, green, blue);
        } else {
            qDebug() << "Failed to retrieve AccentPalette from the registry.";
        }

        RegCloseKey(hKey);
    } else {
        qDebug() << "Failed to open registry key.";
    }

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

QIcon Utils::getIcon(int icon, bool pinned, const QString& color)
{
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
    case 10:
        iconPixmap = QPixmap(":/icons/strikethrough.png");
        break;
    case 11:
        iconPixmap = QPixmap(":/icons/bulletlist.png");
        break;
    default:
        return QIcon();
    }

    if (pinned && (icon == 2 || icon == 4 || icon == 5 || icon == 8 || icon == 9 || icon == 10 || icon == 11)) {
        static const QMap<QString, QColor> colorMap = {
            { "Red", QColor(230, 45, 66) },
            { "Green", QColor(58, 148, 74) },
            { "Blue", QColor(52, 133, 228) },
            { "Orange", QColor(236, 91, 1) },
            { "Teal", QColor(32, 145, 164) },
            { "Pink", QColor(213, 96, 152) },
            { "Purple", QColor(144, 64, 172) },
            { "Yellow", QColor(200, 136, 0) }
        };

        if (color == "Windows") {
            recolor = (theme == "dark") ? getAccentColor("dark2") : getAccentColor("light2");
        } else if (colorMap.contains(color)) {
            recolor = colorMap.value(color);
        }
    }

    QPixmap recoloredIcon = recolorIcon(iconPixmap, recolor, 0, 0, 0);

    return QIcon(recoloredIcon);
}

QString Utils::getRandomPlaceholder()
{
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

QPalette Utils::setTitleColor(QPalette originalPalette, const QString& color)
{
    static const QMap<QString, QColor> colorMap = {
        { "Red", QColor(230, 45, 66) },
        { "Green", QColor(58, 148, 74) },
        { "Blue", QColor(52, 133, 228) },
        { "Orange", QColor(236, 91, 1) },
        { "Teal", QColor(32, 145, 164) },
        { "Pink", QColor(213, 96, 152) },
        { "Purple", QColor(144, 64, 172) },
        { "Yellow", QColor(200, 136, 0) }
    };

    if (color == "Windows") {
        QColor accentColor = getTheme() == "dark" ? getAccentColor("dark2") : getAccentColor("light2");
        originalPalette.setColor(QPalette::Text, accentColor);
        originalPalette.setColor(QPalette::Highlight, accentColor);
    } else {
        QColor chosenColor = colorMap.value(color, QColor(236, 91, 1)); // Default to Orange if color not found
        originalPalette.setColor(QPalette::Text, chosenColor);
        originalPalette.setColor(QPalette::Highlight, chosenColor);
    }

    return originalPalette;
}
