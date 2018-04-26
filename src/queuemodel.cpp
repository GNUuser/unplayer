/*
 * Unplayer
 * Copyright (C) 2015-2018 Alexey Rochev <equeim@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "queuemodel.h"

namespace unplayer
{
    QVariant QueueModel::data(const QModelIndex& index, int role) const
    {
        if (!index.isValid()) {
            return QVariant();
        }

        const QueueTrack* track = mTracks.at(index.row()).get();

        switch (role) {
        case FilePathRole:
            return track->filePath;
        case TitleRole:
            return track->title;
        case ArtistRole:
            return track->artist;
        case DisplayedArtistRole:
            return track->artist;
        case AlbumRole:
            return track->album;
        case DisplayedAlbumRole:
            return track->album;
        case DurationRole:
            return track->duration;
        default:
            return QVariant();
        }
    }

    int QueueModel::rowCount(const QModelIndex&) const
    {
        return mTracks.size();
    }

    Queue* QueueModel::queue() const
    {
        return mQueue;
    }

    void QueueModel::setQueue(Queue* queue)
    {
        mQueue = queue;
        mTracks = mQueue->tracks();

        QObject::connect(mQueue, &Queue::tracksAdded, this, [=](int start) {
            const QList<std::shared_ptr<QueueTrack>>& tracks = mQueue->tracks();
            for (int i = start, max = tracks.size(); i < max; i++) {
                beginInsertRows(QModelIndex(), i, i);
                mTracks.append(tracks.at(i));
                endInsertRows();
            }
        });

        QObject::connect(mQueue, &Queue::trackRemoved, this, [=](int index) {
            beginRemoveRows(QModelIndex(), index, index);
            mTracks.removeAt(index);
            endRemoveRows();
        });

        QObject::connect(mQueue, &Queue::tracksRemoved, this, [=](const QVector<int>& indexes) {
            for (int index : indexes) {
                beginRemoveRows(QModelIndex(), index, index);
                mTracks.removeAt(index);
                endRemoveRows();
            }
        });

        QObject::connect(mQueue, &Queue::cleared, this, [=]() {
            beginRemoveRows(QModelIndex(), 0, mTracks.size() - 1);
            mTracks.clear();
            endRemoveRows();
        });
    }

    QStringList QueueModel::getTracks(const QVector<int>& indexes)
    {
        QStringList tracks;
        tracks.reserve(indexes.size());
        for (int index : indexes) {
            tracks.append(mTracks.at(index)->filePath);
        }
        return tracks;
    }

    QHash<int, QByteArray> QueueModel::roleNames() const
    {
        return {{FilePathRole, "filePath"},
                {TitleRole, "title"},
                {ArtistRole, "artist"},
                {DisplayedArtistRole, "displayedArtist"},
                {AlbumRole, "album"},
                {DisplayedAlbumRole, "displayedAlbum"},
                {DurationRole, "duration"}};
    }
}
