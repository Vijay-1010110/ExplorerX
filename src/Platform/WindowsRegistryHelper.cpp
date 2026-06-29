#include "WindowsRegistryHelper.h"

#include <QSettings>
#include <QFileIconProvider>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QStringList>

namespace ExplorerX::Platform {

std::vector<ShellNewTemplate> WindowsRegistryHelper::GetShellNewTemplates() {
    static std::vector<ShellNewTemplate> cachedTemplates;
    static bool isCached = false;

    if (isCached) {
        return cachedTemplates;
    }

    QSettings classesRoot("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
    QStringList allExtensions = classesRoot.childGroups();
    
    QFileIconProvider iconProvider;

    for (const QString& ext : allExtensions) {
        if (!ext.startsWith(".")) {
            continue;
        }

        bool hasShellNew = false;
        QString progId;

        // Check if the extension itself has a ShellNew key
        classesRoot.beginGroup(ext);
        if (classesRoot.childGroups().contains("ShellNew", Qt::CaseInsensitive)) {
            hasShellNew = true;
        }
        
        // Read the default value which is typically the ProgID (e.g. Word.Document.12)
        progId = classesRoot.value(".").toString();
        classesRoot.endGroup();

        // If not directly on the extension, check the ProgID
        if (!hasShellNew && !progId.isEmpty()) {
            classesRoot.beginGroup(progId);
            if (classesRoot.childGroups().contains("ShellNew", Qt::CaseInsensitive)) {
                hasShellNew = true;
            }
            classesRoot.endGroup();
        }

        if (hasShellNew) {
            ShellNewTemplate tpl;
            tpl.extension = ext;
            
            // Try to get a friendly name from the ProgID or the extension
            QString friendlyName;
            if (!progId.isEmpty()) {
                classesRoot.beginGroup(progId);
                friendlyName = classesRoot.value(".").toString();
                classesRoot.endGroup();
            }
            
            if (friendlyName.isEmpty()) {
                // Fallback to the extension itself or a generic name
                friendlyName = QString("%1 File").arg(ext.mid(1).toUpper());
            }
            tpl.friendlyName = friendlyName;

            // Generate a dummy file to extract its native icon
            QTemporaryFile tempFile;
            tempFile.setFileTemplate("XXXXXX" + ext);
            if (tempFile.open()) {
                QFileInfo fileInfo(tempFile.fileName());
                tpl.icon = iconProvider.icon(fileInfo);
                tempFile.close();
            }

            cachedTemplates.push_back(tpl);
        }
    }

    isCached = true;
    return cachedTemplates;
}

} // namespace ExplorerX::Platform
