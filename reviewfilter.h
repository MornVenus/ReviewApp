#pragma once
#include <QSortFilterProxyModel>
#include "reviewcard.h"

class ReviewFilter: public QSortFilterProxyModel
{
	Q_OBJECT
public:
	ReviewFilter(QObject* parent) : QSortFilterProxyModel(parent) {}

	void setFilterText(const QString& text)
	{
		m_filterText = text.trimmed();
		invalidateFilter();
	}

protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex& parent) const override
	{
		// 如果没有过滤文本，显示所有行
		if (m_filterText.isEmpty())
			return true;

		QModelIndex index = sourceModel()->index(sourceRow, 0, parent);
		if (!index.isValid())
			return false;

		// 获取 ReviewRecord
		ReviewCard* card = index.data(Qt::UserRole + 1).value<ReviewCard*>();
		if (!card)
			return false;

		// 检查 question 或 answer 是否包含过滤文本（不区分大小写）
		return card->question.contains(m_filterText, Qt::CaseInsensitive) ||
			card->answer.contains(m_filterText, Qt::CaseInsensitive);
	}

private:
	QString m_filterText;
};
