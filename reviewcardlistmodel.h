#pragma once
#include <QAbstractListModel>
#include <QList>
#include "reviewcard.h"
#include <QtSql>

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
			ReviewCard* card = m_cards.at(index.row());
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
		beginResetModel();
		qDeleteAll(m_cards);
		m_cards.clear();

		QSqlQuery query;
		query.prepare("SELECT question, answer, category, tags, level, create_time, review_time, fav, id FROM records;");

		if (query.exec())
		{
			int eligibleCount = 0;
			const int MAX_RECORDS = 20;

			while (query.next())
			{
				QString question = query.value(0).toString();
				QString answer = query.value(1).toString();
				QString category = query.value(2).toString();
				QString tags = query.value(3).toString();
				ReviewCard::ReviewLevel level = static_cast<ReviewCard::ReviewLevel>(query.value(4).toInt());
				QDateTime createTime = query.value(5).toDateTime();
				QDateTime reviewTime = query.value(6).toDateTime();
				int fav = query.value(7).toInt();
				int id = query.value(8).toInt();

				bool toAdd = false;

				if (m_showListType == ReviewCard::ShowListType::All)
				{
					toAdd = true;

				}
				else if (m_showListType == ReviewCard::ShowListType::Fav)
				{
					toAdd = fav;
				}
				else if (m_showListType == ReviewCard::ShowListType::Test)
				{
					// 蓄水池抽样：每条记录概率相等，最多选20条
					ReviewCard* record = new ReviewCard{ id, question, answer, category, tags, createTime, reviewTime, fav == 1, level };
					eligibleCount++;

					if (m_cards.size() < MAX_RECORDS)
					{
						// 前20条直接加入
						m_cards.append(record);
					}
					else
					{
						// 第n条（n>20）以 20/n 的概率替换蓄水池中的随机一条
						int randomIndex = QRandomGenerator::global()->bounded(eligibleCount);
						if (randomIndex < MAX_RECORDS)
						{
							delete m_cards[randomIndex];
							m_cards[randomIndex] = record;
						}
						else
						{
							delete record;
						}
					}
					continue; // Test模式下不执行后续的toAdd逻辑
				}
				else
				{
					// 检查是否需要复习：reviewTime + level对应的时间间隔 <= 当前时间
					QDateTime nextReviewTime = reviewTime;
					switch (level)
					{
					case ReviewCard::ReviewLevel::OneDay:
						nextReviewTime = reviewTime.addDays(1);
						break;
					case ReviewCard::ReviewLevel::ThreeDay:
						nextReviewTime = reviewTime.addDays(3);
						break;
					case ReviewCard::ReviewLevel::OneWeek:
						nextReviewTime = reviewTime.addDays(7);
						break;
					case ReviewCard::ReviewLevel::TwoWeek:
						nextReviewTime = reviewTime.addDays(14);
						break;
					case ReviewCard::ReviewLevel::OneMonth:
						nextReviewTime = reviewTime.addMonths(1);
						break;
					case ReviewCard::ReviewLevel::ThreeMonth:
						nextReviewTime = reviewTime.addMonths(3);
						break;
					case ReviewCard::ReviewLevel::SixMonth:
						nextReviewTime = reviewTime.addMonths(6);
						break;
					case ReviewCard::ReviewLevel::OneYear:
						nextReviewTime = reviewTime.addYears(1);
						break;
					case ReviewCard::ReviewLevel::TwoYear:
						nextReviewTime = reviewTime.addYears(2);
						break;
					default:
						nextReviewTime = reviewTime.addDays(1);
						break;
					}

					toAdd = nextReviewTime <= QDateTime::currentDateTime();
				}

				if (toAdd)
				{
					ReviewCard* card = new ReviewCard{ id, question, answer, category, tags, createTime, reviewTime, fav == 1, level};
					m_cards.append(card);
				}
			}
		}
		else
		{
			qCritical() << "Failed to execute query:" << query.lastError().text();
		}

		endResetModel();

		// fake list data for testing
		/*ReviewCard* card = new ReviewCard{ 1, "Question 1", "Answer 1", "Category 1", "Tag1, Tag2", QDateTime::currentDateTime(), QDateTime::currentDateTime(), false, ReviewCard::OneDay };
		m_cards.append(card);

		ReviewCard* card2 = new ReviewCard{ 1, "Question 2", "Answer 2", "Category 2", "Tag1, Tag2", QDateTime::currentDateTime(), QDateTime::currentDateTime(), false, ReviewCard::OneDay };
		m_cards.append(card2);*/



	}

	void append(PlainCard card)
	{
		QSqlQuery query;
		query.prepare(R"(INSERT INTO records (question, answer, category, tags) VALUES (:question, :answer, :category, :tags))");
		query.bindValue(":question", card.question);
		query.bindValue(":answer", card.answer);
		query.bindValue(":category", card.category);
		query.bindValue(":tags", card.tags);

		if (!query.exec())
		{
			qCritical() << "Failed to insert record:" << query.lastError().text();
		}
		else
		{
			qDebug() << "Record inserted successfully.";
			if (m_showListType == ReviewCard::ShowListType::All)
			{
				QVariant id = query.lastInsertId();
				ReviewCard* newCard = new ReviewCard{ id.toInt(), card.question, card.answer, card.category, card.tags, QDateTime::currentDateTime(), QDateTime::currentDateTime(), false, ReviewCard::OneDay};
				beginInsertRows(QModelIndex(), m_cards.size(), m_cards.size());
				m_cards.append(newCard);
				endInsertRows();
			}
		}
	}

private:
	QList<ReviewCard*> m_cards;
	ReviewCard::ShowListType m_showListType;
};
