#include "OCREngine.h"
#include <QProcess>
#include <QTemporaryFile>
#include <QDir>
#include <QFile>
#include <QDebug>

OCREngine::OCREngine(QObject *parent)
    : QObject(parent)
    , m_processing(false)
{
}

OCREngine::~OCREngine() {}

void OCREngine::extractText(const QPixmap &pixmap)
{
    if (m_processing) return;
    m_processing = true;

    QTemporaryFile tmpFile(QDir::tempPath() + "/eshot_ocr_XXXXXX.png");
    tmpFile.setAutoRemove(false); // PowerShell çalışana kadar dosya kalmalı
    if (!tmpFile.open()) {
        emit ocrError("Failed to create temp file");
        m_processing = false;
        return;
    }
    pixmap.save(&tmpFile, "PNG");
    QString imgPath = tmpFile.fileName();
    tmpFile.close();

    // Basit PowerShell OCR scripti
    QString psScript =
        "Add-Type -AssemblyName System.Runtime.WindowsRuntime\n"
        "\n"
        "# WinRT tiplerini yükle\n"
        "[void][Windows.ApplicationModel.Package,Windows.ApplicationModel,ContentType=WindowsRuntime]\n"
        "[void][Windows.Storage.Streams.IRandomAccessStream,Windows.Storage.Streams,ContentType=WindowsRuntime]\n"
        "[void][Windows.Media.Ocr.OcrEngine,Windows.Media.Ocr,ContentType=WindowsRuntime]\n"
        "[void][Windows.Graphics.Imaging.SoftwareBitmap,Windows.Graphics.Imaging,ContentType=WindowsRuntime]\n"
        "[void][Windows.Graphics.Imaging.BitmapDecoder,Windows.Graphics.Imaging,ContentType=WindowsRuntime]\n"
        "[void][Windows.Storage.StorageFile,Windows.Storage,ContentType=WindowsRuntime]\n"
        "[void][Windows.Globalization.Language,Windows.Globalization,ContentType=WindowsRuntime]\n"
        "\n"
        "# Await fonksiyonu\n"
        "Function Await($WinRtTask, $ResultType) {\n"
        "    $asTaskGeneric = ([System.WindowsRuntimeSystemExtensions].GetMethods() | Where-Object { $_.Name -eq 'GetAwaiter' -and $_.GetParameters().Count -eq 1 -and $_.GetParameters()[0].ParameterType.Name -eq 'IAsyncOperation`1' })[0]\n"
        "    $asTask = $asTaskGeneric.MakeGenericMethod($ResultType)\n"
        "    $netTask = $asTask.Invoke($null, @($WinRtTask))\n"
        "    $netTask.GetAwaiter().GetResult()\n"
        "}\n"
        "\n"
        "$imgPath = '%1'\n"
        "\n"
        "try {\n"
        "    $file = Await([Windows.Storage.StorageFile]::GetFileFromPathAsync($imgPath)) ([Windows.Storage.StorageFile])\n"
        "    $stream = Await($file.OpenReadAsync()) ([Windows.Storage.Streams.IRandomAccessStreamWithContentType])\n"
        "    $decoder = Await([Windows.Graphics.Imaging.BitmapDecoder]::CreateAsync($stream)) ([Windows.Graphics.Imaging.BitmapDecoder])\n"
        "    $bmp = Await($decoder.GetSoftwareBitmapAsync()) ([Windows.Graphics.Imaging.SoftwareBitmap])\n"
        "    \n"
        "    $lang = [Windows.Media.Ocr.OcrEngine]::TryCreateFromUserProfileLanguages()\n"
        "    if (-not $lang) { $lang = [Windows.Media.Ocr.OcrEngine]::TryCreateFromLanguage([Windows.Globalization.Language]('en-US')) }\n"
        "    if (-not $lang) { Write-Error 'No OCR engine available'; exit 1 }\n"
        "    \n"
        "    $ocrResult = Await($lang.RecognizeAsync($bmp)) ([Windows.Media.Ocr.OcrResult])\n"
        "    Write-Output $ocrResult.Text\n"
        "} catch {\n"
        "    Write-Error $_.Exception.Message\n"
        "    exit 1\n"
        "}\n";

    psScript.replace("%1", imgPath.replace(QLatin1Char('\\'), QLatin1String("\\\\")));

    QProcess *process = new QProcess(this);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, process, imgPath](int exitCode, QProcess::ExitStatus) {
        QString output = process->readAllStandardOutput().trimmed();
        QString err = process->readAllStandardError().trimmed();
        process->deleteLater();
        QFile::remove(imgPath); // Temp dosyayı temizle
        m_processing = false;

        qDebug() << "[OCR] exit:" << exitCode << "output:" << output.left(100) << "err:" << err.left(200);

        if (!output.isEmpty()) {
            emit textExtracted(output);
        } else {
            emit ocrError(err.isEmpty() ? "OCR failed: no output" : err);
        }
    });

    process->setProgram("powershell");
    process->setArguments({"-NoProfile", "-NonInteractive", "-Command", psScript});
    process->start();
}
