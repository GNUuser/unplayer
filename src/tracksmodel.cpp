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

#include "tracksmodel.h"

#include <QCoreApplication>
#include <QDebug>
#include <QSqlDriver>
#include <QSqlError>
#include <QUrl>

#include "settings.h"

namespace unplayer
{
    namespace
    {
        enum Field
        {
            FilePathField,
            TitleField,
            ArtistField,
            AlbumField,
            DurationField
        };
    }

    TracksModel::~TracksModel()
    {
        if (mAllArtists) {
            Settings::instance()->setAllTracksSortSettings(mSortDescending, mSortMode, mInsideAlbumSortMode);
        } else {
            if (mAllAlbums) {
                Settings::instance()->setArtistTracksSortSettings(mSortDescending, mSortMode, mInsideAlbumSortMode);
            } else {
                Settings::instance()->setAlbumTracksSortSettings(mSortDescending, mInsideAlbumSortMode);
            }
        }
    }

    void TracksModel::componentComplete()
    {
        if (mAllArtists) {
            mSortDescending = Settings::instance()->allTracksSortDescending();
            mSortMode = static_cast<SortMode>(Settings::instance()->allTracksSortMode(SortMode::ArtistAlbumYear));
            mInsideAlbumSortMode = static_cast<InsideAlbumSortMode>(Settings::instance()->allTracksInsideAlbumSortMode(InsideAlbumSortMode::TrackNumber));
        } else {
            if (mAllAlbums) {
                mSortDescending = Settings::instance()->artistTracksSortDescending();
                mSortMode = static_cast<SortMode>(Settings::instance()->artistTracksSortMode(SortMode::ArtistAlbumYear));
                mInsideAlbumSortMode = static_cast<InsideAlbumSortMode>(Settings::instance()->artistTracksInsideAlbumSortMode(InsideAlbumSortMode::TrackNumber));
            } else {
                mSortDescending = Settings::instance()->albumTracksSortDescending();
                mSortMode = SortMode::ArtistAlbumTitle;
                mInsideAlbumSortMode = static_cast<InsideAlbumSortMode>(Settings::instance()->albumTracksSortMode(InsideAlbumSortMode::TrackNumber));
            }
        }

        emit sortModeChanged();
        emit insideAlbumSortModeChanged();

        setQuery();
    }

    QVariant TracksModel::data(const QModelIndex& index, int role) const
    {
        mQuery->seek(index.row());

        switch (role) {
        case FilePathRole:
            return mQuery->value(FilePathField);
        case TitleRole:
            return mQuery->value(TitleField);
        case ArtistRole:
        {
            const QString artist(mQuery->value(ArtistField).toString());
            if (artist.isEmpty()) {
                return qApp->translate("unplayer", "Unknown artist");
            }
            return artist;
        }
        case AlbumRole:
        {
            const QString album(mQuery->value(AlbumField).toString());
            if (album.isEmpty()) {
                return qApp->translate("unplayer", "Unknown album");
            }
            return album;
        }
        case DurationRole:
            return mQuery->value(DurationField);
        default:
            return QVariant();
        }
    }

    bool TracksModel::allArtists() const
    {
        return mAllArtists;
    }

    void TracksModel::setAllArtists(bool allArtists)
    {
        mAllArtists = allArtists;
    }

    bool TracksModel::allAlbums() const
    {
        return mAllAlbums;
    }

    void TracksModel::setAllAlbums(bool allAlbums)
    {
        mAllAlbums = allAlbums;
    }

    const QString& TracksModel::artist() const
    {
        return mArtist;
    }

    void TracksModel::setArtist(const QString& artist)
    {
        mArtist = artist;
    }

    const QString& TracksModel::album() const
    {
        return mAlbum;
    }

    void TracksModel::setAlbum(const QString& album)
    {
        mAlbum = album;
    }

    const QString& TracksModel::genre() const
    {
        return mGenre;
    }

    void TracksModel::setGenre(const QString& genre)
    {
        mGenre = genre;
    }

    bool TracksModel::sortDescending() const
    {
        return mSortDescending;
    }

    void TracksModel::setSortDescending(bool descending)
    {
        if (descending != mSortDescending) {
            mSortDescending = descending;
            setQuery();
        }
    }

    TracksModel::SortMode TracksModel::sortMode() const
    {
        return mSortMode;
    }

    void TracksModel::setSortMode(TracksModel::SortMode mode)
    {
        if (mode != mSortMode) {
            mSortMode = mode;
            emit sortModeChanged();
            setQuery();
        }
    }

    TracksModel::InsideAlbumSortMode TracksModel::insideAlbumSortMode() const
    {
        return mInsideAlbumSortMode;
    }

    void TracksModel::setInsideAlbumSortMode(TracksModel::InsideAlbumSortMode mode)
    {
        if (mode != mInsideAlbumSortMode) {
            mInsideAlbumSortMode = mode;
            emit insideAlbumSortModeChanged();
            setQuery();
        }
    }

    QStringList TracksModel::getTracks(const QVector<int>& indexes)
    {
        QStringList tracks;
        tracks.reserve(indexes.size());
        for (int index : indexes) {
            mQuery->seek(index);
            tracks.append(mQuery->value(FilePathField).toString());
        }
        return tracks;
    }

    QHash<int, QByteArray> TracksModel::roleNames() const
    {
        return {{FilePathRole, "filePath"},
                {TitleRole, "title"},
                {ArtistRole, "artist"},
                {AlbumRole, "album"},
                {DurationRole, "duration"}};
    }

    void TracksModel::setQuery()
    {
        beginResetModel();

        QString query(QLatin1String("SELECT filePath, title, artist, album, duration FROM tracks "));

        if (mAllArtists) {
            if (!mGenre.isEmpty()) {
                query += QLatin1String("WHERE genre = ? ");
            }
            query += QLatin1String("GROUP BY id, artist, album ");
        } else {
            query += QLatin1String("WHERE artist = ? ");
            if (mAllAlbums) {
                query += QLatin1String("GROUP BY id, album ");
            } else {
                query += QLatin1String("AND album = ? "
                                       "GROUP BY id ");
            }
        }

        switch (mSortMode) {
        case SortMode::Title:
            query += QLatin1String("ORDER BY title %1");
            break;
        case SortMode::AddedDate:
            query += QLatin1String("ORDER BY id %1");
            break;
        case SortMode::ArtistAlbumTitle:
            query += QLatin1String("ORDER BY artist = '' %1, artist %1, album = '' %1, album %1, ");
            break;
        case SortMode::ArtistAlbumYear:
            query += QLatin1String("ORDER BY artist = '' %1, artist %1, album = '' %1, year %1, album %1, ");
            break;
        }

        if (mSortMode == SortMode::ArtistAlbumTitle ||
                mSortMode == SortMode::ArtistAlbumYear) {
            switch (mInsideAlbumSortMode) {
            case InsideAlbumSortMode::Title:
                query += QLatin1String("title %1");
                break;
            case InsideAlbumSortMode::TrackNumber:
                query += QLatin1String("trackNumber %1, title %1");
                break;
            }
        }

        query = query.arg(mSortDescending ? QLatin1String("DESC")
                                          : QLatin1String("ASC"));

        mQuery->prepare(query);

        if (mAllArtists) {
            if (!mGenre.isEmpty()) {
                mQuery->addBindValue(mGenre);
            }
        } else {
            mQuery->addBindValue(mArtist);
            if (!mAllAlbums) {
                mQuery->addBindValue(mAlbum);
            }
        }

        execQuery();

        endResetModel();
    }
}
