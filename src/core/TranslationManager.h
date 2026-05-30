#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

#include <QString>
#include <QMap>
#include <QSettings>
#include <QLocale>

class TranslationManager {
public:
    enum Language { Turkish, English };

    static void init() {
        QSettings s("EShot", "EShot");
        int saved = s.value("language", -1).toInt();
        if (saved >= 0) {
            m_lang = static_cast<Language>(saved);
        } else {
            // Sistem diline göre otomatik seç
            QLocale loc = QLocale::system();
            m_lang = (loc.language() == QLocale::Turkish) ? Turkish : English;
        }
    }

    static void setLanguage(Language lang) {
        m_lang = lang;
        QSettings s("EShot", "EShot");
        s.setValue("language", static_cast<int>(lang));
    }

    static Language currentLanguage() { return m_lang; }

    static QString langCode() { return m_lang == Turkish ? "tr" : "en"; }

    // ─── Genel ───
    static QString appTitle()         { return m_lang == Turkish ? "EShot - Ekran Görüntüsü Aracı" : "EShot - Screenshot Tool"; }
    static QString settingsTitle()    { return m_lang == Turkish ? "EShot Ayarları" : "EShot Settings"; }
    static QString save()             { return m_lang == Turkish ? "Kaydet" : "Save"; }
    static QString cancel()           { return m_lang == Turkish ? "İptal" : "Cancel"; }
    static QString reset()            { return m_lang == Turkish ? "Varsayılana Sıfırla" : "Reset to Default"; }
    static QString browse()           { return m_lang == Turkish ? "Gözat..." : "Browse..."; }
    static QString language()         { return m_lang == Turkish ? "Dil" : "Language"; }

    // ─── Tab isimleri ───
    static QString tabGeneral()       { return m_lang == Turkish ? "Genel" : "General"; }
    static QString tabCapture()       { return m_lang == Turkish ? "Yakalama" : "Capture"; }
    static QString tabAppearance()    { return m_lang == Turkish ? "Görünüm" : "Appearance"; }
    static QString tabInterface()     { return m_lang == Turkish ? "Arayüz" : "Interface"; }
    static QString tabHotkey()        { return m_lang == Turkish ? "Kısayol Tuşu" : "Hotkey"; }

    // ─── Genel tab ───
    static QString saveDir()          { return m_lang == Turkish ? "Kayıt Dizini" : "Save Directory"; }
    static QString saveDirDesc()      { return m_lang == Turkish ? "Ekran görüntülerinin kaydedileceği dizin" : "Directory where screenshots will be saved"; }
    static QString filenamePattern()  { return m_lang == Turkish ? "Dosya Adı Şablonu" : "Filename Pattern"; }
    static QString patternPreview()   { return m_lang == Turkish ? "Önizleme" : "Preview"; }
    static QString patternVars() {
        return m_lang == Turkish
            ? "<b>Değişkenler:</b> %Y (yıl 4h), %y (yıl 2h), %M (ay), %D (gün), %h (saat), %m (dakika), %s (saniye), %T (pencere başlığı)"
            : "<b>Variables:</b> %Y (year 4h), %y (year 2h), %M (month), %D (day), %h (hour), %m (minute), %s (second), %T (window title)";
    }
    static QString generalOptions()   { return m_lang == Turkish ? "Genel Seçenekler" : "General Options"; }
    static QString autoStart()        { return m_lang == Turkish ? "Windows ile birlikte başlat" : "Start with Windows"; }
    static QString showNotifications(){ return m_lang == Turkish ? "Bildirim göster" : "Show notifications"; }
    static QString playSound()        { return m_lang == Turkish ? "Yakalama sesi çal" : "Play capture sound"; }
    static QString copyPathAfterSave(){ return m_lang == Turkish ? "Kaydettikten sonra dosya yolunu panoya kopyala" : "Copy file path to clipboard after save"; }

    // ─── Yakalama tab ───
    static QString fileFormat()       { return m_lang == Turkish ? "Dosya Formatı" : "File Format"; }
    static QString formatPng()        { return m_lang == Turkish ? "PNG (Kayıpsız)" : "PNG (Lossless)"; }
    static QString formatJpeg()       { return m_lang == Turkish ? "JPEG (Küçük dosya)" : "JPEG (Smaller file)"; }
    static QString formatBmp()        { return m_lang == Turkish ? "BMP (Sıkıştırmasız)" : "BMP (Uncompressed)"; }
    static QString jpegQuality()      { return m_lang == Turkish ? "JPEG Kalitesi:" : "JPEG Quality:"; }
    static QString captureSettings()  { return m_lang == Turkish ? "Yakalama Ayarları" : "Capture Settings"; }
    static QString delay()            { return m_lang == Turkish ? "Gecikme:" : "Delay:"; }
    static QString noDelay()          { return m_lang == Turkish ? "Gecikme yok" : "No delay"; }
    static QString copyAfterCapture() { return m_lang == Turkish ? "Yakaladıktan sonra panoya kopyala" : "Copy to clipboard after capture"; }
    static QString closeAfterCopy()   { return m_lang == Turkish ? "Kopyaladıktan sonra overlay'i kapat" : "Close overlay after copy"; }

    // ─── Görünüm tab ───
    static QString theme()            { return m_lang == Turkish ? "Tema" : "Theme"; }
    static QString darkMode()         { return m_lang == Turkish ? "Koyu tema kullan (yeniden başlatma gerekir)" : "Use dark theme (restart required)"; }
    static QString overlaySettings()  { return m_lang == Turkish ? "Overlay Ayarları" : "Overlay Settings"; }
    static QString bgOpacity()        { return m_lang == Turkish ? "Arka Plan Opaklığı:" : "Background Opacity:"; }
    static QString crosshair()        { return m_lang == Turkish ? "Crosshair:" : "Crosshair:"; }
    static QString crossDash()        { return m_lang == Turkish ? "Kesikli çizgi" : "Dashed line"; }
    static QString crossSolid()       { return m_lang == Turkish ? "Düz çizgi" : "Solid line"; }
    static QString crossNone()        { return m_lang == Turkish ? "Kapalı" : "Off"; }

    // ─── Arayüz tab ───
    static QString toolbarVisibility(){ return m_lang == Turkish ? "Araç Çubuğu Görünürlüğü" : "Toolbar Visibility"; }
    static QString toolbarDesc()      { return m_lang == Turkish ? "Toolbar'da gösterilecek araçları seçin:" : "Select tools to show in the toolbar:"; }
    static QString selectAll()        { return m_lang == Turkish ? "Tümünü Seç" : "Select All"; }
    static QString deselectAll()      { return m_lang == Turkish ? "Tümünü Kaldır" : "Deselect All"; }

    // ─── Kısayol tab ───
    static QString hotkeyTitle()      { return m_lang == Turkish ? "Yakalama Kısayol Tuşu" : "Capture Hotkey"; }
    static QString hotkeyDesc() {
        return m_lang == Turkish
            ? "Ekran görüntüsü almak için kullanılacak kısayol tuşunu aşağıya tıklayıp yeni kısayolu basarak ayarlayın.\n\nVarsayılan: <b>Print Screen</b>"
            : "Click below and press a key to set the screenshot hotkey.\n\nDefault: <b>Print Screen</b>";
    }
    static QString hotkeyValid()      { return m_lang == Turkish ? "✅ Kısayol geçerli." : "✅ Hotkey is valid."; }
    static QString hotkeyReset()      { return m_lang == Turkish ? "🔄 Varsayılana Döndür (Print Screen)" : "🔄 Reset to Default (Print Screen)"; }
    static QString hotkeyNote() {
        return m_lang == Turkish
            ? "⚠️ Not: Bazı sistem kısayolları (Win+L, Ctrl+Alt+Del vb.) engellenemez. Modifier tuşları (Ctrl, Alt, Shift) ile kombinasyon tavsiye edilir."
            : "⚠️ Note: Some system shortcuts (Win+L, Ctrl+Alt+Del etc.) cannot be overridden. Combos with modifier keys (Ctrl, Alt, Shift) are recommended.";
    }
    static QString hotkeyInvalid()    { return m_lang == Turkish ? "⚠️ Geçersiz kısayol. Lütfen tekrar deneyin." : "⚠️ Invalid hotkey. Please try again."; }

    // ─── Dil seçimi ───
    static QString langTurkish()      { return m_lang == Turkish ? "Türkçe" : "Turkish"; }
    static QString langEnglish()      { return m_lang == Turkish ? "İngilizce" : "English"; }

    // ─── Tray ───
    static QString trayCapture()      { return m_lang == Turkish ? "Yakala" : "Capture"; }
    static QString traySettings()     { return m_lang == Turkish ? "Ayarlar" : "Settings"; }
    static QString trayAbout()        { return m_lang == Turkish ? "Hakkında" : "About"; }
    static QString trayQuit()         { return m_lang == Turkish ? "Çıkış" : "Quit"; }

    // ─── Toolbar butonları ───
    static QString toolPen()          { return m_lang == Turkish ? "Kalem (P)" : "Pen (P)"; }
    static QString toolArrow()        { return m_lang == Turkish ? "Ok (A)" : "Arrow (A)"; }
    static QString toolRect()         { return m_lang == Turkish ? "Kare (R)" : "Rectangle (R)"; }
    static QString toolCircle()       { return m_lang == Turkish ? "Çember (C) [Shift=Tam Daire]" : "Circle (C) [Shift=Perfect Circle]"; }
    static QString toolText()         { return m_lang == Turkish ? "Metin (T)" : "Text (T)"; }
    static QString toolHighlighter()  { return m_lang == Turkish ? "Vurgulayıcı (H)" : "Highlighter (H)"; }
    static QString toolBlur()         { return m_lang == Turkish ? "Bulanıklaştır (B)" : "Blur (B)"; }
    static QString toolCounter()      { return m_lang == Turkish ? "Numara (#)" : "Counter (#)"; }
    static QString toolColor()        { return m_lang == Turkish ? "Renk Seç" : "Pick Color"; }
    static QString toolWidth()        { return m_lang == Turkish ? "Çizgi Kalınlığı" : "Pen Width"; }
    static QString toolUndo()         { return m_lang == Turkish ? "Geri Al (Ctrl+Z)" : "Undo (Ctrl+Z)"; }
    static QString toolRedo()         { return m_lang == Turkish ? "İleri Al (Ctrl+Y)" : "Redo (Ctrl+Y)"; }
    static QString actionPin()        { return m_lang == Turkish ? "Ekrana Sabitle 📌" : "Pin to Screen 📌"; }
    static QString actionCopy()       { return m_lang == Turkish ? "Kopyala (Ctrl+C / Enter)" : "Copy (Ctrl+C / Enter)"; }
    static QString actionSave()       { return m_lang == Turkish ? "Kaydet (Ctrl+S)" : "Save (Ctrl+S)"; }
    static QString actionClose()      { return m_lang == Turkish ? "Kapat (Esc)" : "Close (Esc)"; }

    // ─── Araç görünürlüğü listesi ───
    static QString toolListPen()      { return m_lang == Turkish ? "✏️ Kalem" : "✏️ Pen"; }
    static QString toolListArrow()    { return m_lang == Turkish ? "➡️ Ok" : "➡️ Arrow"; }
    static QString toolListRect()     { return m_lang == Turkish ? "⬜ Kare" : "⬜ Rectangle"; }
    static QString toolListCircle()   { return m_lang == Turkish ? "⭕ Çember / Elips" : "⭕ Circle / Ellipse"; }
    static QString toolListText()     { return m_lang == Turkish ? "🔤 Metin" : "🔤 Text"; }
    static QString toolListHighlight(){ return m_lang == Turkish ? "🖍️ Vurgulayıcı" : "🖍️ Highlighter"; }
    static QString toolListBlur()     { return m_lang == Turkish ? "🔲 Bulanıklaştır" : "🔲 Blur"; }
    static QString toolListCounter()  { return m_lang == Turkish ? "🔢 Numara Sayacı" : "🔢 Counter"; }

    // ─── Pinned ───
    static QString pinnedLabel()      { return m_lang == Turkish ? "Sabitlendi" : "Pinned"; }
    static QString pinnedCopy()       { return m_lang == Turkish ? "Kopyala" : "Copy"; }
    static QString pinnedClose()      { return m_lang == Turkish ? "Kapat" : "Close"; }

    // ─── Hatalar ───
    static QString errSaveDir()       { return m_lang == Turkish ? "Kayıt dizini oluşturulamadı: " : "Failed to create save directory: "; }
    static QString errInvalidHotkey() { return m_lang == Turkish ? "Seçilen kısayol tuşu geçerli değil.\nLütfen geçerli bir kısayol seçin veya varsayılan olarak bırakın." : "The selected hotkey is not valid.\nPlease select a valid hotkey or leave it as default."; }
    static QString errTitle()         { return m_lang == Turkish ? "Hata" : "Error"; }
    static QString errInvalidHotkeyTitle() { return m_lang == Turkish ? "Geçersiz Kısayol" : "Invalid Hotkey"; }

    // ─── Sıfırlama ───
    static QString resetTitle()       { return m_lang == Turkish ? "Sıfırla" : "Reset"; }
    static QString resetConfirm()     { return m_lang == Turkish ? "Tüm ayarlar varsayılan değerlere sıfırlansın mı?" : "Reset all settings to default values?"; }

    // ─── About ───
    static QString aboutTitle()       { return m_lang == Turkish ? "EShot Hakkında" : "About EShot"; }

    // ─── Bildirim ───
    static QString notifCaptureTitle(){ return m_lang == Turkish ? "EShot" : "EShot"; }
    static QString notifCaptureMsg(int w, int h) {
        return m_lang == Turkish
            ? QString("Ekran görüntüsü alındı (%1x%2)").arg(w).arg(h)
            : QString("Screenshot taken (%1x%2)").arg(w).arg(h);
    }

private:
    static inline Language m_lang = Turkish;
};

#endif
