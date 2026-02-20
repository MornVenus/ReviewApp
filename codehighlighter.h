#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class CodeHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit CodeHighlighter(QTextDocument* parent)
        : QSyntaxHighlighter(parent)
    {
        // 关键字格式
        keywordFormat.setForeground(Qt::blue);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatternsList = {
            "\\bclass\\b", "\\bconst\\b", "\\bvoid\\b", "\\bint\\b", "\\bfloat\\b",
            "\\bdouble\\b", "\\bQString\\b", "\\bpublic\\b", "\\bprivate\\b", "\\bprotected\\b",
            "\\bif\\b", "\\belse\\b", "\\bfor\\b", "\\bwhile\\b", "\\breturn\\b"
        };
        for (const QString& pattern : keywordPatternsList) {
            keywordPatterns.append(QRegularExpression(pattern));
        }

        // 注释格式
        commentFormat.setForeground(Qt::darkGreen);
        commentFormat.setFontItalic(true);

        // 字符串格式
        stringFormat.setForeground(Qt::darkRed);
    }

protected:
    void highlightBlock(const QString& text) override
    {
        // 高亮关键字
        for (const QRegularExpression& pattern : keywordPatterns) {
            QRegularExpressionMatchIterator i = pattern.globalMatch(text);
            while (i.hasNext()) {
                QRegularExpressionMatch match = i.next();
                setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);
            }
        }

        // 高亮字符串
        QRegularExpression stringPattern("\".*?\"");
        QRegularExpressionMatchIterator stringIt = stringPattern.globalMatch(text);
        while (stringIt.hasNext()) {
            QRegularExpressionMatch match = stringIt.next();
            setFormat(match.capturedStart(), match.capturedLength(), stringFormat);
        }

        // 高亮注释
        QRegularExpression commentPattern("//[^\n]*");
        QRegularExpressionMatchIterator commentIt = commentPattern.globalMatch(text);
        while (commentIt.hasNext()) {
            QRegularExpressionMatch match = commentIt.next();
            setFormat(match.capturedStart(), match.capturedLength(), commentFormat);
        }
    }

private:
    QTextCharFormat keywordFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat stringFormat;
    QList<QRegularExpression> keywordPatterns;
};