#pragma once
#include <QObject>
#include <QDateTime>

class ReviewCard
{
public:
	enum ReviewLevel
	{
		OneDay = 0,
		ThreeDay = 1,
		OneWeek = 2,
		TwoWeek = 3,
		OneMonth = 4,
		ThreeMonth = 5,
		SixMonth = 6,
		OneYear = 7,
		TwoYear = 8
	};

	enum ShowListType
	{
		All = 0,
		Review = 1,
		Fav = 2,
		Test = 3,
	};

public:
	int id;
	QString question;
	QString answer;
	QString category;
	QString tags;
	QDateTime createTime;
	QDateTime reviewTime;
	bool fav;
	ReviewLevel level;
};
