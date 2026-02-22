#pragma once

#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QPainter>
#include "reviewcard.h"
#include "theme.h"

class CardStyleDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	CardStyleDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override
	{
		Q_UNUSED(option);
		Q_UNUSED(index);
		return QSize(option.rect.width(), 72); // Set a fixed size for each card item
	}

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
	{
		if (!index.isValid()) return;

		ReviewCard* card = index.data(Qt::UserRole + 1).value<ReviewCard*>();
		if (!card) return;

		Theme theme = ThemeManager::instance()->getCurrentTheme();

		bool isSelected = option.state & QStyle::State_Selected;
		bool isHovered = option.state & QStyle::State_MouseOver;
		QColor textColor = theme.text;
		QColor mutedColor = theme.textMuted;

		QStyleOptionViewItem opt = option;
		initStyleOption(&opt, index);

		int margin = 6;
		int padding = 16;
		QRect contentRect = opt.rect.adjusted(margin, margin, -margin, -margin);
		int contentX = contentRect.left() + padding;
		int contentY = contentRect.top() + padding;
		int contentWidth = contentRect.width() - padding * 2;

		painter->save();
		painter->setRenderHint(QPainter::Antialiasing, true);
		painter->setRenderHint(QPainter::TextAntialiasing, true);

		// paint background
		QColor background = theme.background;
		if (isSelected)
		{
			QColor selectedColor = theme.selected;
			QLinearGradient gradient(contentRect.topLeft(), contentRect.bottomLeft());
			gradient.setColorAt(0, selectedColor);
			gradient.setColorAt(1, selectedColor.darker(110));
			
			painter->setBrush(gradient);
			painter->setPen(Qt::NoPen);
			painter->drawRoundedRect(contentRect, 10, 10);

			textColor = opt.palette.highlightedText().color();
			mutedColor = opt.palette.highlightedText().color().darker(150);
		}
		else if (isHovered)
		{
			QColor hoverColor = theme.hover;
			QLinearGradient gradient(contentRect.topLeft(), contentRect.bottomLeft());
			gradient.setColorAt(0, hoverColor.lighter(105));
			gradient.setColorAt(1, hoverColor);

			painter->setBrush(gradient);
			painter->setPen(Qt::NoPen);
			painter->drawRoundedRect(contentRect, 10, 10);

			textColor = opt.palette.text().color();
			mutedColor = opt.palette.text().color().darker(150);
		}
		else
		{
			painter->setPen(Qt::NoPen);
			painter->setBrush(theme.surface);
			painter->drawRoundedRect(contentRect, 10, 10);
			textColor = opt.palette.text().color();
			mutedColor = opt.palette.text().color().darker(150);
		}

		// paint question 
		QString question = card->question;
		if (question.length() > 45)
		{
			question = question.left(45) + "...";
		}
		QFont questionFont = painter->font();
		questionFont.setBold(true);
		questionFont.setPointSize(questionFont.pointSize() + 1);
		QFontMetrics fm(questionFont);
		int questionHeight = fm.height();
		painter->setFont(questionFont);
		painter->setPen(textColor);

		QRect questionRect(contentX, contentY, contentWidth, questionHeight);
		painter->drawText(questionRect, Qt::AlignLeft | Qt::AlignVCenter, question);

		// paint category and tags
		QStringList info;
		if (!card->category.isEmpty())
		{
			info.append(card->category);
		}
		if (!card->tags.isEmpty())
		{
			info.append(card->tags);
		}

		if (!info.isEmpty())
		{
			QString infoText = info.join(" | ");
			if (infoText.length() > 55) infoText = infoText.left(55) + "...";
			QFont infoFont = painter->font();
			infoFont.setPointSize(infoFont.pointSize() - 1);
			infoFont.setBold(false);
			painter->setFont(infoFont);
			QFontMetrics infoFm(infoFont);
			int infoHeight = infoFm.height();
			QRect infoRect(contentX, contentY + questionHeight + 6, contentWidth, infoHeight);
			painter->setPen(mutedColor);
			painter->drawText(infoRect, Qt::AlignLeft | Qt::AlignVCenter, infoText);
		}
		painter->restore();
	}
};
