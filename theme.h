#pragma once
#include <QObject>

class ThemeManager : public QObject
{
	Q_OBJECT

public:
	static ThemeManager* instance()
	{
		return nullptr;
	}

private:
	explicit ThemeManager(QObject* parent) : QObject(parent)
	{

	}

};

#define THEME_MANAGER ThemeManager::instance()