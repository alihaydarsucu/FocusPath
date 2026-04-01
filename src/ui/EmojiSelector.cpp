#include "EmojiSelector.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QWidget>
#include <QFont>

EmojiSelector::EmojiSelector(const QString &selectedEmoji, QWidget *parent)
    : QDialog(parent), m_selectedEmoji(selectedEmoji)
{
    setWindowTitle("Select Emoji");
    setWindowModality(Qt::ApplicationModal);
    setMinimumWidth(500);
    setMinimumHeight(400);
    
    setupUI();
    populateEmojis();
}

void EmojiSelector::setupUI()
{
    setStyleSheet(
        "QDialog { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #FAFBFF, stop:0.5 #F0F6FF, stop:1 #E8F4FF); "
        "}"
    );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Search input
    QHBoxLayout *searchLayout = new QHBoxLayout();
    QLabel *searchLabel = new QLabel("Search:", this);
    searchInput = new QLineEdit(this);
    searchInput->setPlaceholderText("Type to search emojis...");
    searchInput->setStyleSheet(
        "QLineEdit { "
        "  border:2px solid #E8F0F7; "
        "  border-radius:8px; "
        "  padding:8px; "
        "  font-size:13px; "
        "  background: white; "
        "} "
        "QLineEdit:focus { "
        "  border:2px solid #0288D1; "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FFFFFF, stop:1 #F0F8FF); "
        "}"
    );
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchInput);
    mainLayout->addLayout(searchLayout);

    // Scrollable emoji grid
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet(
        "QScrollArea { "
        "  border:2px solid #E8F0F7; "
        "  border-radius:8px; "
        "  background: white; "
        "}"
    );

    emojiContainerWidget = new QWidget();
    emojiGridLayout = new QGridLayout(emojiContainerWidget);
    emojiGridLayout->setSpacing(6);
    emojiGridLayout->setContentsMargins(10, 10, 10, 10);

    scrollArea->setWidget(emojiContainerWidget);
    mainLayout->addWidget(scrollArea, 1);

    // OK button
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *okButton = new QPushButton("OK", this);
    okButton->setStyleSheet(
        "QPushButton { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0288D1, stop:1 #0150A8); "
        "  color:white; "
        "  border:none; "
        "  border-radius:6px; "
        "  padding:8px 20px; "
        "  font-weight:600; "
        "} "
        "QPushButton:hover { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0150A8, stop:1 #001A4D); "
        "}"
    );
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(okButton);
    mainLayout->addLayout(buttonLayout);

    connect(searchInput, &QLineEdit::textChanged, this, &EmojiSelector::onSearchTextChanged);
}

void EmojiSelector::populateEmojis()
{
    m_allEmojis = {
        // Smileys & Emotion
        {"😀", "smileys", "grinning face"},
        {"😃", "smileys", "grinning face with big eyes"},
        {"😄", "smileys", "grinning face with smiling eyes"},
        {"😁", "smileys", "beaming face with smiling eyes"},
        {"😆", "smileys", "grinning squinting face"},
        {"😅", "smileys", "grinning face with sweat"},
        {"🤣", "smileys", "rolling on the floor laughing"},
        {"😂", "smileys", "face with tears of joy"},
        {"🙂", "smileys", "slightly smiling face"},
        {"🙃", "smileys", "upside down face"},
        {"😉", "smileys", "winking face"},
        {"😌", "smileys", "relieved face"},
        {"😍", "smileys", "smiling face with heart eyes"},
        {"🥰", "smileys", "smiling face with hearts"},
        {"😘", "smileys", "face blowing a kiss"},
        {"😗", "smileys", "kissing face"},
        {"😚", "smileys", "kissing face with closed eyes"},
        {"😙", "smileys", "kissing face with smiling eyes"},
        // Hand Gestures
        {"👍", "hands", "thumbs up"},
        {"👎", "hands", "thumbs down"},
        {"👊", "hands", "oncoming fist"},
        {"✊", "hands", "raised fist"},
        {"✌️", "hands", "victory hand"},
        {"🤞", "hands", "crossed fingers"},
        {"🤟", "hands", "love you gesture"},
        {"🤘", "hands", "sign of the horns"},
        {"🤙", "hands", "call me hand"},
        {"👋", "hands", "waving hand"},
        {"🖐️", "hands", "hand with fingers splayed"},
        {"✋", "hands", "raised hand"},
        {"🖖", "hands", "vulcan salute"},
        // Objects & Activities
        {"🚀", "objects", "rocket"},
        {"💡", "objects", "light bulb"},
        {"🎯", "objects", "direct hit"},
        {"🔥", "objects", "fire"},
        {"⚡", "objects", "high voltage"},
        {"💻", "objects", "laptop"},
        {"📱", "objects", "mobile phone"},
        {"⌨️", "objects", "keyboard"},
        {"🖥️", "objects", "desktop computer"},
        {"🖨️", "objects", "printer"},
        {"📷", "objects", "camera"},
        {"📹", "objects", "video camera"},
        {"🎥", "objects", "movie camera"},
        {"🎬", "objects", "clapper board"},
        {"📺", "objects", "television"},
        {"📻", "objects", "radio"},
        // Nature & Animals
        {"🌙", "nature", "crescent moon"},
        {"⭐", "nature", "star"},
        {"✨", "nature", "sparkles"},
        {"☀️", "nature", "sun"},
        {"🌈", "nature", "rainbow"},
        {"🌸", "nature", "cherry blossom"},
        {"🌺", "nature", "hibiscus"},
        {"🌻", "nature", "sunflower"},
        {"🌷", "nature", "tulip"},
        {"🌹", "nature", "rose"},
        {"🍎", "nature", "red apple"},
        {"🍊", "nature", "orange"},
        {"🍋", "nature", "lemon"},
        {"🍌", "nature", "banana"},
        {"🍇", "nature", "grapes"},
        // Food
        {"☕", "food", "hot beverage"},
        {"🍵", "food", "teacup without handle"},
        {"🧃", "food", "juice box"},
        {"🥤", "food", "cup with straw"},
        {"🧋", "food", "bubble tea"},
        {"🍰", "food", "cake"},
        {"🎂", "food", "birthday cake"},
        {"🍪", "food", "cookie"},
        {"🍩", "food", "doughnut"},
        {"🍫", "food", "chocolate bar"},
        {"🍬", "food", "candy"},
        // Learning & Knowledge
        {"📚", "learning", "books"},
        {"📖", "learning", "open book"},
        {"📝", "learning", "memo"},
        {"✏️", "learning", "pencil"},
        {"📓", "learning", "notebook"},
        {"📔", "learning", "notebook with decorative cover"},
        {"📕", "learning", "closed book with red cover"},
        {"📗", "learning", "closed book with green cover"},
        {"📘", "learning", "closed book with blue cover"},
        {"📙", "learning", "closed book with brown cover"},
        {"📒", "learning", "ledger"},
        {"📑", "learning", "bookmark tabs"},
        // Work & Productivity
        {"💼", "work", "briefcase"},
        {"📊", "work", "bar chart"},
        {"📈", "work", "chart increasing"},
        {"📉", "work", "chart decreasing"},
        {"📋", "work", "clipboard"},
        {"📌", "work", "pushpin"},
        {"📍", "work", "round pushpin"},
        {"📎", "work", "paperclip"},
        {"✒️", "work", "black nib"},
        {"🖋️", "work", "fountain pen"},
        {"🖊️", "work", "pen"},
        // Sports & Wellness
        {"⚽", "sports", "soccer ball"},
        {"🏀", "sports", "basketball"},
        {"🏈", "sports", "american football"},
        {"⚾", "sports", "baseball"},
        {"🎾", "sports", "tennis"},
        {"🏐", "sports", "volleyball"},
        {"🏉", "sports", "rugby football"},
        {"🥎", "sports", "softball"},
        {"🎳", "sports", "bowling"},
        {"🧘", "wellness", "person cartwheeling"},
        {"🏃", "wellness", "person running"},
        {"🚴", "wellness", "person biking"},
        {"🏊", "wellness", "person swimming"},
        // Music & Art
        {"🎵", "music", "musical note"},
        {"🎶", "music", "multiple musical notes"},
        {"🎤", "music", "microphone"},
        {"🎧", "music", "headphones"},
        {"🎸", "music", "guitar"},
        {"🎹", "music", "musical keyboard"},
        {"🎺", "music", "trumpet"},
        {"🎷", "music", "saxophone"},
        {"🎻", "music", "violin"},
        {"🥁", "music", "drum"},
        {"🎨", "art", "artist palette"},
        {"🖌️", "art", "paintbrush"},
        {"🖍️", "art", "crayon"},
        // Science & Lab
        {"🔬", "science", "microscope"},
        {"🔭", "science", "telescope"},
        {"🧪", "science", "test tube"},
        {"🧬", "science", "dna"},
        {"⚗️", "science", "alembic"},
        {"⚛️", "science", "atom symbol"},
        {"🔧", "tools", "wrench"},
        {"🔨", "tools", "hammer"},
        // Misc
        {"🧩", "misc", "puzzle piece"},
        {"❤️", "misc", "red heart"},
        {"💚", "misc", "green heart"},
        {"💙", "misc", "blue heart"},
        {"💛", "misc", "yellow heart"},
        {"💜", "misc", "purple heart"},
    };

    m_filteredEmojis = m_allEmojis;
    
    // Display all emojis initially
    int colCount = 6;
    for (int i = 0; i < m_filteredEmojis.size(); ++i) {
        int row = i / colCount;
        int col = i % colCount;

        QPushButton *btn = new QPushButton(m_filteredEmojis[i].emoji);
        btn->setFixedSize(60, 60);
        
        // Set font for better emoji rendering
        QFont font = btn->font();
        font.setPointSize(28);
        font.setFamily("Noto Color Emoji,Apple Color Emoji,Segoe UI Emoji,Sans Serif");
        btn->setFont(font);
        
        btn->setStyleSheet(
            "QPushButton { background:#F7F7F7; border:1px solid #E0E0E0; border-radius:8px; }"
            "QPushButton:hover { background:#E0E0E0; }"
        );

        if (m_filteredEmojis[i].emoji == m_selectedEmoji) {
            btn->setStyleSheet(
                "QPushButton { background:#0078D4; border:2px solid #0078D4; border-radius:8px; }"
            );
        }

        connect(btn, &QPushButton::clicked, this, [this, emoji = m_filteredEmojis[i].emoji]() {
            onEmojiClicked(emoji);
        });

        emojiGridLayout->addWidget(btn, row, col);
    }
}

void EmojiSelector::onEmojiClicked(const QString &emoji)
{
    m_selectedEmoji = emoji;
    emit emojiSelected(emoji);

    // Update button styles to reflect selection
    for (int i = 0; i < emojiGridLayout->count(); ++i) {
        auto btn = qobject_cast<QPushButton*>(emojiGridLayout->itemAt(i)->widget());
        if (btn) {
            if (btn->text() == emoji) {
                btn->setStyleSheet(
                    "QPushButton { background:#0078D4; border:2px solid #0078D4; border-radius:8px; font-size:24px; }"
                );
            } else {
                btn->setStyleSheet(
                    "QPushButton { background:#F7F7F7; border:1px solid #E0E0E0; border-radius:8px; font-size:24px; }"
                    "QPushButton:hover { background:#E0E0E0; }"
                );
            }
        }
    }
}

void EmojiSelector::onSearchTextChanged(const QString &text)
{
    filterEmojis(text);
}

void EmojiSelector::filterEmojis(const QString &searchText)
{
    // Clear existing buttons
    while (emojiGridLayout->count() > 0) {
        delete emojiGridLayout->takeAt(0)->widget();
    }

    m_filteredEmojis.clear();

    // Filter emojis based on search text
    for (const auto &info : m_allEmojis) {
        if (searchText.isEmpty() || 
            info.category.contains(searchText, Qt::CaseInsensitive) ||
            info.description.contains(searchText, Qt::CaseInsensitive)) {
            m_filteredEmojis.append(info);
        }
    }

    // Populate grid with filtered emojis
    int colCount = 6;
    for (int i = 0; i < m_filteredEmojis.size(); ++i) {
        int row = i / colCount;
        int col = i % colCount;

        QPushButton *btn = new QPushButton(m_filteredEmojis[i].emoji);
        btn->setFixedSize(60, 60);
        
        // Set font for better emoji rendering
        QFont font = btn->font();
        font.setPointSize(28);
        font.setFamily("Noto Color Emoji,Apple Color Emoji,Segoe UI Emoji,Sans Serif");
        btn->setFont(font);
        
        btn->setStyleSheet(
            "QPushButton { background:#F7F7F7; border:1px solid #E0E0E0; border-radius:8px; }"
            "QPushButton:hover { background:#E0E0E0; }"
        );

        if (m_filteredEmojis[i].emoji == m_selectedEmoji) {
            btn->setStyleSheet(
                "QPushButton { background:#0078D4; border:2px solid #0078D4; border-radius:8px; }"
            );
        }

        connect(btn, &QPushButton::clicked, this, [this, emoji = m_filteredEmojis[i].emoji]() {
            onEmojiClicked(emoji);
        });

        emojiGridLayout->addWidget(btn, row, col);
    }

    // Add stretch at the end
    emojiGridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding),
                             (m_filteredEmojis.size() + 5) / 6, 0, -1, -1);
}
