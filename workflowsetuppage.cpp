#include "workflowsetuppage.h"
#include "emoji_selector.h"

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
#include <QCheckBox>
#include <QSet>
#include <QIcon>

// Resolve icon paths from .desktop entries, handling env vars and common icon locations.
static QString resolveIconPath(const QString &rawIcon, const QString &desktopDir)
{
    if (rawIcon.isEmpty()) return QString();

    QString icon = rawIcon;

    const QString snap = qEnvironmentVariable("SNAP");
    if (!snap.isEmpty()) {
        icon.replace("${SNAP}", snap);
        icon.replace("$SNAP", snap);
    }
    const QString home = QDir::homePath();
    icon.replace("${HOME}", home);
    icon.replace("$HOME", home);

    auto exists = [](const QString &path) { return !path.isEmpty() && QFile::exists(path); };

    if (QDir::isAbsolutePath(icon) && exists(icon)) return icon;

    if (!QDir::isAbsolutePath(icon) && icon.contains('/')) {
        const QString candidate = QDir(desktopDir).filePath(icon);
        if (exists(candidate)) return candidate;
    }

    QStringList searchRoots = {
        desktopDir,
        "/usr/share/pixmaps",
        "/usr/share/icons/hicolor/64x64/apps",
        "/usr/share/icons/hicolor/48x48/apps",
        "/var/lib/snapd/desktop/icons"
    };
    QStringList extensions = {"", ".png", ".svg", ".xpm"};

    for (const QString &root : searchRoots) {
        for (const QString &ext : extensions) {
            const QString candidate = QDir(root).filePath(icon + ext);
            if (exists(candidate)) return candidate;
        }
    }

    return rawIcon;  // Fallback to theme name; handled by QIcon::fromTheme
}

WorkflowSetupPage::WorkflowSetupPage(QWidget *parent)
    : QWidget(parent), selectedEmoji("🚀")
{
    qDebug() << "[WorkflowSetupPage] Initializing";

    // Broaden icon lookup paths so QIcon::fromTheme can find Snap/Flatpak/pixmaps assets.
    QStringList themePaths = QIcon::themeSearchPaths();
    themePaths << "/usr/share/icons" << "/usr/share/pixmaps" << "/var/lib/snapd/desktop/icons";
    themePaths.removeDuplicates();
    QIcon::setThemeSearchPaths(themePaths);
    if (QIcon::themeName().isEmpty()) {
        QIcon::setThemeName("hicolor");
    }
    // Main layout
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(40, 40, 40, 40);
    rootLayout->setSpacing(20);

    // Create main card frame
    QFrame *mainCard = new QFrame(this);
    mainCard->setStyleSheet(
        "QFrame { "
        "background:#ffffff; "
        "border-radius:20px; "
        "border:1px solid #e0e0e0; "
        "}"
    );

    // Add drop shadow effect
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(20);
    shadow->setYOffset(8);
    shadow->setXOffset(0);
    shadow->setColor(QColor(0, 0, 0, 20));
    mainCard->setGraphicsEffect(shadow);

    // Setup stacked widget inside card
    QVBoxLayout *cardLayout = new QVBoxLayout(mainCard);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

    // Create tab navigation at the top
    QFrame *tabFrame = new QFrame();
    tabFrame->setStyleSheet(
        "QFrame { "
        "background:#ffffff; "
        "border-bottom:1px solid #e5e7eb; "
        "border-radius:20px 20px 0px 0px; "
        "}"
    );
    QHBoxLayout *tabLayout = new QHBoxLayout(tabFrame);
    tabLayout->setContentsMargins(40, 0, 40, 0);
    tabLayout->setSpacing(0);

    QPushButton *appSetupTab = new QPushButton("App Setup");
    appSetupTab->setFixedHeight(50);
    appSetupTab->setStyleSheet(
        "QPushButton { "
        "background:transparent; "
        "border:none; "
        "border-bottom:3px solid #0078d4; "
        "font-size:14px; "
        "font-weight:600; "
        "color:#0078d4; "
        "padding:0px 20px; "
        "}"
        "QPushButton:hover { color:#005a9e; border-bottom:3px solid #005a9e; }"
    );
    appSetupTab->setFlat(true);

    QPushButton *durationSetupTab = new QPushButton("Duration Setup");
    durationSetupTab->setFixedHeight(50);
    durationSetupTab->setStyleSheet(
        "QPushButton { "
        "background:transparent; "
        "border:none; "
        "border-bottom:3px solid transparent; "
        "font-size:14px; "
        "font-weight:600; "
        "color:#9ca3af; "
        "padding:0px 20px; "
        "}"
        "QPushButton:hover { color:#6b7280; }"
    );
    durationSetupTab->setFlat(true);

    tabLayout->addWidget(appSetupTab);
    tabLayout->addWidget(durationSetupTab);
    tabLayout->addStretch();

    cardLayout->addWidget(tabFrame);

    stacked = new QStackedWidget();
    QVBoxLayout *stackedLayout = new QVBoxLayout();
    stackedLayout->setContentsMargins(40, 40, 40, 40);
    stackedLayout->setSpacing(0);
    
    QWidget *stackedContainer = new QWidget();
    stackedContainer->setLayout(stackedLayout);
    stackedLayout->addWidget(stacked);
    
    cardLayout->addWidget(stackedContainer, 1);

    // Setup both pages
    setupAppSetupPage();
    setupDurationSetupPage();

    stacked->setCurrentIndex(0);

    // Tab switching
    connect(appSetupTab, &QPushButton::clicked, this, [this, appSetupTab, durationSetupTab]() {
        stacked->setCurrentIndex(0);
        appSetupTab->setStyleSheet(
            "QPushButton { "
            "background:transparent; "
            "border:none; "
            "border-bottom:3px solid #0078d4; "
            "font-size:14px; "
            "font-weight:600; "
            "color:#0078d4; "
            "padding:0px 20px; "
            "}"
            "QPushButton:hover { color:#005a9e; border-bottom:3px solid #005a9e; }"
        );
        durationSetupTab->setStyleSheet(
            "QPushButton { "
            "background:transparent; "
            "border:none; "
            "border-bottom:3px solid transparent; "
            "font-size:14px; "
            "font-weight:600; "
            "color:#9ca3af; "
            "padding:0px 20px; "
            "}"
            "QPushButton:hover { color:#6b7280; }"
        );
    });

    connect(durationSetupTab, &QPushButton::clicked, this, [this, appSetupTab, durationSetupTab]() {
        stacked->setCurrentIndex(1);
        appSetupTab->setStyleSheet(
            "QPushButton { "
            "background:transparent; "
            "border:none; "
            "border-bottom:3px solid transparent; "
            "font-size:14px; "
            "font-weight:600; "
            "color:#9ca3af; "
            "padding:0px 20px; "
            "}"
            "QPushButton:hover { color:#6b7280; }"
        );
        durationSetupTab->setStyleSheet(
            "QPushButton { "
            "background:transparent; "
            "border:none; "
            "border-bottom:3px solid #0078d4; "
            "font-size:14px; "
            "font-weight:600; "
            "color:#0078d4; "
            "padding:0px 20px; "
            "}"
            "QPushButton:hover { color:#005a9e; border-bottom:3px solid #005a9e; }"
        );
    });

    rootLayout->addWidget(mainCard, 1);

    // Bottom buttons layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);

    QPushButton *backButton = new QPushButton("Back");
    backButton->setFixedHeight(44);
    backButton->setMinimumWidth(100);
    backButton->setStyleSheet(
        "QPushButton { "
        "background:#f2f4f5; "
        "border:1px solid #e0e0e0; "
        "border-radius:8px; "
        "padding:10px 24px; "
        "font-weight:600; "
        "font-size:14px; "
        "color:#333333; "
        "}"
        "QPushButton:hover { background:#e8ecef; }"
    );
    connect(backButton, &QPushButton::clicked, this, &WorkflowSetupPage::onBackClicked);

    buttonLayout->addWidget(backButton);
    buttonLayout->addStretch();

    QPushButton *actionButton = new QPushButton("Next →");
    actionButton->setFixedHeight(44);
    actionButton->setMinimumWidth(140);
    
    // Style variable for next/start buttons (will change based on page)
    auto nextButtonStyle = "QPushButton { "
        "background:#0078d4; "
        "color:white; "
        "border:none; "
        "border-radius:8px; "
        "padding:10px 24px; "
        "font-weight:700; "
        "font-size:14px; "
        "}"
        "QPushButton:hover { background:#005a9e; }";
    
    auto startButtonStyle = "QPushButton { "
        "background:#8B4513; "
        "color:white; "
        "border:none; "
        "border-radius:8px; "
        "padding:10px 24px; "
        "font-weight:700; "
        "font-size:14px; "
        "}"
        "QPushButton:hover { background:#6B3410; }";
    
    actionButton->setStyleSheet(nextButtonStyle);
    
    // Update button text and function based on current page
    connect(appSetupTab, &QPushButton::clicked, this, [actionButton, nextButtonStyle]() {
        actionButton->setText("Next →");
        actionButton->setStyleSheet(nextButtonStyle);
    });
    
    connect(durationSetupTab, &QPushButton::clicked, this, [actionButton, startButtonStyle]() {
        actionButton->setText("Start Tracking");
        actionButton->setStyleSheet(startButtonStyle);
    });
    
    connect(actionButton, &QPushButton::clicked, this, [this, actionButton, durationSetupTab]() {
        if (actionButton->text() == "Next →") {
            // Go to Duration Setup and click the tab to update everything
            stacked->setCurrentIndex(1);
            durationSetupTab->click();  // This will trigger tab styling and button text update
        } else if (actionButton->text() == "Start Tracking") {
            // Start the workflow
            onStartWorkflowClicked();
        }
    });

    buttonLayout->addWidget(actionButton);
    rootLayout->addLayout(buttonLayout);

    // Load Linux apps
    loadLinuxApps();
}

void WorkflowSetupPage::setupAppSetupPage()
{
    QWidget *appPage = new QWidget();
    QVBoxLayout *pageLayout = new QVBoxLayout(appPage);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(20);

    // Title
    QLabel *titleLabel = new QLabel("App Setup");
    titleLabel->setStyleSheet(
        "font-size:24px; "
        "font-weight:600; "
        "color:#1f2937; "
    );
    pageLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel("Select allowed applications for your focused session.\nSearch and whitelist the apps that you want to stay focused on.");
    subtitleLabel->setStyleSheet(
        "font-size:13px; "
        "color:#6b7280; "
        "line-height:1.5; "
    );
    subtitleLabel->setWordWrap(true);
    pageLayout->addWidget(subtitleLabel);

    // Search input
    appSearchInput = new QLineEdit();
    appSearchInput->setPlaceholderText("Search apps to whitelist...");
    appSearchInput->setFixedHeight(44);
    appSearchInput->setStyleSheet(
        "QLineEdit { "
        "background:#ffffff; "
        "border:1px solid #e5e7eb; "
        "border-radius:8px; "
        "padding:10px 14px; "
        "font-size:14px; "
        "color:#1f2937; "
        "}"
        "QLineEdit:focus { "
        "border:1px solid #0078d4; "
        "background:#ffffff; "
        "}"
    );
    connect(appSearchInput, &QLineEdit::textChanged, this, &WorkflowSetupPage::filterApps);
    pageLayout->addWidget(appSearchInput);

    // Two-column layout for suggestions and selected apps
    QHBoxLayout *appsContentLayout = new QHBoxLayout();
    appsContentLayout->setSpacing(20);

    // Left side: Suggestions
    QVBoxLayout *suggestionsContainer = new QVBoxLayout();
    suggestionsContainer->setContentsMargins(0, 0, 0, 0);
    suggestionsContainer->setSpacing(12);

    QLabel *suggestionsLabel = new QLabel("Suggestions");
    suggestionsLabel->setStyleSheet(
        "font-size:12px; "
        "font-weight:600; "
        "color:#9ca3af; "
        "text-transform:uppercase; "
        "letter-spacing:0.5px; "
    );
    suggestionsContainer->addWidget(suggestionsLabel);

    // Suggestions list widget
    suggestionsListWidget = new QListWidget();
    suggestionsListWidget->setStyleSheet(
        "QListWidget { "
        "border:1px solid #e5e7eb; "
        "border-radius:8px; "
        "background:#ffffff; "
        "padding:0px; "
        "}"
        "QListWidget::item { "
        "padding:12px 12px; "
        "border-bottom:1px solid #f3f4f6; "
        "}"
        "QListWidget::item:last { "
        "border-bottom:none; "
        "}"
    );
    suggestionsListWidget->setMinimumHeight(250);
    suggestionsContainer->addWidget(suggestionsListWidget, 1);

    // Right side: Selected Apps
    QVBoxLayout *selectedContainer = new QVBoxLayout();
    selectedContainer->setContentsMargins(0, 0, 0, 0);
    selectedContainer->setSpacing(12);

    QLabel *selectedLabel = new QLabel("Selected Apps");
    selectedLabel->setStyleSheet(
        "font-size:12px; "
        "font-weight:600; "
        "color:#9ca3af; "
        "text-transform:uppercase; "
        "letter-spacing:0.5px; "
    );
    selectedContainer->addWidget(selectedLabel);

    selectedAppsListWidget = new QListWidget();
    selectedAppsListWidget->setStyleSheet(
        "QListWidget { "
        "border:2px solid #e5e7eb; "
        "border-radius:8px; "
        "background:#f9fafb; "
        "padding:0px; "
        "}"
        "QListWidget::item { "
        "padding:12px 12px; "
        "border-bottom:1px solid #f3f4f6; "
        "}"
        "QListWidget::item:last { "
        "border-bottom:none; "
        "}"
    );
    selectedAppsListWidget->setMinimumHeight(250);
    selectedContainer->addWidget(selectedAppsListWidget, 1);

    appsContentLayout->addLayout(suggestionsContainer, 1);
    appsContentLayout->addLayout(selectedContainer, 1);
    pageLayout->addLayout(appsContentLayout, 1);

    // Footer info
    QLabel *infoLabel = new QLabel("💡 FocusPath will monitor your active window. Time spent outside the selected applications will be recorded as distraction time.");
    infoLabel->setStyleSheet(
        "font-size:12px; "
        "color:#6b7280; "
        "background:#f9fafb; "
        "border-left:3px solid #0078d4; "
        "padding:12px; "
        "border-radius:4px; "
    );
    infoLabel->setWordWrap(true);
    pageLayout->addWidget(infoLabel);

    // System status
    QHBoxLayout *statusLayout = new QHBoxLayout();
    QLabel *statusDot = new QLabel("●");
    statusDot->setStyleSheet("color:#10b981; font-size:16px;");
    QLabel *statusLabel = new QLabel("System Ready");
    statusLabel->setStyleSheet("color:#6b7280; font-size:12px;");
    QLabel *versionLabel = new QLabel("v2.4.0");
    versionLabel->setStyleSheet("color:#9ca3af; font-size:12px;");
    
    statusLayout->addWidget(statusDot);
    statusLayout->addWidget(statusLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(versionLabel);
    pageLayout->addLayout(statusLayout);

    stacked->addWidget(appPage);
}

void WorkflowSetupPage::setupDurationSetupPage()
{
    QWidget *durationPage = new QWidget();
    QVBoxLayout *pageLayout = new QVBoxLayout(durationPage);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(24);

    // Title section
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("Duration Setup");
    titleLabel->setStyleSheet(
        "font-size:24px; "
        "font-weight:600; "
        "color:#1f2937; "
    );
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();

    timeLabel = new QLabel("45 min");
    timeLabel->setStyleSheet(
        "font-size:20px; "
        "font-weight:700; "
        "color:#0078d4; "
    );
    titleLayout->addWidget(timeLabel);
    pageLayout->addLayout(titleLayout);

    QLabel *subtitleLabel = new QLabel("Set your session duration and give a name to your workflow for any starting next time.\nOptionally save the setup as a template.");
    subtitleLabel->setStyleSheet(
        "font-size:13px; "
        "color:#6b7280; "
        "line-height:1.5; "
    );
    subtitleLabel->setWordWrap(true);
    pageLayout->addWidget(subtitleLabel);

    // Session Duration section
    QLabel *durationLabel = new QLabel("Session Duration");
    durationLabel->setStyleSheet(
        "font-size:14px; "
        "font-weight:600; "
        "color:#1f2937; "
    );
    pageLayout->addWidget(durationLabel);

    // Slider
    durationSlider = new QSlider(Qt::Horizontal);
    durationSlider->setRange(5, 180);
    durationSlider->setValue(45);
    durationSlider->setFixedHeight(6);
    durationSlider->setStyleSheet(
        "QSlider::groove:horizontal { "
        "border:none; "
        "height:6px; "
        "background:#e5e7eb; "
        "border-radius:3px; "
        "}"
        "QSlider::handle:horizontal { "
        "background:#0078d4; "
        "width:20px; "
        "margin:-7px 0; "
        "border-radius:10px; "
        "}"
        "QSlider::handle:horizontal:hover { "
        "background:#005a9e; "
        "}"
    );
    connect(durationSlider, &QSlider::valueChanged, this, &WorkflowSetupPage::updateDurationLabel);
    pageLayout->addWidget(durationSlider);

    // Slider range labels
    QHBoxLayout *sliderRangeLayout = new QHBoxLayout();
    QLabel *minLabel = new QLabel("5m");
    minLabel->setStyleSheet("font-size:12px; color:#9ca3af;");
    QLabel *maxLabel = new QLabel("3h");
    maxLabel->setStyleSheet("font-size:12px; color:#9ca3af;");
    sliderRangeLayout->addWidget(minLabel);
    sliderRangeLayout->addStretch();
    sliderRangeLayout->addWidget(maxLabel);
    pageLayout->addLayout(sliderRangeLayout);

    // Quick presets
    QLabel *presetsLabel = new QLabel("Quick Presets");
    presetsLabel->setStyleSheet(
        "font-size:14px; "
        "font-weight:600; "
        "color:#1f2937; "
    );
    pageLayout->addSpacing(8);
    pageLayout->addWidget(presetsLabel);

    QHBoxLayout *presetsLayout = new QHBoxLayout();
    presetsLayout->setSpacing(12);

    QPushButton *preset1 = new QPushButton("Pomodoro (25m)");
    preset1->setFixedHeight(40);
    preset1->setStyleSheet(
        "QPushButton { "
        "background:#f3f4f6; "
        "border:1px solid #e5e7eb; "
        "border-radius:8px; "
        "font-weight:600; "
        "font-size:13px; "
        "}"
        "QPushButton:hover { background:#e5e7eb; }"
    );
    connect(preset1, &QPushButton::clicked, [this]() { durationSlider->setValue(25); });
    presetsLayout->addWidget(preset1);

    QPushButton *preset2 = new QPushButton("Deep Work (45m)");
    preset2->setFixedHeight(40);
    preset2->setStyleSheet(
        "QPushButton { "
        "background:#f3f4f6; "
        "border:1px solid #e5e7eb; "
        "border-radius:8px; "
        "font-weight:600; "
        "font-size:13px; "
        "}"
        "QPushButton:hover { background:#e5e7eb; }"
    );
    connect(preset2, &QPushButton::clicked, [this]() { durationSlider->setValue(45); });
    presetsLayout->addWidget(preset2);

    QPushButton *preset3 = new QPushButton("Long Session (60m)");
    preset3->setFixedHeight(40);
    preset3->setStyleSheet(
        "QPushButton { "
        "background:#f3f4f6; "
        "border:1px solid #e5e7eb; "
        "border-radius:8px; "
        "font-weight:600; "
        "font-size:13px; "
        "}"
        "QPushButton:hover { background:#e5e7eb; }"
    );
    connect(preset3, &QPushButton::clicked, [this]() { durationSlider->setValue(60); });
    presetsLayout->addWidget(preset3);
    presetsLayout->addStretch();

    pageLayout->addLayout(presetsLayout);

    // Workflow name section
    QLabel *nameLabel = new QLabel("Workflow Name");
    nameLabel->setStyleSheet(
        "font-size:14px; "
        "font-weight:600; "
        "color:#1f2937; "
    );
    pageLayout->addSpacing(12);
    pageLayout->addWidget(nameLabel);

    workflowNameInput = new QLineEdit();
    workflowNameInput->setPlaceholderText("e.g. Morning Coding Session");
    workflowNameInput->setFixedHeight(40);
    workflowNameInput->setStyleSheet(
        "QLineEdit { "
        "background:#ffffff; "
        "border:1px solid #e5e7eb; "
        "border-radius:8px; "
        "padding:10px 12px; "
        "font-size:14px; "
        "color:#1f2937; "
        "}"
        "QLineEdit:focus { "
        "border:1px solid #0078d4; "
        "background:#ffffff; "
        "}"
    );
    pageLayout->addWidget(workflowNameInput);

    // Save as template
    QCheckBox *templateCheckbox = new QCheckBox("Save as Template");
    templateCheckbox->setStyleSheet(
        "QCheckBox { "
        "font-size:14px; "
        "color:#1f2937; "
        "}"
        "QCheckBox::indicator { "
        "width:18px; "
        "height:18px; "
        "}"
    );
    pageLayout->addWidget(templateCheckbox);

    // Emoji selector section
    QLabel *emojiLabel = new QLabel("Workflow Icon");
    emojiLabel->setStyleSheet(
        "font-size:14px; "
        "font-weight:600; "
        "color:#1f2937; "
    );
    pageLayout->addSpacing(12);
    pageLayout->addWidget(emojiLabel);

    QLabel *emojiSubLabel = new QLabel("Pick an emoji icon for this workflow");
    emojiSubLabel->setStyleSheet(
        "font-size:13px; "
        "color:#6b7280; "
    );
    pageLayout->addWidget(emojiSubLabel);

    QHBoxLayout *emojiLayout = new QHBoxLayout();
    emojiLayout->setSpacing(12);

    // Selected emoji display
    selectedEmojiLabel = new QLabel(selectedEmoji);
    selectedEmojiLabel->setFixedSize(50, 50);
    selectedEmojiLabel->setAlignment(Qt::AlignCenter);
    
    // Set emoji font
    QFont emojiFont;
    emojiFont.setFamilies({"Noto Color Emoji", "Apple Color Emoji", "Segoe UI Emoji", "Sans Serif"});
    emojiFont.setPointSize(28);
    selectedEmojiLabel->setFont(emojiFont);
    
    selectedEmojiLabel->setStyleSheet(
        "QLabel { "
        "background:#f3f4f6; "
        "border-radius:8px; "
        "border:1px solid #e5e7eb; "
        "}"
    );
    emojiLayout->addWidget(selectedEmojiLabel);

    // Emoji selector button
    QPushButton *emojiButton = new QPushButton("Choose Emoji");
    emojiButton->setFixedHeight(40);
    emojiButton->setMinimumWidth(120);
    emojiButton->setStyleSheet(
        "QPushButton { "
        "background:#f3f4f6; "
        "border:1px solid #e5e7eb; "
        "border-radius:8px; "
        "font-weight:600; "
        "font-size:13px; "
        "color:#1f2937; "
        "}"
        "QPushButton:hover { background:#e5e7eb; }"
    );
    connect(emojiButton, &QPushButton::clicked, this, &WorkflowSetupPage::onEmojiButtonClicked);
    emojiLayout->addWidget(emojiButton);
    emojiLayout->addStretch();

    pageLayout->addLayout(emojiLayout);

    pageLayout->addStretch();

    // System status
    QHBoxLayout *statusLayout = new QHBoxLayout();
    QLabel *statusDot = new QLabel("●");
    statusDot->setStyleSheet("color:#10b981; font-size:16px;");
    QLabel *statusLabel = new QLabel("System Ready");
    statusLabel->setStyleSheet("color:#6b7280; font-size:12px;");
    QLabel *versionLabel = new QLabel("v2.4.0");
    versionLabel->setStyleSheet("color:#9ca3af; font-size:12px;");
    
    statusLayout->addWidget(statusDot);
    statusLayout->addWidget(statusLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(versionLabel);
    pageLayout->addLayout(statusLayout);

    stacked->addWidget(durationPage);
}

void WorkflowSetupPage::loadLinuxApps()
{
    qDebug() << "[WorkflowSetupPage] Loading Linux apps";
    appIconMap.clear();

    // Tüm olası uygulama dizinleri (standart + Snap + AppImage)
    QStringList dirs = {
        // Standart Linux dizinleri
        "/usr/share/applications",
        "/usr/local/share/applications",
        QDir::homePath() + "/.local/share/applications",
        
        // Snap uygulamaları için dizinler
        "/var/lib/snapd/desktop/applications",      // En önemli - Snap uygulamaları buraya kaydedilir
        "/usr/share/snap/applications",
        
        // Flatpak uygulamaları için dizin (eğer varsa)
        QDir::homePath() + "/.local/share/flatpak/export/share/applications",
        "/var/lib/flatpak/exports/share/applications"
    };

    QSet<QString> seenApps;  // Duplikat uygulamaları önlemek için

    for (const QString &dirPath : dirs) {
        QDir dir(dirPath);
        if (!dir.exists()) continue;

        qDebug() << "[WorkflowSetupPage] Scanning" << dirPath;

        for (const QString &file : dir.entryList(QStringList() << "*.desktop", QDir::Files)) {
            const QString desktopPath = dir.absoluteFilePath(file);
            QSettings settings(desktopPath, QSettings::IniFormat);
            settings.beginGroup("Desktop Entry");

            if (settings.value("Type") != "Application")
                continue;

            QString name = settings.value("Name").toString();
            const QString iconName = settings.value("Icon").toString();
            if (name.isEmpty())
                continue;

            // Aynı uygulamayı iki kez eklememeyi kontrol et
            if (seenApps.contains(name))
                continue;

            seenApps.insert(name);
            const QString resolvedIcon = resolveIconPath(iconName, dir.absolutePath());
            appIconMap[name] = resolvedIcon;
        }
    }

    // Uygulamaları alfabetik sıraya göre sırala
    QMap<QString, QString> sortedMap;
    for (auto it = appIconMap.begin(); it != appIconMap.end(); ++it) {
        sortedMap.insert(it.key(), it.value());
    }
    appIconMap = sortedMap;

    qDebug() << "[WorkflowSetupPage] Loaded" << appIconMap.size() << "apps";

    // Populate suggestions widget with all apps
    filterApps("");
}

void WorkflowSetupPage::filterApps(const QString &searchText)
{
    qDebug() << "[WorkflowSetupPage] Filtering apps for" << searchText;
    suggestionsListWidget->clear();

    int matchCount = 0;

    for (auto it = appIconMap.begin(); it != appIconMap.end(); ++it) {
        const QString &appName = it.key();
        const QString &iconName = it.value();

        if (!searchText.isEmpty() && !appName.toLower().contains(searchText.toLower())) {
            continue;
        }

        // Create item widget
        QWidget *itemWidget = new QWidget();
        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(0, 0, 0, 0);
        itemLayout->setSpacing(12);

        // App icon
        QIcon appIcon = QFile::exists(iconName)
                            ? QIcon(iconName)
                            : QIcon::fromTheme(iconName);
        if (appIcon.isNull()) {
            appIcon = QIcon(":/pictures/icons8-play-48.png");
        }
        QLabel *iconLabel = new QLabel();
        QPixmap pix = appIcon.pixmap(64, 64).scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        iconLabel->setPixmap(pix);
        iconLabel->setAlignment(Qt::AlignCenter);
        iconLabel->setFixedSize(24, 24);
        itemLayout->addWidget(iconLabel);

        // App name
        QLabel *nameLabel = new QLabel(appName);
        nameLabel->setStyleSheet(
            "font-size:13px; "
            "color:#1f2937; "
            "font-weight:500; "
        );
        itemLayout->addWidget(nameLabel, 1);

        // Add button
        QPushButton *addButton = new QPushButton("+ Add");
        addButton->setFixedSize(70, 28);
        addButton->setStyleSheet(
            "QPushButton { "
            "background:#f3f4f6; "
            "border:1px solid #d1d5db; "
            "border-radius:6px; "
            "font-size:12px; "
            "font-weight:600; "
            "color:#374151; "
            "}"
            "QPushButton:hover { background:#e5e7eb; }"
        );
        connect(addButton, &QPushButton::clicked, this, [this, appName]() {
            onAppAddClicked(appName);
        });
        itemLayout->addWidget(addButton);

        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(QSize(0, 56));
        suggestionsListWidget->addItem(item);
        suggestionsListWidget->setItemWidget(item, itemWidget);

        ++matchCount;
    }

    qDebug() << "[WorkflowSetupPage] Filter matched" << matchCount << "apps";
}

void WorkflowSetupPage::onAppAddClicked(const QString &appName)
{
    if (!selectedApps.contains(appName)) {
        selectedApps.append(appName);
        qDebug() << "Added app:" << appName;
        updateSelectedAppsList();
    }
}

void WorkflowSetupPage::onAppRemoveClicked(const QString &appName)
{
    qDebug() << "[WorkflowSetupPage] Removing app" << appName;
    selectedApps.removeAll(appName);
    updateSelectedAppsList();
}

void WorkflowSetupPage::updateSelectedAppsList()
{
    selectedAppsListWidget->clear();

    if (selectedApps.isEmpty()) {
        QLabel *emptyLabel = new QLabel("No apps selected yet");
        emptyLabel->setStyleSheet(
            "color:#9ca3af; "
            "font-style:italic; "
        );
        emptyLabel->setAlignment(Qt::AlignCenter);
        
        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(QSize(0, 80));
        selectedAppsListWidget->addItem(item);
        selectedAppsListWidget->setItemWidget(item, emptyLabel);
        return;
    }

    for (const QString &appName : selectedApps) {
        const QString &iconName = appIconMap.value(appName);

        // Create item widget
        QWidget *itemWidget = new QWidget();
        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(0, 0, 0, 0);
        itemLayout->setSpacing(12);

        // App icon
        QIcon appIcon = QFile::exists(iconName)
                            ? QIcon(iconName)
                            : QIcon::fromTheme(iconName);
        if (appIcon.isNull()) {
            appIcon = QIcon(":/pictures/icons8-play-48.png");
        }
        QLabel *iconLabel = new QLabel();
        QPixmap pix = appIcon.pixmap(64, 64).scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        iconLabel->setPixmap(pix);
        iconLabel->setAlignment(Qt::AlignCenter);
        iconLabel->setFixedSize(24, 24);
        itemLayout->addWidget(iconLabel);

        // App name
        QLabel *nameLabel = new QLabel(appName);
        nameLabel->setStyleSheet(
            "font-size:13px; "
            "color:#1f2937; "
            "font-weight:500; "
        );
        itemLayout->addWidget(nameLabel, 1);

        // Remove button
        QPushButton *removeButton = new QPushButton("✕ Remove");
        removeButton->setFixedSize(80, 28);
        removeButton->setStyleSheet(
            "QPushButton { "
            "background:#fee2e2; "
            "border:1px solid #fca5a5; "
            "border-radius:6px; "
            "font-size:12px; "
            "font-weight:600; "
            "color:#dc2626; "
            "}"
            "QPushButton:hover { background:#fecaca; }"
        );
        connect(removeButton, &QPushButton::clicked, this, [this, appName]() {
            onAppRemoveClicked(appName);
        });
        itemLayout->addWidget(removeButton);

        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(QSize(0, 56));
        selectedAppsListWidget->addItem(item);
        selectedAppsListWidget->setItemWidget(item, itemWidget);
    }
}

void WorkflowSetupPage::updateDurationLabel(int totalMinutes)
{
    int hours = totalMinutes / 60;
    int minutes = totalMinutes % 60;

    QString timeText;
    if (hours > 0 && minutes > 0) {
        timeText = QString("%1h %2m").arg(hours).arg(minutes);
    } else if (hours > 0) {
        timeText = QString("%1h").arg(hours);
    } else {
        timeText = QString("%1m").arg(minutes);
    }

    timeLabel->setText(timeText);
    qDebug() << "[WorkflowSetupPage] Duration updated to" << timeText;
}

void WorkflowSetupPage::onEmojiButtonClicked()
{
    EmojiSelector selector(selectedEmoji, this);
    if (selector.exec() == QDialog::Accepted) {
        selectedEmoji = selector.getSelectedEmoji();
        updateEmojiDisplay();
        qDebug() << "[WorkflowSetupPage] Emoji selected" << selectedEmoji;
    }
}

void WorkflowSetupPage::updateEmojiDisplay()
{
    if (selectedEmojiLabel) {
        selectedEmojiLabel->setText(selectedEmoji);
        
        // Apply proper emoji font (same as EmojiSelector)
        QFont emojiFont;
        emojiFont.setFamilies({"Noto Color Emoji", "Apple Color Emoji", "Segoe UI Emoji", "Sans Serif"});
        emojiFont.setPointSize(28);
        selectedEmojiLabel->setFont(emojiFont);
        qDebug() << "[WorkflowSetupPage] Emoji display updated" << selectedEmoji;
    }
}

void WorkflowSetupPage::createWorkflow(int totalMinutes, bool isFavorite)
{
    qDebug() << "[WorkflowSetupPage] Creating workflow" << totalMinutes << "minutes" << "favorite" << isFavorite;
    if (selectedApps.isEmpty()) {
        QMessageBox::warning(
            this,
            "Workflow Creation Failed",
            "You must add at least one application to your workflow."
        );
        return;
    }

    QString workflowName = workflowNameInput->text().trimmed();
    
    Workflow w = workflowName.isEmpty()
                     ? Workflow(totalMinutes, isFavorite)
                     : Workflow(workflowName.toStdString(), totalMinutes, isFavorite);

    for (const QString &appName : selectedApps) {
        w.addApps(appName.toStdString());
    }

    qDebug() << "[WorkflowSetupPage] Selected apps" << selectedApps;

    w.setIcon(selectedEmoji.toStdString());

    // Create workflows directory
    QString workflowsDir = QCoreApplication::applicationDirPath() + "/workflows";
    QDir dir;
    if (!dir.exists(workflowsDir)) {
        dir.mkpath(workflowsDir);
        qDebug() << "Created workflows directory:" << workflowsDir;
    }

    QString filePath = QString("%1/%2.json").arg(workflowsDir, QString::fromStdString(w.getDate()));
    Workflow::saveWorkflowToFile(w, filePath);

    qDebug() << "[WorkflowSetupPage] Workflow saved to" << filePath;
    
    emit startWorkflow(w);
    w.print();
}

void WorkflowSetupPage::onBackClicked()
{
    qDebug() << "[WorkflowSetupPage] Back requested";
    emit backToDashboard();
}

void WorkflowSetupPage::onStartWorkflowClicked()
{
    qDebug() << "[WorkflowSetupPage] Start workflow clicked";
    createWorkflow(durationSlider->value(), true);
}

void WorkflowSetupPage::loadTemplateWorkflow(const Workflow &workflow)
{
    qDebug() << "[WorkflowSetupPage] Loading template" << QString::fromStdString(workflow.getName());
    workflowNameInput->setText(QString::fromStdString(workflow.getName()));
    durationSlider->setValue(static_cast<int>(workflow.getDuration()));
    updateDurationLabel(durationSlider->value());

    selectedApps.clear();
    for (const auto &app : workflow.getApps()) {
        selectedApps.append(QString::fromStdString(app));
    }

    selectedEmoji = QString::fromStdString(workflow.getIcon());
    updateEmojiDisplay();
}



















