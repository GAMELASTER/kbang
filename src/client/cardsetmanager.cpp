/***************************************************************************
 *   Copyright (C) 2009 by MacJariel                                       *
 *   echo "badmailet@gbalt.dob" | tr "edibmlt" "ecrmjil"                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "cardsetmanager.h"
#include "config.h"

using namespace client;

CardSetManager::CardSetManager()
{
    refreshKnownSlots();
}

/* static */ CardSetManager&
CardSetManager::instance()
{
    static CardSetManager singleton;
    return singleton;
}

void CardSetManager::refreshLocalCardSets()
{
    m_localCardSets.clear();
    foreach (QString dataLocation, Config::dataLocations()) {
        QDir cardsetDir(dataLocation);
        if (!cardsetDir.cd("cardsets")) {
            continue;
        }
        cardsetDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (QString cardSet, cardsetDir.entryList()) {
            m_localCardSets.append(CardSetInfo(cardsetDir.filePath(cardSet)));
        }
    }
}

void
CardSetManager::addKnownSlot(QString knownSlot)
{
    if (m_knownSlots.contains(knownSlot) || knownSlot.isEmpty()) {
        return;
    }
    m_knownSlots.append(knownSlot);
    saveKnownSlots();
}


void
CardSetManager::refreshKnownSlots()
{
    m_knownSlots.clear();
    foreach (QString dataLocation, Config::dataLocations()) {
        QDir cardsetDir(dataLocation);
        if (!cardsetDir.exists("known_slots")) {
            continue;
        }
        QFile knownSlotsFile(cardsetDir.filePath("known_slots"));
        if (!knownSlotsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue;
        }
        QTextStream in(&knownSlotsFile);
        while (!in.atEnd()) {
            QString knownSlot = in.readLine();
            if (!m_knownSlots.contains(knownSlot)) {
                m_knownSlots.append(knownSlot);
            }
        }
    }
}

void
CardSetManager::saveKnownSlots()
{
    QStringList extraKnownSlots = m_knownSlots;
    QDir cardsetDir(Config::userDataLocation());
    QFile knownSlotsFile(cardsetDir.filePath("known_slots"));
    if (!knownSlotsFile.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)) {
        qWarning("Cannot write to '%s'.", qPrintable(knownSlotsFile.fileName()));
        return;
    }
    QTextStream stream(&knownSlotsFile);
    while (!stream.atEnd()) {
        extraKnownSlots.removeAll(stream.readLine());
    }
    foreach (QString knownSlot, extraKnownSlots) {
        stream << knownSlot << endl;
    }
}
