#pragma once
#include <QObject>
#include <QColor>
#include <QFont>
#include <QMap>
#include <QApplication>
#include <QSettings>
#include <QFile>

// 主题定义 - 增强版
struct Theme
{
    QString name;
    QString displayName;
    QString description;

    // 颜色
    QColor primary;           // 主色调
    QColor secondary;         // 次色调
    QColor accent;            // 强调色
    QColor background;        // 背景色
    QColor surface;           // 表面/卡片色
    QColor surfaceElevated;   // 提升的表面色
    QColor text;              // 文本色
    QColor textMuted;         // 次要文本色
    QColor border;            // 边框色
    QColor hover;             // 悬停色
    QColor selected;          // 选中色
    QColor success;           // 成功色
    QColor warning;           // 警告色
    QColor error;             // 错误色

    // 渐变颜色
    QColor gradientStart;     // 渐变开始
    QColor gradientEnd;       // 渐变结束

    // 字体 - 支持 display 和 body 字体
    QString fontDisplay;      // 显示字体（标题等）
    QString fontBody;         // 正文字体
    int fontSize;

    bool isDark;              // 是否为深色主题
};


class ThemeManager : public QObject
{
	Q_OBJECT

public:
	static ThemeManager* instance()
	{
        static ThemeManager* s_instance = new ThemeManager(nullptr);
        return s_instance;
	}

    Theme getCurrentTheme() const { return m_currentTheme; }

    QStringList getThemeNames() const
    {
        QStringList names;
        for (auto it = m_themes.begin(); it != m_themes.end(); ++it)
        {
            names.append(it.key());
        }
        return names;
    }

    Theme getTheme(const QString& name) const
    {
        if (m_themes.contains(name))
        {
            return m_themes[name];
        }
        return Theme();
    }

    void applyTheme(const QString& name)
    {
        if (!m_themes.contains(name))
        {
            qWarning() << "Theme not found:" << name;
            return;
        }

        m_currentTheme = m_themes[name];
        m_currentThemeName = name;

        QString styleSheet = generateStyleSheet(m_currentTheme);
        qApp->setStyleSheet(styleSheet);

        QSettings settings("ReviewApp", "Theme");
        settings.setValue("currentTheme", name);

        emit themeChanged(m_currentTheme);

        qDebug() << "Theme applied:" << name;
    }

private:
	explicit ThemeManager(QObject* parent) : QObject(parent)
	{
        initializeThemes();
	}

    // 初始化所有主题
    void initializeThemes()
    {
        setupTheme_ArcticFrost();
        setupTheme_OceanDepths();

        QSettings settings("ReviewApp", "Theme");
        QString savedTheme = settings.value("currentTheme").toString();
        if (savedTheme.isEmpty() || !m_themes.contains(savedTheme))
        {
            savedTheme = "ArcticFrost";
        }
        applyTheme(savedTheme);
    }

    void setupTheme_ArcticFrost()
    {
        Theme theme;
        theme.name = "ArcticFrost";
        theme.displayName = "Arctic Frost";
        theme.description = "Fresh and minimal cool tone theme, pure and elegant";

        theme.primary = QColor("#4a6fa5");
        theme.secondary = QColor("#6b8cae");
        theme.accent = QColor("#87ceeb");
        theme.background = QColor("#f8fafc");
        theme.surface = QColor("#ffffff");
        theme.surfaceElevated = QColor("#f1f5f9");
        theme.text = QColor("#1e293b");
        theme.textMuted = QColor("#64748b");
        theme.border = QColor("#e2e8f0");
        theme.hover = QColor("#e2e8f0");
        theme.selected = QColor("#4a6fa5");
        theme.success = QColor("#10b981");
        theme.warning = QColor("#f59e0b");
        theme.error = QColor("#ef4444");

        theme.gradientStart = QColor("#4a6fa5");
        theme.gradientEnd = QColor("#87ceeb");

        theme.fontDisplay = "Segoe UI";
        theme.fontBody = "Segoe UI";
        theme.fontSize = 10;
        theme.isDark = false;

        m_themes[theme.name] = theme;
    }

    void setupTheme_OceanDepths()
    {
        Theme theme;
        theme.name = "OceanDepths";
        theme.displayName = "Ocean Depths";
        theme.description = "Professional and calm navy blue theme with glass morphism effects";

        theme.primary = QColor("#1a5f7a");
        theme.secondary = QColor("#2d8b8b");
        theme.accent = QColor("#57c5b6");
        theme.background = QColor("#0a1929");
        theme.surface = QColor("#112240");
        theme.surfaceElevated = QColor("#1a3a5c");
        theme.text = QColor("#e6f1ff");
        theme.textMuted = QColor("#8892b0");
        theme.border = QColor("#1d4e6f");
        theme.hover = QColor("#1d3a52");
        theme.selected = QColor("#2d8b8b");
        theme.success = QColor("#64ffda");
        theme.warning = QColor("#ffb703");
        theme.error = QColor("#e63946");

        theme.gradientStart = QColor("#1a5f7a");
        theme.gradientEnd = QColor("#57c5b6");

        theme.fontDisplay = "Cascadia Code";
        theme.fontBody = "Segoe UI";
        theme.fontSize = 10;
        theme.isDark = true;

        m_themes[theme.name] = theme;
    }

    //% 1 : primary
    //% 2 : secondary
    //% 3 : accent
    //% 4 : background
    //% 5 : surface
    //% 6 : surfaceElevated
    //% 7 : text
    //% 8 : textMuted
    //% 9 : border
    //% 10 : hover
    //% 11 : selected
    //% 12 : fontFamily
    //% 13 : fontSize
    //% 14 : fontSize - 1
    //% 15 : fontSize + 1
    //% 16 : fontSize + 2
    //% 17 : scrollBg
    //% 18 : disbled-background 
    QString generateStyleSheet(const Theme& theme)
    {
        QFile file(":/themes/template.qss");   // 资源路径
        if (file.open(QFile::ReadOnly | QFile::Text))
        {
           QString qss = file.readAll();
           return qss.arg(theme.primary.name())
               .arg(theme.secondary.name())
               .arg(theme.accent.name())
               .arg(theme.background.name())
               .arg(theme.surface.name())
               .arg(theme.surfaceElevated.name())
               .arg(theme.text.name())
               .arg(theme.textMuted.name())
               .arg(theme.border.name())
               .arg(theme.hover.name())
               .arg(theme.selected.name())
               .arg(theme.fontDisplay)
               .arg(theme.fontSize)
               .arg(theme.fontSize - 1)
               .arg(theme.fontSize + 1)
               .arg(theme.fontSize + 2)
               .arg(theme.isDark ? "#1a1a2e" : "#f1f5f9")
               .arg(theme.isDark ? "#2a2a2a" : "#e0e0e0");
        }
        else
        {
            return "";
        }
    }

signals:
    void themeChanged(const Theme& theme);

private:
    QMap<QString, Theme> m_themes;
    Theme m_currentTheme;
    QString m_currentThemeName;
};

#define THEME_MANAGER ThemeManager::instance()