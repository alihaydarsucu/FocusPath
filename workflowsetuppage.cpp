#include "workflowsetuppage.h"
#include "FocusCard.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QListWidget>
#include <QStackedWidget>
#include <QDir>
#include <QSettings>
#include <QFile>
#include <QIcon>
#include <QSize>
#include <QPushButton>
#include <QDebug>
#include <QLineEdit>
#include <QMessageBox>
#include <QStandardPaths>
#include <QCoreApplication>



WorkflowSetupPage::WorkflowSetupPage(QWidget *parent)
    : QWidget(parent)
{
    selectedIcon = "🚀";

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(120, 30, 120, 40);

    appsSetup = new ClickableLabel();
    durationSetup = new ClickableLabel();
    startWorkflowButton = new QPushButton("Start Session");
    backButton = new QPushButton("Back");

    appsSetup->setText("Apps Setup");
    durationSetup->setText("Duration Setup");




    QFrame *mainCard = new QFrame(this);

    root->addWidget(mainCard);
    root->addWidget(appsSetup);
    root->addWidget(durationSetup);


    mainCard->setStyleSheet(
        "QFrame { background:#ffffff; border-radius:24px; border:1px solid #eee; }"
        );

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(30);
    shadow->setYOffset(10);
    shadow->setColor(QColor(0, 0, 0, 30));
    mainCard->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(mainCard);
    cardLayout->setContentsMargins(30, 30, 30, 30);
    cardLayout->setSpacing(20);





    stacked = new QStackedWidget(mainCard);
    cardLayout->addWidget(stacked);



    QWidget *pageOne = new QWidget;
    QHBoxLayout *pageOneLayout = new QHBoxLayout(pageOne);
    pageOneLayout->setSpacing(20);

    QLabel *title = new QLabel("Allowed Applications");
    title->setStyleSheet("font-size:28px; font-weight:300;");
    pageOneLayout->addWidget(title);

    allAppsList = new QListWidget;
    selectedList = new QListWidget;

    allAppsList->setIconSize(QSize(24, 24));
    selectedList->setIconSize(QSize(24, 24));

    allAppsList->setStyleSheet(
        "QListWidget { border:1px solid #ddd; border-radius:12px; padding:6px; }"
        );
    selectedList->setStyleSheet(
        "QListWidget { border:1px solid #ddd; border-radius:12px; padding:6px; }"
        );

    pageOneLayout->addWidget(allAppsList);
    pageOneLayout->addWidget(selectedList);

    stacked->addWidget(pageOne);



    QWidget *pageTwo = new QWidget;
    QVBoxLayout *pageTwoLayout = new QVBoxLayout(pageTwo);

    pageTwo->setStyleSheet("background-color: #FFFFFF;");
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("Session Duration");


    timeLabel = new QLabel("0m");
    QFont font = timeLabel->font();
    font.setPointSize(16);
    font.setBold(true);
    timeLabel->setFont(font);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(timeLabel);

    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 180);
    slider->setValue(45);

    QHBoxLayout *sliderInfoLayout = new QHBoxLayout();
    sliderInfoLayout->addWidget(new QLabel("0m"));
    sliderInfoLayout->addStretch();
    sliderInfoLayout->addWidget(new QLabel("3h"));

    QVBoxLayout *sliderContainer = new QVBoxLayout();
    sliderContainer->addWidget(slider);
    sliderContainer->addLayout(sliderInfoLayout);


    QHBoxLayout *buttonsLayout = new QHBoxLayout();


    QPushButton *btnShort = new QPushButton("Short (30m)");
    QPushButton *btnFocus = new QPushButton("Focus (1h 15m)");
    QPushButton *btnLong  = new QPushButton("Deep (2h)");

    buttonsLayout->addWidget(btnShort);
    buttonsLayout->addWidget(btnFocus);
    buttonsLayout->addWidget(btnLong);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setStyleSheet("background:#F8F9FA;");

    QLabel *titleTemplate = new QLabel("Save as Template");
    titleTemplate->setStyleSheet("font-size:16px; font-weight:600;");
    templateName = new QLineEdit();
    templateName->setPlaceholderText("Give a name your template..");
    templateName->setStyleSheet(
        "QLineEdit { border:1px solid #ddd; border-radius:10px; padding:10px; font-size:14px; }"
    );

    // Icon selection row
    setupIconDisplay();

    QLabel *iconHint = new QLabel("Pick an icon for this template");
    iconHint->setStyleSheet("font-size:13px; color:#6C757D;");

    QGridLayout *iconButtonsLayout = new QGridLayout();
    iconButtonsLayout->setHorizontalSpacing(8);
    iconButtonsLayout->setVerticalSpacing(8);
    QStringList emojiList {
        "🔥","🎨","✉️","📚","🧘","🚀","💡","🎯","💻","🎵","🍃","☕","📖","🧪","🧩","🎬","🌙","🏃"
    };
    int colCount = 6;
    int idx = 0;
    for (const QString &emoji : emojiList) {
        int r = idx / colCount;
        int c = idx % colCount;
        QPushButton *btn = new QPushButton(emoji);
        btn->setCheckable(true);
        btn->setStyleSheet(
            "QPushButton { background:#F7F7F7; border:1px solid #E0E0E0; border-radius:10px; padding:8px 12px; font-size:16px; }"
            "QPushButton:checked { background:#0078D4; color:white; border-color:#0078D4; }"
        );
        if (emoji == QString::fromStdString(selectedIcon)) {
            btn->setChecked(true);
        }
        connect(btn, &QPushButton::clicked, this, [this, btn, emoji, iconButtonsLayout]() {
            for (int i = 0; i < iconButtonsLayout->count(); ++i) {
                auto otherBtn = qobject_cast<QPushButton*>(iconButtonsLayout->itemAt(i)->widget());
                if (otherBtn && otherBtn != btn) {
                    otherBtn->setChecked(false);
                }
            }
            selectedIcon = emoji.toStdString();
            updateIconDisplay();
        });
        iconButtonsLayout->addWidget(btn, r, c);
        idx++;
    }


    pageTwoLayout->addLayout(headerLayout);
    pageTwoLayout->addSpacing(15);
    pageTwoLayout->addLayout(sliderContainer);
    pageTwoLayout->addSpacing(15);
    pageTwoLayout->addWidget(new QLabel("Quick Presets"));
    pageTwoLayout->addLayout(buttonsLayout);
    pageTwoLayout->addStretch();
    pageTwoLayout->addWidget(line);
    pageTwoLayout->addSpacing(15);
    pageTwoLayout->addWidget(titleTemplate);
    pageTwoLayout->addWidget(templateName);
    pageTwoLayout->addSpacing(8);
    pageTwoLayout->addWidget(iconHint);
    QHBoxLayout *iconRow = new QHBoxLayout();
    iconRow->addWidget(iconDisplayFrame);
    iconRow->addLayout(iconButtonsLayout);
    iconRow->addStretch();
    pageTwoLayout->addLayout(iconRow);




    connect(slider, &QSlider::valueChanged, this, &WorkflowSetupPage::updateLabel);


    connect(btnShort, &QPushButton::clicked, [=](){ slider->setValue(30); });
    connect(btnFocus, &QPushButton::clicked, [=](){ slider->setValue(75); });
    connect(btnLong,  &QPushButton::clicked, [=](){ slider->setValue(120); });


    updateLabel(slider->value());


    stacked->addWidget(pageTwo);


    stacked->setCurrentIndex(0);



    loadLinuxApps();



    connect(allAppsList, &QListWidget::itemClicked, this,
            [this](QListWidgetItem *item) {

                for (int i = 0; i < selectedList->count(); ++i)
                    if (selectedList->item(i)->text() == item->text())
                        return;

                selectedList->addItem(
                    new QListWidgetItem(item->icon(), item->text())
                    );
            });

    connect(selectedList, &QListWidget::itemClicked,
            this, [this](QListWidgetItem *item) {
                delete selectedList->takeItem(selectedList->row(item));
            });


    connect(appsSetup, &ClickableLabel::clicked, this, [=]() {
        stacked->setCurrentIndex(0);
    });

    connect(durationSetup, &ClickableLabel::clicked, this, [=]() {
        stacked->setCurrentIndex(1);
    });

    QHBoxLayout *actionRow = new QHBoxLayout();
    actionRow->addWidget(backButton);
    actionRow->addStretch();
    actionRow->addWidget(startWorkflowButton);

    backButton->setStyleSheet(
        "QPushButton { background:#F2F4F5; border:1px solid #E0E0E0; border-radius:10px; padding:10px 16px; font-weight:600; }"
        "QPushButton:hover { background:#e9ecef; }"
    );
    startWorkflowButton->setStyleSheet(
        "QPushButton { background:#0078D4; color:white; border:none; border-radius:10px; padding:10px 18px; font-weight:700; }"
        "QPushButton:hover { background:#005A9E; }"
    );

    connect(startWorkflowButton, &QPushButton::clicked, this, &WorkflowSetupPage::onStartWorkflowClicked);
    connect(backButton, &QPushButton::clicked, this, &WorkflowSetupPage::onBackClicked);

    root->addSpacing(12);
    root->addLayout(actionRow);
}



void WorkflowSetupPage::loadLinuxApps()
{
    allAppsList->clear();

    QStringList dirs = {
        "/usr/share/applications",
        "/usr/local/share/applications",
        QDir::homePath() + "/.local/share/applications"
    };

    for (const QString &dirPath : dirs) {
        QDir dir(dirPath);
        if (!dir.exists()) continue;

        for (const QString &file :
             dir.entryList(QStringList() << "*.desktop", QDir::Files)) {

            QSettings f(dir.absoluteFilePath(file), QSettings::IniFormat);
            f.beginGroup("Desktop Entry");

            if (f.value("Type") != "Application")
                continue;

            QString name = f.value("Name").toString();
            QString iconName = f.value("Icon").toString();
            if (name.isEmpty())
                continue;

            QIcon icon = QFile::exists(iconName)
                             ? QIcon(iconName)
                             : QIcon::fromTheme(iconName);

            allAppsList->addItem(new QListWidgetItem(icon, name));
        }
    }
}
void WorkflowSetupPage::updateLabel(int totalMinutes) {
    int hours = totalMinutes / 60;
    int minutes = totalMinutes % 60;

    QString timeText;

    if (hours > 0 && minutes > 0) {

        timeText = QString("%1h %2m").arg(hours).arg(minutes);
    }
    else if (hours > 0) {

        timeText = QString("%1h").arg(hours);
    }
    else {

        timeText = QString("%1m").arg(minutes);
    }

    timeLabel->setText(timeText);
}


void WorkflowSetupPage::createWorkflow(int totalMinutes, bool isFavorite)
{

    if (selectedList->count() == 0) {
        QMessageBox::warning(
            this,
            "Workflow Creation Failed",
            "You cannot save a workflow without adding at least one application."
            );
        return;
    }

    QString value = templateName->text().trimmed();

    Workflow w = value.isEmpty()
                     ? Workflow(totalMinutes, isFavorite)
                     : Workflow(value.toStdString(), totalMinutes, isFavorite);


    for (int i = 0; i < selectedList->count(); ++i) {
        QListWidgetItem* item = selectedList->item(i);
        w.addApps(item->text().toStdString());
    }

    if (!selectedIcon.empty()) {
        w.setIcon(selectedIcon);
    }

    // Create workflows directory if it doesn't exist
    QString workflowsDir = QCoreApplication::applicationDirPath() + "/workflows";
    QDir dir;
    if (!dir.exists(workflowsDir)) {
        dir.mkpath(workflowsDir);
        qDebug() << "Created workflows directory:" << workflowsDir;
    }

    QString filePath = QString("%1/%2.json").arg(workflowsDir, QString::fromStdString(w.getDate()));
    
    qDebug() << "Saving workflow to:" << filePath;
    Workflow::saveWorkflowToFile(w, filePath);
    qDebug() << "Workflow saved successfully";

    emit startWorkflow(w);

    w.print();
}

void WorkflowSetupPage::onBackClicked()
{
    emit backToDashboard();
}

void WorkflowSetupPage::onStartWorkflowClicked()
{
    createWorkflow(slider->value(), true);
}

void WorkflowSetupPage::setupIconDisplay()
{
    iconDisplayFrame = new QFrame();
    iconDisplayFrame->setFixedSize(52, 52);
    iconDisplayFrame->setStyleSheet(
        "QFrame { background:#E8F4F8; border-radius:16px; border:1px solid #D9E9F2; }"
    );

    QVBoxLayout *iconLayout = new QVBoxLayout(iconDisplayFrame);
    iconLayout->setContentsMargins(0, 0, 0, 0);
    iconLayout->setAlignment(Qt::AlignCenter);

    iconLabel = new QLabel(iconDisplayFrame);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size:20px; font-weight:700; color:#0A2540;");
    iconLayout->addWidget(iconLabel);

    updateIconDisplay();
}

void WorkflowSetupPage::updateIconDisplay()
{
    if (iconLabel) {
        iconLabel->setText(QString::fromStdString(selectedIcon));
    }
}

void WorkflowSetupPage::loadTemplateWorkflow(const Workflow &workflow)
{
    templateName->setText(QString::fromStdString(workflow.getName()));
    slider->setValue(static_cast<int>(workflow.getDuration()));
    updateLabel(slider->value());

    selectedList->clear();
    for (const auto &app : workflow.getApps()) {
        selectedList->addItem(new QListWidgetItem(QString::fromStdString(app)));
    }

    selectedIcon = workflow.getIcon();
    updateIconDisplay();
}



















