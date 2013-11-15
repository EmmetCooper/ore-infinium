/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU General Public License as           *
 *   published by the Free Software Foundation; either version 2 of           *
 *   the License, or (at your option) any later version.                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/

#include "chatmodel.h"

#include "src/client/client.h"

#include "src/debug.h"

#include <QVariant>

ChatModel::ChatModel(QObject* parent)
: QAbstractListModel(parent)
{
    setObjectName("chatModel");
}

ChatModel::~ChatModel()
{
}

QVariant ChatModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    ChatLine line = m_chatLines.at(index.row());

    switch (role) {
        case TimeStampRole:
            return line.timeStamp;

        case PlayerNameRole:
            return line.playerName;

        case ChatTextRole:
            return line.chatText;

        default:
            Q_ASSERT_X(0, "chat model", "role not handled!");
    }
}

QHash< int, QByteArray > ChatModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[TimeStampRole] = "timeStamp";
    roles[PlayerNameRole] = "playerName";
    roles[ChatTextRole] = "chatText";

    return roles;
}


Qt::ItemFlags ChatModel::flags(const QModelIndex& index) const
{
    return Qt::NoItemFlags;
}

int ChatModel::rowCount(const QModelIndex& parent) const
{
    return m_chatLines.size();
}

void ChatModel::addChatLine(const QString& timestamp, const QString& playerName, const QString& line)
{
    int row = m_chatLines.size();

    beginInsertRows(QModelIndex(), row, row);

    ChatLine chat(timestamp, playerName, line);

    m_chatLines.append(chat);

    endInsertRows();
}
