#include "SettingsDialog.h"
#include "../core/HotkeyManager.h"
#include "../core/TranslationManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QGroupBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QDateTime>
#include <QListWidgetItem>
#include <QKeySequenceEdit>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(TranslationManager::settingsTitle());
    setMinimumSize(560, 580);
    setMaximumSize(750, 720);

    m_settings = new QSettings("EShot", "EShot", this);
    setupUI();
    loadSettings();
}

SettingsDialog::~SettingsDialog() {}

QString SettingsDialog::resolvePatternPreview(const QString &pattern) const
{
    QDateTime now = QDateTime::currentDateTime();
    QString r = pattern;
    r.replace("%Y", now.toString("yyyy"));
    r.replace("%y", now.toString("yy"));
    r.replace("%M", now.toString("MM"));
    r.replace("%D", now.toString("dd"));
    r.replace("%h", now.toString("HH"));
    r.replace("%m", now.toString("mm"));
    r.replace("%s", now.toString("ss"));
    r.replace("%T", "WindowTitle");
    return r;
}

bool SettingsDialog::keySequenceToWin32(const QKeySequence &seq, UINT &modifiers, UINT &vkey)
{
#ifdef Q_OS_WIN
    if (seq.isEmpty()) return false;

    QKeyCombination combo = seq[0];
    Qt::KeyboardModifiers qtMod = combo.keyboardModifiers();
    Qt::Key qtKey = combo.key();

    modifiers = 0;
    if (qtMod & Qt::ShiftModifier)   modifiers |= MOD_SHIFT;
    if (qtMod & Qt::ControlModifier) modifiers |= MOD_CONTROL;
    if (qtMod & Qt::AltModifier)     modifiers |= MOD_ALT;
    if (qtMod & Qt::MetaModifier)    modifiers |= MOD_WIN;

    struct { Qt::Key qt; UINT win; } mapping[] = {
        { Qt::Key_Print,      VK_SNAPSHOT },
        { Qt::Key_F1,         VK_F1 }, { Qt::Key_F2,  VK_F2  }, { Qt::Key_F3,  VK_F3  },
        { Qt::Key_F4,         VK_F4 }, { Qt::Key_F5,  VK_F5  }, { Qt::Key_F6,  VK_F6  },
        { Qt::Key_F7,         VK_F7 }, { Qt::Key_F8,  VK_F8  }, { Qt::Key_F9,  VK_F9  },
        { Qt::Key_F10,        VK_F10}, { Qt::Key_F11, VK_F11 }, { Qt::Key_F12, VK_F12 },
        { Qt::Key_Home,       VK_HOME },  { Qt::Key_End,    VK_END    },
        { Qt::Key_PageUp,     VK_PRIOR }, { Qt::Key_PageDown,VK_NEXT  },
        { Qt::Key_Insert,     VK_INSERT },{ Qt::Key_Delete,  VK_DELETE },
        { Qt::Key_Left,       VK_LEFT },  { Qt::Key_Right,   VK_RIGHT },
        { Qt::Key_Up,         VK_UP   },  { Qt::Key_Down,    VK_DOWN  },
        { Qt::Key_Space,      VK_SPACE }, { Qt::Key_Return,  VK_RETURN},
        { Qt::Key_Escape,     VK_ESCAPE },{ Qt::Key_Tab,     VK_TAB   },
        { Qt::Key_Backspace,  VK_BACK  },
    };
    for (auto &m : mapping) {
        if (qtKey == m.qt) { vkey = m.win; return true; }
    }

    if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z) {
        vkey = 'A' + (qtKey - Qt::Key_A);
        return true;
    }
    if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9) {
        vkey = '0' + (qtKey - Qt::Key_0);
        return true;
    }
    return false;
#else
    Q_UNUSED(seq); Q_UNUSED(modifiers); Q_UNUSED(vkey);
    return false;
#endif
}

static QKeySequence win32ToKeySequence(UINT modifiers, UINT vkey)
{
    Qt::KeyboardModifiers qtMod;
#ifdef Q_OS_WIN
    if (modifiers & MOD_SHIFT)   qtMod |= Qt::ShiftModifier;
    if (modifiers & MOD_CONTROL) qtMod |= Qt::ControlModifier;
    if (modifiers & MOD_ALT)     qtMod |= Qt::AltModifier;
    if (modifiers & MOD_WIN)     qtMod |= Qt::MetaModifier;

    struct { UINT win; Qt::Key qt; } mapping[] = {
        { VK_SNAPSHOT, Qt::Key_Print },
        { VK_F1,  Qt::Key_F1  }, { VK_F2,  Qt::Key_F2  }, { VK_F3,  Qt::Key_F3  },
        { VK_F4,  Qt::Key_F4  }, { VK_F5,  Qt::Key_F5  }, { VK_F6,  Qt::Key_F6  },
        { VK_F7,  Qt::Key_F7  }, { VK_F8,  Qt::Key_F8  }, { VK_F9,  Qt::Key_F9  },
        { VK_F10, Qt::Key_F10 }, { VK_F11, Qt::Key_F11 }, { VK_F12, Qt::Key_F12 },
        { VK_HOME,   Qt::Key_Home   }, { VK_END,    Qt::Key_End      },
        { VK_PRIOR,  Qt::Key_PageUp }, { VK_NEXT,   Qt::Key_PageDown  },
        { VK_INSERT, Qt::Key_Insert }, { VK_DELETE, Qt::Key_Delete    },
        { VK_LEFT,   Qt::Key_Left   }, { VK_RIGHT,  Qt::Key_Right     },
        { VK_UP,     Qt::Key_Up     }, { VK_DOWN,   Qt::Key_Down      },
        { VK_SPACE,  Qt::Key_Space  }, { VK_RETURN, Qt::Key_Return    },
        { VK_ESCAPE, Qt::Key_Escape }, { VK_TAB,    Qt::Key_Tab       },
        { VK_BACK,   Qt::Key_Backspace },
    };
    for (auto &m : mapping) {
        if (vkey == m.win) return QKeySequence(QKeyCombination(qtMod, m.qt));
    }
    if (vkey >= 'A' && vkey <= 'Z') {
        return QKeySequence(QKeyCombination(qtMod, static_cast<Qt::Key>(Qt::Key_A + (vkey - 'A'))));
    }
    if (vkey >= '0' && vkey <= '9') {
        return QKeySequence(QKeyCombination(qtMod, static_cast<Qt::Key>(Qt::Key_0 + (vkey - '0'))));
    }
#else
    Q_UNUSED(modifiers); Q_UNUSED(vkey);
#endif
    return QKeySequence(Qt::Key_Print);
}

void SettingsDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);

    QLabel *titleLabel = new QLabel(TranslationManager::settingsTitle());
    QFont tf = titleLabel->font(); tf.setPointSize(16); tf.setBold(true);
    titleLabel->setFont(tf);
    mainLayout->addWidget(titleLabel);

    QTabWidget *tabs = new QTabWidget(this);
    tabs->addTab(createGeneralTab(),     TranslationManager::tabGeneral());
    tabs->addTab(createCaptureTab(),     TranslationManager::tabCapture());
    tabs->addTab(createAppearanceTab(),  TranslationManager::tabAppearance());
    tabs->addTab(createInterfaceTab(),   TranslationManager::tabInterface());
    tabs->addTab(createHotkeyTab(),      TranslationManager::tabHotkey());
    mainLayout->addWidget(tabs);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    QPushButton *resetBtn = new QPushButton(TranslationManager::reset());
    resetBtn->setStyleSheet("color: #ff6b6b;");
    connect(resetBtn, &QPushButton::clicked, this, &SettingsDialog::onReset);
    btnLayout->addWidget(resetBtn);

    QPushButton *cancelBtn = new QPushButton(TranslationManager::cancel());
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(cancelBtn);

    QPushButton *saveBtn = new QPushButton(TranslationManager::save());
    saveBtn->setDefault(true);
    saveBtn->setStyleSheet(R"(
        QPushButton { background-color: #0078D4; color: white; border: none;
                      padding: 8px 24px; border-radius: 4px; font-weight: bold; }
        QPushButton:hover { background-color: #1a8cff; }
    )");
    connect(saveBtn, &QPushButton::clicked, this, &SettingsDialog::onSave);
    btnLayout->addWidget(saveBtn);

    mainLayout->addLayout(btnLayout);
}

QWidget* SettingsDialog::createGeneralTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);

    // Dil seçimi
    QGroupBox *langGroup = new QGroupBox(TranslationManager::language());
    QHBoxLayout *langLayout = new QHBoxLayout(langGroup);
    m_langCombo = new QComboBox();
    m_langCombo->addItem(TranslationManager::langTurkish(), "tr");
    m_langCombo->addItem(TranslationManager::langEnglish(), "en");
    langLayout->addWidget(m_langCombo);
    langLayout->addStretch();
    layout->addWidget(langGroup);

    // Kayıt dizini
    QGroupBox *pathGroup = new QGroupBox(TranslationManager::saveDir());
    QHBoxLayout *pathLayout = new QHBoxLayout(pathGroup);
    m_savePathEdit = new QLineEdit();
    m_savePathEdit->setPlaceholderText(TranslationManager::saveDirDesc());
    QPushButton *browseBtn = new QPushButton(TranslationManager::browse());
    connect(browseBtn, &QPushButton::clicked, this, &SettingsDialog::onBrowse);
    pathLayout->addWidget(m_savePathEdit);
    pathLayout->addWidget(browseBtn);
    layout->addWidget(pathGroup);

    // Dosya adı şablonu
    QGroupBox *fnGroup = new QGroupBox(TranslationManager::filenamePattern());
    QVBoxLayout *fnLayout = new QVBoxLayout(fnGroup);
    m_filenamePatternEdit = new QLineEdit();
    m_filenamePatternEdit->setPlaceholderText("Screenshot_%Y-%M-%D_%h-%m-%s");
    connect(m_filenamePatternEdit, &QLineEdit::textChanged, this, &SettingsDialog::onFilenamePatternChanged);
    fnLayout->addWidget(m_filenamePatternEdit);
    m_patternPreviewLabel = new QLabel();
    m_patternPreviewLabel->setStyleSheet("color: #888; font-style: italic;");
    fnLayout->addWidget(m_patternPreviewLabel);
    QLabel *helpLabel = new QLabel(TranslationManager::patternVars());
    helpLabel->setWordWrap(true);
    helpLabel->setStyleSheet("color: #999; font-size: 11px;");
    fnLayout->addWidget(helpLabel);
    layout->addWidget(fnGroup);

    // Genel seçenekler
    QGroupBox *genGroup = new QGroupBox(TranslationManager::generalOptions());
    QVBoxLayout *genLayout = new QVBoxLayout(genGroup);
    m_autoStartCheck         = new QCheckBox(TranslationManager::autoStart());
    m_showNotificationsCheck = new QCheckBox(TranslationManager::showNotifications());
    m_playSoundCheck         = new QCheckBox(TranslationManager::playSound());
    m_copyPathAfterSaveCheck = new QCheckBox(TranslationManager::copyPathAfterSave());
    genLayout->addWidget(m_autoStartCheck);
    genLayout->addWidget(m_showNotificationsCheck);
    genLayout->addWidget(m_playSoundCheck);
    genLayout->addWidget(m_copyPathAfterSaveCheck);
    layout->addWidget(genGroup);

    layout->addStretch();
    return tab;
}

QWidget* SettingsDialog::createCaptureTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QGroupBox *fmtGroup = new QGroupBox(TranslationManager::fileFormat());
    QFormLayout *fmtLayout = new QFormLayout(fmtGroup);
    m_formatCombo = new QComboBox();
    m_formatCombo->addItem(TranslationManager::formatPng(), "PNG");
    m_formatCombo->addItem(TranslationManager::formatJpeg(), "JPEG");
    m_formatCombo->addItem(TranslationManager::formatBmp(), "BMP");
    fmtLayout->addRow("Format:", m_formatCombo);

    QHBoxLayout *qLayout = new QHBoxLayout();
    m_qualitySlider = new QSlider(Qt::Horizontal);
    m_qualitySlider->setRange(10, 100);
    m_qualitySpin = new QSpinBox();
    m_qualitySpin->setRange(10, 100);
    m_qualitySpin->setSuffix("%");
    connect(m_qualitySlider, &QSlider::valueChanged, m_qualitySpin, &QSpinBox::setValue);
    connect(m_qualitySpin, QOverload<int>::of(&QSpinBox::valueChanged), m_qualitySlider, &QSlider::setValue);
    qLayout->addWidget(m_qualitySlider);
    qLayout->addWidget(m_qualitySpin);
    fmtLayout->addRow(TranslationManager::jpegQuality(), qLayout);

    connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int idx) {
        bool jpeg = (m_formatCombo->itemData(idx).toString() == "JPEG");
        m_qualitySlider->setEnabled(jpeg);
        m_qualitySpin->setEnabled(jpeg);
    });
    layout->addWidget(fmtGroup);

    QGroupBox *capGroup = new QGroupBox(TranslationManager::captureSettings());
    QFormLayout *capLayout = new QFormLayout(capGroup);
    m_delaySpin = new QSpinBox();
    m_delaySpin->setRange(0, 10000);
    m_delaySpin->setSingleStep(500);
    m_delaySpin->setSuffix(" ms");
    m_delaySpin->setSpecialValueText(TranslationManager::noDelay());
    capLayout->addRow(TranslationManager::delay(), m_delaySpin);
    m_copyAfterCaptureCheck = new QCheckBox(TranslationManager::copyAfterCapture());
    capLayout->addRow(m_copyAfterCaptureCheck);
    m_closeAfterCopyCheck = new QCheckBox(TranslationManager::closeAfterCopy());
    capLayout->addRow(m_closeAfterCopyCheck);
    layout->addWidget(capGroup);

    layout->addStretch();
    return tab;
}

QWidget* SettingsDialog::createAppearanceTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QGroupBox *themeGroup = new QGroupBox(TranslationManager::theme());
    QVBoxLayout *themeLayout = new QVBoxLayout(themeGroup);
    m_darkModeCheck = new QCheckBox(TranslationManager::darkMode());
    themeLayout->addWidget(m_darkModeCheck);
    layout->addWidget(themeGroup);

    QGroupBox *overlayGroup = new QGroupBox(TranslationManager::overlaySettings());
    QFormLayout *overlayLayout = new QFormLayout(overlayGroup);
    QHBoxLayout *opLayout = new QHBoxLayout();
    m_opacitySlider = new QSlider(Qt::Horizontal);
    m_opacitySlider->setRange(0, 255);
    m_opacitySlider->setTickInterval(25);
    m_opacityValueLabel = new QLabel("39%");
    m_opacityValueLabel->setFixedWidth(40);
    connect(m_opacitySlider, &QSlider::valueChanged, [this](int val) {
        int pct = qRound(val * 100.0 / 255.0);
        m_opacityValueLabel->setText(QString("%1%").arg(pct));
    });
    opLayout->addWidget(m_opacitySlider);
    opLayout->addWidget(m_opacityValueLabel);
    overlayLayout->addRow(TranslationManager::bgOpacity(), opLayout);
    m_crosshairStyleCombo = new QComboBox();
    m_crosshairStyleCombo->addItem(TranslationManager::crossDash(), "dash");
    m_crosshairStyleCombo->addItem(TranslationManager::crossSolid(), "solid");
    m_crosshairStyleCombo->addItem(TranslationManager::crossNone(), "none");
    overlayLayout->addRow(TranslationManager::crosshair(), m_crosshairStyleCombo);
    layout->addWidget(overlayGroup);

    layout->addStretch();
    return tab;
}

QWidget* SettingsDialog::createInterfaceTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QGroupBox *toolsGroup = new QGroupBox(TranslationManager::toolbarVisibility());
    QVBoxLayout *toolsLayout = new QVBoxLayout(toolsGroup);

    QLabel *infoLabel = new QLabel(TranslationManager::toolbarDesc());
    infoLabel->setStyleSheet("color: #999;");
    toolsLayout->addWidget(infoLabel);

    m_toolVisibilityList = new QListWidget();
    m_toolVisibilityList->setAlternatingRowColors(true);

    struct ToolInfo { QString key; QString label; };
    QVector<ToolInfo> tools = {
        {"Pen",         TranslationManager::toolListPen()},
        {"Arrow",       TranslationManager::toolListArrow()},
        {"Rectangle",   TranslationManager::toolListRect()},
        {"Circle",      TranslationManager::toolListCircle()},
        {"Text",        TranslationManager::toolListText()},
        {"Highlighter", TranslationManager::toolListHighlight()},
        {"Blur",        TranslationManager::toolListBlur()},
        {"Counter",     TranslationManager::toolListCounter()},
    };

    for (const auto &t : tools) {
        QListWidgetItem *item = new QListWidgetItem(t.label);
        item->setData(Qt::UserRole, t.key);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);
        m_toolVisibilityList->addItem(item);
    }
    toolsLayout->addWidget(m_toolVisibilityList);

    QHBoxLayout *selBtnLayout = new QHBoxLayout();
    QPushButton *selectAllBtn   = new QPushButton(TranslationManager::selectAll());
    QPushButton *deselectAllBtn = new QPushButton(TranslationManager::deselectAll());
    connect(selectAllBtn,   &QPushButton::clicked, this, &SettingsDialog::onSelectAllTools);
    connect(deselectAllBtn, &QPushButton::clicked, this, &SettingsDialog::onDeselectAllTools);
    selBtnLayout->addWidget(selectAllBtn);
    selBtnLayout->addWidget(deselectAllBtn);
    selBtnLayout->addStretch();
    toolsLayout->addLayout(selBtnLayout);

    layout->addWidget(toolsGroup);
    layout->addStretch();
    return tab;
}

QWidget* SettingsDialog::createHotkeyTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setSpacing(12);

    QGroupBox *group = new QGroupBox(TranslationManager::hotkeyTitle());
    QVBoxLayout *gl = new QVBoxLayout(group);

    QLabel *desc = new QLabel(TranslationManager::hotkeyDesc());
    desc->setWordWrap(true);
    desc->setStyleSheet("color: #ccc; font-size: 12px;");
    gl->addWidget(desc);

    m_hotkeyEdit = new QKeySequenceEdit();
    m_hotkeyEdit->setMinimumHeight(40);
    m_hotkeyEdit->setStyleSheet(R"(
        QKeySequenceEdit {
            font-size: 14px;
            font-weight: bold;
            padding: 6px 12px;
            border: 2px solid #0078D4;
            border-radius: 6px;
            background: #2a2a2a;
            color: #ffffff;
        }
        QKeySequenceEdit:focus { border-color: #1a8cff; }
    )");
    connect(m_hotkeyEdit, &QKeySequenceEdit::keySequenceChanged,
            this, &SettingsDialog::onHotkeyChanged);
    gl->addWidget(m_hotkeyEdit);

    m_hotkeyStatusLabel = new QLabel(TranslationManager::hotkeyValid());
    m_hotkeyStatusLabel->setStyleSheet("color: #4caf50; font-size: 12px;");
    gl->addWidget(m_hotkeyStatusLabel);

    QPushButton *resetHkBtn = new QPushButton(TranslationManager::hotkeyReset());
    resetHkBtn->setStyleSheet("color: #aaa;");
    connect(resetHkBtn, &QPushButton::clicked, [this]() {
        m_hotkeyEdit->setKeySequence(QKeySequence(Qt::Key_Print));
    });
    gl->addWidget(resetHkBtn);

    QLabel *noteLabel = new QLabel(TranslationManager::hotkeyNote());
    noteLabel->setWordWrap(true);
    noteLabel->setStyleSheet("color: #888; font-size: 11px;");
    gl->addWidget(noteLabel);

    layout->addWidget(group);
    layout->addStretch();
    return tab;
}

void SettingsDialog::loadSettings()
{
    QString defPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

    m_savePathEdit->setText(m_settings->value("savePath", defPath).toString());
    m_filenamePatternEdit->setText(m_settings->value("filenamePattern", "Screenshot_%Y-%M-%D_%h-%m-%s").toString());
    onFilenamePatternChanged(m_filenamePatternEdit->text());

    m_autoStartCheck->setChecked(m_settings->value("autoStart", false).toBool());
    m_showNotificationsCheck->setChecked(m_settings->value("showNotifications", true).toBool());
    m_playSoundCheck->setChecked(m_settings->value("playSound", false).toBool());
    m_copyPathAfterSaveCheck->setChecked(m_settings->value("copyPathAfterSave", false).toBool());

    // Dil
    QString lang = m_settings->value("language", TranslationManager::langCode()).toString();
    int li = m_langCombo->findData(lang);
    if (li >= 0) m_langCombo->setCurrentIndex(li);

    QString fmt = m_settings->value("imageFormat", "PNG").toString();
    int fi = m_formatCombo->findData(fmt);
    if (fi >= 0) m_formatCombo->setCurrentIndex(fi);

    int q = m_settings->value("imageQuality", 95).toInt();
    m_qualitySlider->setValue(q);
    m_qualitySpin->setValue(q);
    m_delaySpin->setValue(m_settings->value("captureDelay", 0).toInt());
    m_copyAfterCaptureCheck->setChecked(m_settings->value("copyAfterCapture", true).toBool());
    m_closeAfterCopyCheck->setChecked(m_settings->value("closeAfterCopy", true).toBool());

    bool jpeg = (fmt == "JPEG");
    m_qualitySlider->setEnabled(jpeg);
    m_qualitySpin->setEnabled(jpeg);

    m_darkModeCheck->setChecked(m_settings->value("darkMode", true).toBool());
    int opacity = m_settings->value("overlayOpacity", 100).toInt();
    m_opacitySlider->setValue(opacity);
    QString cross = m_settings->value("crosshairStyle", "dash").toString();
    int ci = m_crosshairStyleCombo->findData(cross);
    if (ci >= 0) m_crosshairStyleCombo->setCurrentIndex(ci);

    QStringList visibleTools = m_settings->value("visibleTools",
        QStringList{"Pen","Arrow","Rectangle","Circle","Text","Highlighter","Blur","Counter"})
        .toStringList();
    for (int i = 0; i < m_toolVisibilityList->count(); ++i) {
        QListWidgetItem *item = m_toolVisibilityList->item(i);
        QString key = item->data(Qt::UserRole).toString();
        item->setCheckState(visibleTools.contains(key) ? Qt::Checked : Qt::Unchecked);
    }

    UINT savedMod  = static_cast<UINT>(m_settings->value("hotkeyModifiers", 0).toUInt());
    UINT savedVKey = static_cast<UINT>(m_settings->value("hotkeyVKey", VK_SNAPSHOT).toUInt());
    m_hotkeyEdit->setKeySequence(win32ToKeySequence(savedMod, savedVKey));
    m_hotkeyStatusLabel->setText(TranslationManager::hotkeyValid());
    m_hotkeyStatusLabel->setStyleSheet("color: #4caf50; font-size: 12px;");
}

void SettingsDialog::onHotkeyChanged(const QKeySequence &seq)
{
    UINT mod = 0, vk = 0;
    bool ok = keySequenceToWin32(seq, mod, vk);
    if (!ok || seq.isEmpty()) {
        m_hotkeyStatusLabel->setText(TranslationManager::hotkeyInvalid());
        m_hotkeyStatusLabel->setStyleSheet("color: #ff9800; font-size: 12px;");
    } else {
        m_hotkeyStatusLabel->setText(QString("✅ %1").arg(seq.toString(QKeySequence::NativeText)));
        m_hotkeyStatusLabel->setStyleSheet("color: #4caf50; font-size: 12px;");
    }
}

void SettingsDialog::onFilenamePatternChanged(const QString &text)
{
    QString preview = resolvePatternPreview(text);
    m_patternPreviewLabel->setText(TranslationManager::patternPreview() + ": " + preview + ".png");
}

void SettingsDialog::onBrowse()
{
    QString dir = QFileDialog::getExistingDirectory(this, TranslationManager::saveDir(), m_savePathEdit->text());
    if (!dir.isEmpty()) m_savePathEdit->setText(dir);
}

void SettingsDialog::onSelectAllTools()
{
    for (int i = 0; i < m_toolVisibilityList->count(); ++i)
        m_toolVisibilityList->item(i)->setCheckState(Qt::Checked);
}

void SettingsDialog::onDeselectAllTools()
{
    for (int i = 0; i < m_toolVisibilityList->count(); ++i)
        m_toolVisibilityList->item(i)->setCheckState(Qt::Unchecked);
}

void SettingsDialog::onSave()
{
    QString savePath = m_savePathEdit->text();
    if (!savePath.isEmpty()) {
        QDir dir(savePath);
        if (!dir.exists() && !dir.mkpath(".")) {
            QMessageBox::warning(this, TranslationManager::errTitle(), TranslationManager::errSaveDir() + savePath);
            return;
        }
    }

    QKeySequence seq = m_hotkeyEdit->keySequence();
    UINT newMod = 0, newVKey = 0;
    if (!seq.isEmpty()) {
        if (!keySequenceToWin32(seq, newMod, newVKey)) {
            QMessageBox::warning(this, TranslationManager::errInvalidHotkeyTitle(), TranslationManager::errInvalidHotkey());
            return;
        }
    } else {
        newMod  = 0;
        newVKey = VK_SNAPSHOT;
    }

    // Dil kaydet
    QString newLang = m_langCombo->currentData().toString();
    TranslationManager::setLanguage(newLang == "tr" ? TranslationManager::Turkish : TranslationManager::English);

    m_settings->setValue("language",          newLang);
    m_settings->setValue("savePath",          savePath);
    m_settings->setValue("filenamePattern",    m_filenamePatternEdit->text());
    m_settings->setValue("autoStart",          m_autoStartCheck->isChecked());
    m_settings->setValue("showNotifications",  m_showNotificationsCheck->isChecked());
    m_settings->setValue("playSound",          m_playSoundCheck->isChecked());
    m_settings->setValue("copyPathAfterSave",  m_copyPathAfterSaveCheck->isChecked());

    m_settings->setValue("imageFormat",        m_formatCombo->currentData().toString());
    m_settings->setValue("imageQuality",       m_qualitySpin->value());
    m_settings->setValue("captureDelay",       m_delaySpin->value());
    m_settings->setValue("copyAfterCapture",   m_copyAfterCaptureCheck->isChecked());
    m_settings->setValue("closeAfterCopy",     m_closeAfterCopyCheck->isChecked());

    m_settings->setValue("darkMode",           m_darkModeCheck->isChecked());
    m_settings->setValue("overlayOpacity",     m_opacitySlider->value());
    m_settings->setValue("crosshairStyle",     m_crosshairStyleCombo->currentData().toString());

    QStringList visibleTools;
    for (int i = 0; i < m_toolVisibilityList->count(); ++i) {
        QListWidgetItem *item = m_toolVisibilityList->item(i);
        if (item->checkState() == Qt::Checked)
            visibleTools.append(item->data(Qt::UserRole).toString());
    }
    m_settings->setValue("visibleTools", visibleTools);

    m_settings->setValue("hotkeyModifiers", newMod);
    m_settings->setValue("hotkeyVKey",      newVKey);
    HotkeyManager::instance().reRegisterCaptureHotkey(newMod, newVKey);

#ifdef Q_OS_WIN
    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                   QSettings::NativeFormat);
    if (m_autoStartCheck->isChecked()) {
        QString appPath = QCoreApplication::applicationFilePath().replace('/', '\\');
        reg.setValue("EShot", "\"" + appPath + "\"");
    } else {
        reg.remove("EShot");
    }
#endif

    m_settings->sync();
    accept();
}

void SettingsDialog::onReset()
{
    if (QMessageBox::question(this, TranslationManager::resetTitle(),
            TranslationManager::resetConfirm(),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        m_settings->clear();
        m_settings->sync();
        TranslationManager::init();
        loadSettings();
    }
}
