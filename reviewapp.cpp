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

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

	QString appDir = QCoreApplication::applicationDirPath();
	QString datebasePath = QDir(appDir).filePath("review.db");

	bool dbFileExist = QFile(datebasePath).exists();
	db.setDatabaseName(datebasePath);
	if (db.open())
	{
		qDebug() << "Database opened successfully.";

		if (!dbFileExist)
		{
			// create table
			QSqlQuery query;
			QString createTableSQL = R"(
				CREATE TABLE records(
					id integer primary key autoincrement,
					question text not null,
					answer text,
					category text,
					tags text,
					level integer not null default(0),
					create_time datetime not null default(datetime('now', 'localtime')),
					review_time datetime not null default(datetime('now', 'localtime')),
					fav integer default 0
				)
			)";
			if (!query.exec(createTableSQL))
			{
				qCritical() << "Failed to create table records:" << query.lastError().text();
			}
			else
			{
				qDebug() << "Table records created successfully.";
			}
		}
	}
	else
	{
		qDebug() << "Failed to open database:" << db.lastError().text();
	}

	m_model = new ReviewCardListModel(this);
	setShowListType(ReviewCard::Review); //  default is review list
	m_filter = new ReviewFilter(this);
	
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

void ReviewApp::on_editBtn_clicked()
{
	auto index = ui.cardListView->currentIndex();
	if (!index.isValid()) return;

	ReviewCard* card = index.data(Qt::UserRole + 1).value<ReviewCard*>();
	if (!card) return;
	EditDialog dialog(card);
	dialog.setMinimumWidth(600);
	dialog.setMinimumHeight(400);
	if (dialog.exec())
	{
		auto result = dialog.getResult();
		// update database
		QSqlQuery query;
		query.prepare(R"(update records set question = :question, answer = :answer, category = :category, tags = :tags where id = :id)");
		query.bindValue(":question", result.question);
		query.bindValue(":answer", result.answer);
		query.bindValue(":category", result.category);
		query.bindValue(":tags", result.tags);
		query.bindValue(":id", card->id);

		if (!query.exec())
		{
			qCritical() << "Failed to update record:" << query.lastError().text();
		}
		else
		{
			qDebug() << "Record updated successfully.";
			card->question = result.question;
			card->answer = result.answer;
			card->category = result.category;
			card->tags = result.tags;
			ui.questionTextBox->setPlainText(card->question);
			ui.answerTextBox->setMarkdown(card->answer);
		}
	}
}

void ReviewApp::on_deleteBtn_clicked()
{
	auto index = ui.cardListView->currentIndex();
	if (!index.isValid()) return;

	if (QMessageBox::Ok == QMessageBox::warning(this, "Warning", "Are you sure you want to delete?"))
	{
		QModelIndex sourceIndex = m_filter->mapToSource(index);
		QString error = m_model->remove(sourceIndex);
		if (!error.isEmpty())
		{
			QMessageBox::critical(this, "Error", error);
		}
	}
}

void ReviewApp::on_favBtn_clicked(bool checked)
{
}

void ReviewApp::on_viewAnswerBtn_clicked()
{
}

