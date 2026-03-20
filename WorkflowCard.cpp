#include "WorkflowCard.h"
#include <QGraphicsDropShadowEffect>
#include <QPixmap>
#include <QMenu>

WorkflowCard::WorkflowCard(const Workflow &wf, QWidget *parent)
    : QFrame(parent), workflow(wf)
{
    workflowName = QString::fromStdString(wf.getName());
    duration = static_cast<int>(wf.getDuration());
    
    for (const auto &app : wf.getApps()) {
        apps.append(QString::fromStdString(app));
    }

    setupUI();
    setData();
}

void WorkflowCard::setupUI() {
    this->setObjectName("WorkflowCard");
    this->setFixedSize(300, 240);
    this->setStyleSheet(
        "#WorkflowCard { "
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #FFFFFF, stop:1 #F8FBFD); "
        "  border-radius: 16px; "
        "  border: 2px solid #E8F0F7; "
        "}"
    );

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setXOffset(0);
    shadow->setYOffset(4);
    shadow->setColor(QColor(0, 0, 0, 15));
    this->setGraphicsEffect(shadow);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(12);

    // Top row: Icon and Menu button
    QHBoxLayout *topLayout = new QHBoxLayout();
    
    iconCircle = new QFrame();
    iconCircle->setFixedSize(48, 48);
    
    iconLabel = new QLabel(iconCircle);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("border: none; background: transparent;");
    
    QVBoxLayout *iconInnerLayout = new QVBoxLayout(iconCircle);
    iconInnerLayout->setContentsMargins(0, 0, 0, 0);
    iconInnerLayout->addWidget(iconLabel);

    menuButton = new QPushButton("⋮");
    menuButton->setFixedSize(32, 32);
    menuButton->setStyleSheet(
        "QPushButton { "
        "  background: transparent; "
        "  border: none; "
        "  font-size: 20px; "
        "  color: #999; "
        "} "
        "QPushButton::menu-indicator { image: none; width: 0px; height: 0px; }"
        "QPushButton:hover { "
        "  background-color: #F5F5F5; "
        "  border-radius: 16px; "
        "}"
    );
    menuButton->setCursor(Qt::PointingHandCursor);

    QMenu *menu = new QMenu(this);
    QAction *deleteAction = new QAction("Delete", menu);
    menu->setStyleSheet(
        "QMenu { background:white; color:black; border:1px solid #E0E0E0; }"
        "QMenu::item { padding:8px 16px; }"
        "QMenu::item:selected { background:#F2F4F5; color:black; }"
    );
    menu->addAction(deleteAction);
    menuButton->setMenu(menu);

    connect(deleteAction, &QAction::triggered, this, [this]() {
        emit deleteClicked(workflow);
    });

    topLayout->addWidget(iconCircle);
    topLayout->addStretch();
    topLayout->addWidget(menuButton);

    // Workflow name
    nameLabel = new QLabel();
    nameLabel->setStyleSheet(
        "font-size: 18px; "
        "font-weight: 600; "
        "color: #212529; "
        "background: transparent;"
    );
    nameLabel->setWordWrap(false);

    // Duration
    durationLabel = new QLabel();
    durationLabel->setStyleSheet(
        "font-size: 13px; "
        "color: #6C757D; "
        "background: transparent;"
    );

    // Included apps label
    QLabel *includedLabel = new QLabel("INCLUDED APPS");
    includedLabel->setStyleSheet(
        "font-size: 10px; "
        "font-weight: 600; "
        "color: #999; "
        "background: transparent; "
        "letter-spacing: 0.5px;"
    );

    // Apps list
    appsLabel = new QLabel();
    appsLabel->setStyleSheet(
        "font-size: 12px; "
        "color: #495057; "
        "background: transparent;"
    );
    appsLabel->setWordWrap(false);

    // Start button
    startButton = new QPushButton("▶ Start Session");
    startButton->setFixedHeight(40);
    startButton->setCursor(Qt::PointingHandCursor);
    startButton->setStyleSheet(
        "QPushButton { "
        "  background-color: #E8F4F8; "
        "  color: #0078D4; "
        "  border: none; "
        "  border-radius: 8px; "
        "  font-size: 14px; "
        "  font-weight: 600; "
        "} "
        "QPushButton:hover { "
        "  background-color: #0078D4; "
        "  color: white; "
        "}"
    );

    connect(startButton, &QPushButton::clicked, this, [this]() {
        emit startSessionClicked(workflow);
    });

    mainLayout->addLayout(topLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(durationLabel);
    mainLayout->addSpacing(8);
    mainLayout->addWidget(includedLabel);
    mainLayout->addWidget(appsLabel);
    mainLayout->addStretch();
    mainLayout->addWidget(startButton);
}

void WorkflowCard::setData() {
    nameLabel->setText(workflowName);
    durationLabel->setText(QString("⏱ %1 min").arg(duration));

    // Display apps as icons (simplified - showing first 3 app names)
    QString appsText;
    int count = qMin(static_cast<int>(apps.size()), 3);
    qDebug() << "WorkflowCard::setData() - apps.size()=" << apps.size() << ", count=" << count << ", workflowName=" << workflowName;
    
    for (int i = 0; i < count; ++i) {
        if (i > 0) appsText += "  ";
        // Simple representation - you can add actual app icons later
        QString appName = apps.at(i);  // Use .at() for bounds checking
        if (appName.length() > 15) {
            appName = appName.left(12) + "...";
        }
        appsText += "• " + appName;
    }
    if (apps.size() > 3) {
        appsText += QString("  +%1 more").arg(apps.size() - 3);
    }
    appsLabel->setText(appsText);

    // Set icon and color based on workflow name
    QColor bgColor = getColorForWorkflow(workflowName);
    iconCircle->setStyleSheet(
        QString("background-color: %1; border-radius: 14px;")
        .arg(bgColor.name())
    );

    const QString iconFromWorkflow = QString::fromStdString(workflow.getIcon());
    if (!iconFromWorkflow.isEmpty()) {
        iconLabel->setText(iconFromWorkflow);
        iconLabel->setStyleSheet(
            "border: none; "
            "background: transparent; "
            "font-size: 20px; "
            "font-weight: bold; "
            "color: white;"
        );
    } else {
        QString iconPath = getIconPathForWorkflow(workflowName);
        QPixmap pix(iconPath);
        if (!pix.isNull()) {
            iconLabel->setPixmap(pix.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            // Default icon - just show first letter
            iconLabel->setText(workflowName.left(1).toUpper());
            iconLabel->setStyleSheet(
                "border: none; "
                "background: transparent; "
                "font-size: 20px; "
                "font-weight: bold; "
                "color: white;"
            );
        }
    }
}

QString WorkflowCard::getIconPathForWorkflow(const QString &name) {
    QString lowerName = name.toLower();
    
    if (lowerName.contains("code") || lowerName.contains("programming")) {
        return ":/pictures/icons8-play-48.png";
    } else if (lowerName.contains("design")) {
        return ":/pictures/lace.png";
    } else if (lowerName.contains("email") || lowerName.contains("mail")) {
        return ":/pictures/history.png";
    }
    
    return "";  // Empty means use text fallback
}

QColor WorkflowCard::getColorForWorkflow(const QString &name) {
    QString lowerName = name.toLower();
    
    if (lowerName.contains("code") || lowerName.contains("program")) {
        return QColor(50, 50, 50);  // Dark for coding
    } else if (lowerName.contains("design")) {
        return QColor(255, 87, 34);  // Orange for design
    } else if (lowerName.contains("read")) {
        return QColor(255, 193, 7);  // Yellow for reading
    } else if (lowerName.contains("email") || lowerName.contains("mail")) {
        return QColor(66, 133, 244);  // Blue for email
    }
    
    // Default colors based on hash of name - use simple modulo without QList
    uint hash = qHash(name);
    
    // Define colors as array instead of QList
    const QColor defaultColors[] = {
        QColor(102, 187, 106),  // Green
        QColor(66, 133, 244),   // Blue
        QColor(171, 71, 188),   // Purple
        QColor(255, 112, 67),   // Orange-Red
        QColor(38, 166, 154)    // Teal
    };
    
    int colorCount = 5;
    return defaultColors[hash % colorCount];
}

void WorkflowCard::enterEvent(QEvent *event) {
    this->setStyleSheet(
        "#WorkflowCard { "
        "  background-color: #FFFFFF; "
        "  border-radius: 16px; "
        "  border: 2px solid #0078D4; "
        "}"
    );
    QFrame::enterEvent(event);
}

void WorkflowCard::leaveEvent(QEvent *event) {
    this->setStyleSheet(
        "#WorkflowCard { "
        "  background-color: #FFFFFF; "
        "  border-radius: 16px; "
        "  border: 1px solid #E8E8E8; "
        "}"
    );
    QFrame::leaveEvent(event);
}
