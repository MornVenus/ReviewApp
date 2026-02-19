#pragma once
#include <QAbstractListModel>
#include <QList>
#include "reviewcard.h"

class ReviewCardListModel : public QAbstractListModel
{
	Q_OBJECT
public:
	ReviewCardListModel(QObject* parent) : QAbstractListModel(parent) {}

	~ReviewCardListModel() 
	{
		qDeleteAll(m_cards);
	}

	int rowCount(const QModelIndex& parent = QModelIndex()) const override { return m_cards.size(); }

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
	{
		if (!index.isValid()) return {};
		if (index.row() >= m_cards.size()) return {};

		if (role == Qt::UserRole + 1)
		{
			const ReviewCard* card = m_cards.at(index.row());
			return QVariant::fromValue(card);
		}
		else if (role == Qt::DisplayRole)
		{
			return m_cards.at(index.row())->question;
		}
		return {};
	}

	void setShowListType(ReviewCard::ShowListType type)
	{
		m_showListType = type;

		// fake list data for testing
		ReviewCard* card = new ReviewCard{ 1, "Question 1", "Answer 1", "Category 1", "Tag1, Tag2", QDateTime::currentDateTime(), QDateTime::currentDateTime(), false, ReviewCard::OneDay };
		m_cards.append(card);

		ReviewCard* card2 = new ReviewCard{ 1, "Question 2", "Answer 2", "Category 2", "Tag1, Tag2", QDateTime::currentDateTime(), QDateTime::currentDateTime(), false, ReviewCard::OneDay };
		m_cards.append(card2);
	}

private:
	QList<ReviewCard*> m_cards;
	ReviewCard::ShowListType m_showListType;
};
