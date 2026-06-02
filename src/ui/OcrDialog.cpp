#include "OcrDialog.h"
#include "core/OcrEngine.h"
#include "core/TranslationManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGuiApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>

OcrDialog::OcrDialog(const QPixmap &pixmap, QWidget *parent)
    : QDialog(parent), m_pixmap(pixmap)
{
    setWindowTitle(TranslationManager::ocrTitle());
    setWindowIcon(QIcon(":/icons/pen.svg"));
    resize(520, 420);

    m_engine = new OcrEngine(this);
    connect(m_engine, &OcrEngine::textReady, this, &OcrDialog::onTextReady);
    connect(m_engine, &OcrEngine::failed, this, &OcrDialog::onOcrFailed);
    m_languageTag = QSettings("EShot", "EShot").value("ocrLanguage", m_languageTag).toString();

    auto *layout = new QVBoxLayout(this);

    auto *langRow = new QHBoxLayout();
    QLabel *langLabel = new QLabel(QString("%1:").arg(TranslationManager::language()), this);
    m_langCombo = new QComboBox(this);
    m_langCombo->addItem("English", "en-US");
    m_langCombo->addItem(QString::fromUtf8("T\303\274rk\303\247e"), "tr-TR");
    m_langCombo->addItem(QString::fromUtf8("\320\240\321\203\321\201\321\201\320\272\320\270\320\271"), "ru-RU");
    m_langCombo->addItem("Deutsch", "de-DE");
    m_langCombo->addItem(QString::fromUtf8("Fran\303\247ais"), "fr-FR");
    m_langCombo->addItem(QString::fromUtf8("Espa\303\261ol"), "es-ES");
    m_langCombo->addItem("Italiano", "it-IT");
    langRow->addWidget(langLabel);
    langRow->addWidget(m_langCombo);
    langRow->addStretch();
    layout->addLayout(langRow);

    int idx = m_langCombo->findData(m_languageTag);
    if (idx >= 0) m_langCombo->setCurrentIndex(idx);
    connect(m_langCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &OcrDialog::onLanguageChanged);

    m_statusLabel = new QLabel(TranslationManager::ocrProcessing(), this);
    layout->addWidget(m_statusLabel);

    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setPlaceholderText(TranslationManager::ocrEmpty());
    layout->addWidget(m_textEdit, 1);

    auto *btnRow = new QHBoxLayout();
    m_copyBtn = new QPushButton(TranslationManager::ocrCopy(), this);
    m_copyBtn->setEnabled(false);
    m_retryBtn = new QPushButton(TranslationManager::ocrRetry(), this);
    m_retryBtn->setEnabled(false);
    m_closeBtn = new QPushButton(TranslationManager::ocrClose(), this);

    btnRow->addWidget(m_copyBtn);
    btnRow->addWidget(m_retryBtn);
    btnRow->addStretch();
    btnRow->addWidget(m_closeBtn);
    layout->addLayout(btnRow);

    connect(m_copyBtn, &QPushButton::clicked, this, &OcrDialog::onCopyClicked);
    connect(m_retryBtn, &QPushButton::clicked, this, &OcrDialog::onRetryClicked);
    connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    setBusy(true);
    m_engine->recognize(m_pixmap, m_languageTag);
}

OcrDialog::~OcrDialog() = default;

void OcrDialog::setLanguageTag(const QString &tag)
{
    if (tag.isEmpty()) return;
    m_languageTag = tag;
    if (m_langCombo) {
        const int idx = m_langCombo->findData(tag);
        if (idx >= 0 && idx != m_langCombo->currentIndex())
            m_langCombo->setCurrentIndex(idx);
    }
}

void OcrDialog::translateUi()
{
    setWindowTitle(TranslationManager::ocrTitle());
    m_copyBtn->setText(TranslationManager::ocrCopy());
    m_closeBtn->setText(TranslationManager::ocrClose());
}

void OcrDialog::setBusy(bool busy)
{
    m_retryBtn->setEnabled(!busy);
    m_langCombo->setEnabled(!busy);
    if (busy) {
        m_statusLabel->setText(TranslationManager::ocrProcessing());
    }
}

void OcrDialog::runOcr()
{
    setBusy(true);
    m_textEdit->clear();
    m_copyBtn->setEnabled(false);
    m_engine->recognize(m_pixmap, m_languageTag);
}

void OcrDialog::onLanguageChanged(int index)
{
    Q_UNUSED(index)
    QString tag = m_langCombo->currentData().toString();
    if (!tag.isEmpty() && tag != m_languageTag) {
        m_languageTag = tag;
        QSettings("EShot", "EShot").setValue("ocrLanguage", tag);
        if (!m_textEdit->toPlainText().isEmpty()) {
            runOcr();
        }
    }
}

void OcrDialog::onTextReady(const QString &text)
{
    setBusy(false);
    if (text.trimmed().isEmpty()) {
        m_statusLabel->setText(TranslationManager::ocrEmpty());
        m_textEdit->clear();
        m_copyBtn->setEnabled(false);
    } else {
        m_statusLabel->setText(TranslationManager::ocrTitle());
        m_textEdit->setPlainText(text);
        m_copyBtn->setEnabled(true);
    }
}

void OcrDialog::onOcrFailed(const QString &reason)
{
    setBusy(false);
    m_statusLabel->setText(TranslationManager::ocrFailed() + QStringLiteral(" - ") + reason);
    m_textEdit->clear();
    m_copyBtn->setEnabled(false);
    m_retryBtn->setEnabled(true);
    qWarning() << "[EShot] OCR failed:" << reason;
}

void OcrDialog::onCopyClicked()
{
    QGuiApplication::clipboard()->setText(m_textEdit->toPlainText());
    m_statusLabel->setText(TranslationManager::ocrCopied());
}

void OcrDialog::onRetryClicked()
{
    runOcr();
}
