#pragma once

#include <QPlainTextEdit>
#include <QMimeData>
#include <QImage>
#include <QDir>
#include <QDateTime>
#include <QTextBlock>

class MarkdownPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit MarkdownPlainTextEdit(QWidget* parent = nullptr) : QPlainTextEdit(parent) {}

protected:
    void insertFromMimeData(const QMimeData* source) override
    {
        if (source->hasImage()) {
            QImage image = qvariant_cast<QImage>(source->imageData());
            QString fileName = QDir::temp().filePath(
                QString("pasted_%1.png").arg(QDateTime::currentMSecsSinceEpoch()));
            image.save(fileName, "PNG");
            // 插入 Markdown 图片语法
            this->insertPlainText(QString("![Images](%1)\n").arg(fileName));
        }
        else {
            QPlainTextEdit::insertFromMimeData(source);
        }
    }

    void keyPressEvent(QKeyEvent* event) override
    {
        if (event->key() == Qt::Key_Tab) {
            QTextCursor cursor = textCursor();
            if (cursor.hasSelection()) {
                int start = cursor.selectionStart();
                int end = cursor.selectionEnd();
                cursor.setPosition(start);
                int firstBlock = cursor.blockNumber();

                cursor.setPosition(end, QTextCursor::KeepAnchor);
                int lastBlock = cursor.blockNumber();

                // 记录原始光标
                int selStart = cursor.selectionStart();
                int selEnd = cursor.selectionEnd();

                cursor.beginEditBlock();
                for (int i = firstBlock; i <= lastBlock; ++i) {
                    QTextBlock block = document()->findBlockByNumber(i);
                    QTextCursor blockCursor(block);
                    blockCursor.insertText("\t"); // 或用空格替代
                }
                cursor.endEditBlock();

                // 重新选中原区域（向右偏移）
                QTextCursor newCursor = textCursor();
                newCursor.setPosition(selStart);
                newCursor.setPosition(selEnd + (lastBlock - firstBlock + 1), QTextCursor::KeepAnchor);
                setTextCursor(newCursor);
                return;
            }
        }
        QPlainTextEdit::keyPressEvent(event);
    }
};
