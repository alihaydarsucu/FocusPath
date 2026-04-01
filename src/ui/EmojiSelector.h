#ifndef EMOJI_SELECTOR_H
#define EMOJI_SELECTOR_H

#include <QDialog>
#include <QString>
#include <QGridLayout>

class QLineEdit;
class QPushButton;

class EmojiSelector : public QDialog
{
    Q_OBJECT

public:
    explicit EmojiSelector(const QString &selectedEmoji = "🚀", QWidget *parent = nullptr);
    QString getSelectedEmoji() const { return m_selectedEmoji; }

signals:
    void emojiSelected(const QString &emoji);

private slots:
    void onEmojiClicked(const QString &emoji);
    void onSearchTextChanged(const QString &text);

private:
    void setupUI();
    void populateEmojis();
    void filterEmojis(const QString &searchText);

    QString m_selectedEmoji;
    QLineEdit *searchInput;
    QGridLayout *emojiGridLayout;
    QWidget *emojiContainerWidget;

    struct EmojiInfo {
        QString emoji;
        QString category;
        QString description;
    };

    QVector<EmojiInfo> m_allEmojis;
    QVector<EmojiInfo> m_filteredEmojis;
};

#endif // EMOJI_SELECTOR_H
