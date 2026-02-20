#include "reviewapp.h"

reviewapp::reviewapp(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	QActionGroup* group = new QActionGroup(this);
	group->setExclusive(true);
	group->addAction(ui.actionAll);
	group->addAction(ui.actionReview);
	group->addAction(ui.actionFav);
	group->addAction(ui.actionTest);

	m_model = new ReviewCardListModel(this);

	m_filter = new ReviewFilter(this);
	setShowListType(ReviewCard::Review); //  default is review list
	m_filter->setSourceModel(m_model);

	ui.cardListView->setModel(m_filter);

	

	m_delegate = new CardStyleDelegate(this);
	ui.cardListView->setItemDelegate(m_delegate);

	m_filter->setFilterText(""); // default show all cards in review list
}

reviewapp::~reviewapp()
{}

void reviewapp::setShowListType(ReviewCard::ShowListType type)
{
	m_model->setShowListType(type);
}

void reviewapp::on_actionAdd_triggered()
{
	EditDialog dialog;
	dialog.setMinimumWidth(600);
	dialog.setMinimumHeight(400);
	if (dialog.exec())
	{
		auto result = dialog.getResult();
		m_model->append(result);
	}
}

