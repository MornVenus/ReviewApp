#pragma once
#include <QTextEdit>
#include <QDateTime>
#include <QMimeData>
#include <QDir>

class MarkdownTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit MarkdownTextEdit(QWidget* parent = nullptr) : QTextEdit(parent) {}

protected:
    void insertFromMimeData(const QMimeData* source) override
    {
        if (source->hasImage()) {
            QImage image = qvariant_cast<QImage>(source->imageData());
            // 保存图片到本地临时文件
            QString fileName = QDir::temp().filePath(
                QString("pasted_%1.png").arg(QDateTime::currentMSecsSinceEpoch()));
            image.save(fileName, "PNG");
            // 插入 Markdown 图片语法
            this->insertPlainText(QString("![图片](%1)\n").arg(fileName));
        }
        else {
            QTextEdit::insertFromMimeData(source);
        }
    }
};
