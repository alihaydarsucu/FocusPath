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
    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(120, 40, 120, 50);

    appsSetup = new ClickableLabel();
    durationSetup = new ClickableLabel();
    QPushButton *createWorkflowButton = new QPushButton("Create Workflow");

    appsSetup->setText("Apps Setup");
    durationSetup->setText("Duration Setup");




    QFrame *mainCard = new QFrame(this);

    root->addWidget(mainCard);
    root->addWidget(appsSetup);
    root->addWidget(durationSetup);
    root->addWidget(createWorkflowButton);


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
    templateName = new QLineEdit();
    templateName->setPlaceholderText("Give a name your template..");


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

    connect(createWorkflowButton, &QPushButton::clicked, this, [=]() {
        onBackClicked();
    });
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
                     ? Workflow(totalMinutes, false)
                     : Workflow(value.toStdString(), totalMinutes, true);


    for (int i = 0; i < selectedList->count(); ++i) {
        QListWidgetItem* item = selectedList->item(i);
        w.addApps(item->text().toStdString());
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
    QMessageBox msg(this);
    msg.setWindowTitle("Confirm");
    msg.setText("Are you sure you want to save this Workflow!!");
    msg.setInformativeText("Workflow session will begin.");
    msg.setIcon(QMessageBox::Warning);

    QPushButton *yesBtn = msg.addButton("Yes", QMessageBox::AcceptRole);
    msg.addButton("Cancel", QMessageBox::RejectRole);

    msg.exec();

    if (msg.clickedButton() == yesBtn) {
        createWorkflow(slider->value(), false);
    }
}










