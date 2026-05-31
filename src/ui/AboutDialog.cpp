#include "AboutDialog.h"
#include "../core/TranslationManager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QApplication>

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(TranslationManager::aboutTitle());
    setWindowIcon(QIcon(":/icons/pen.svg"));
    setFixedSize(300, 250);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
}

AboutDialog::~AboutDialog() {}

void AboutDialog::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(20, 20, 20, 20);

    QLabel *iconLabel = new QLabel();
    QPixmap icon(":/icons/pen.svg");
    if (!icon.isNull()) {
        iconLabel->setPixmap(icon.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(iconLabel);

    QLabel *nameLabel = new QLabel("EShot");
    QFont nameFont = nameLabel->font();
    nameFont.setPointSize(18);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);
    nameLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(nameLabel);

    QLabel *verLabel = new QLabel(QString("Version %1").arg(QApplication::applicationVersion()));
    verLabel->setAlignment(Qt::AlignCenter);
    verLabel->setStyleSheet("color: #888; font-size: 11px;");
    layout->addWidget(verLabel);

    QLabel *descLabel = new QLabel(
        TranslationManager::aboutDesc() + "<br>"
        "<a href='https://github.com/Benoks/EShot' style='color: #4285F4; text-decoration: none;'>GitHub</a>"
    );
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setOpenExternalLinks(true);
    descLabel->setStyleSheet("color: #aaa; margin-top: 5px;");
    layout->addWidget(descLabel);

    layout->addStretch();

    QPushButton *closeBtn = new QPushButton(TranslationManager::cancel());
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #333;
            color: white;
            border: 1px solid #555;
            border-radius: 4px;
            padding: 6px 12px;
        }
        QPushButton:hover {
            background-color: #444;
            border-color: #666;
        }
    )");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(closeBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
}
