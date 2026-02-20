#include "reviewapp.h"

ReviewApp::ReviewApp(QWidget *parent)
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

	m_selectionModel = new QItemSelectionModel(m_filter, this);
	ui.cardListView->setSelectionModel(m_selectionModel);

	m_delegate = new CardStyleDelegate(this);
	ui.cardListView->setItemDelegate(m_delegate);

	connect(m_selectionModel, &QItemSelectionModel::currentChanged, this, &ReviewApp::on_currentIndex_changed);

	new CodeHighlighter(ui.answerTextBox->document());
}

ReviewApp::~ReviewApp()
{}

void ReviewApp::setShowListType(ReviewCard::ShowListType type)
{
	m_model->setShowListType(type);
}

void ReviewApp::on_actionAdd_triggered()
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

void ReviewApp::on_currentIndex_changed(const QModelIndex& current, const QModelIndex& previous)
{
	Q_UNUSED(previous);
	if (!current.isValid()) return;
	ReviewCard* card = current.data(Qt::UserRole + 1).value<ReviewCard*>();
	if (!card) return;

	ui.questionTextBox->setPlainText(card->question);
	ui.answerTextBox->setMarkdown(card->answer);
}

