/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 * http://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *   Alexandre C Vieira <acamargo.vieira@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#ifndef PLACESMENU_H
#define PLACESMENU_H

#include "ilxqtpanelplugin.h"
#include <vector>
#include <QLabel>
#include <QToolButton>
#include <QDomElement>
#include <QAction>
#include <QDir>
#include <QSignalMapper>
#include <QSettings>
#include <QMenu>
#include <libfm-qt/core/bookmarks.h>
#include <libfm-qt/core/filepath.h>

class PlacesMenu :  public QObject, public ILXQtPanelPlugin
{
    Q_OBJECT

public:
    PlacesMenu(const ILXQtPanelPluginStartupInfo& startupInfo);
    ~PlacesMenu();

    virtual QWidget* widget() { return &mButton; }
    virtual QString themeId() const { return "PlacesMenu"; }
    virtual Flags flags() const { return SingleInstance; }
    
private Q_SLOTS:
    void showMenu();
    void openDirectory(const QString& path);
    void createMenuItem(QMenu* menu, const QString& name, const QString& iconName, const QString& location);
    void createMenuItem(QMenu* menu, const QString& name, QIcon icon,  const QString& location);
    void createMenuItemMount(QMenu* menu, const QString& name, const QString& iconName, const QString& mountName);
    void createSubmenu(QMenu* menu, GMount* mount);
    void createSubmenu(QMenu* menu, GVolume* volume);
    void onEject(const QString& volumeName);						 

protected Q_SLOTS:
    void buildMenu();

protected:
    static void onVolumeAdded(GVolumeMonitor* monitor, GVolume* volume, PlacesMenu* pThis);
    static void onVolumeRemoved(GVolumeMonitor* monitor, GVolume* volume, PlacesMenu* pThis);
    static void showMessage(const QString& text);

private:
    void addActions(QMenu* menu);
       
    QToolButton mButton;
    QMenu* mMenu;
    QSignalMapper* mOpenDirectorySignalMapper;
    QSignalMapper* mEjectSignalMapper;

    QDir mBaseDirectory;
    const QString mDefaultIcon = "folder";
    std::vector<QString> mPathStrings;
    std::shared_ptr<Fm::Bookmarks> bookmarks_;
    QMap<QString, GVolume*> mapVolumes;

    GVolumeMonitor* volumeMonitor;
};

class PlacesMenuLibrary: public QObject, public ILXQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILXQtPanelPluginLibrary)
public:
    ILXQtPanelPlugin* instance(const ILXQtPanelPluginStartupInfo &startupInfo) const
    {
        return new PlacesMenu(startupInfo);
    }
};


#endif

