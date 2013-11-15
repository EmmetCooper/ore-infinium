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

#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <QAbstractListModel>

#include <QList>
#include <QString>

class ChatModel : public QAbstractListModel
{
    Q_OBJECT
//    Q_PROPERTY(QString name READ name WRITE setName)

public:
    ChatModel(QObject* parent = 0);
    ~ChatModel();

    enum ChatRoles {
        TimeStampRole = Qt::UserRole + 1,
        PlayerNameRole,
        ChatTextRole
    };

    int rowCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QHash<int, QByteArray> roleNames() const;

    void addChatLine(const QString& timeStamp,const QString& playerName, const QString& line);

private:
    struct ChatLine {
        ChatLine(const QString& _timeStamp, const QString& _playerName, const QString& _chatText)
        : timeStamp(_timeStamp),
          playerName(_playerName),
          chatText(_chatText)
        {

        }

        QString timeStamp;

        QString playerName;
        QString chatText;
    };

    QList<ChatLine> m_chatLines;
};

#endif
