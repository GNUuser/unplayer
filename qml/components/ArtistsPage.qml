/*
 * Unplayer
 * Copyright (C) 2015-2017 Alexey Rochev <equeim@gmail.com>
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

import QtQuick 2.2
import Sailfish.Silica 1.0

import harbour.unplayer 0.1 as Unplayer

Page {
    SearchPanel {
        id: searchPanel
    }

    SelectionPanel {
        id: selectionPanel
        selectionText: qsTr("%n artist(s) selected", String(), artistsProxyModel.selectedIndexesCount)

        PushUpMenu {
            MenuItem {
                enabled: artistsProxyModel.hasSelection
                text: qsTr("Add to queue")
                onClicked: {
                    player.queue.addTracks(artistsModel.getTracksForArtists(artistsProxyModel.selectedSourceIndexes))
                    selectionPanel.showPanel = false
                }
            }

            MenuItem {
                enabled: artistsProxyModel.hasSelection
                text: qsTr("Add to playlist")
                onClicked: pageStack.push(addToPlaylistPage)

                Component {
                    id: addToPlaylistPage

                    AddToPlaylistPage {
                        tracks: artistsModel.getTracksForArtists(artistsProxyModel.selectedSourceIndexes)
                        Component.onDestruction: {
                            if (added) {
                                selectionPanel.showPanel = false
                            }
                        }
                    }
                }
            }
        }
    }

    SilicaListView {
        id: listView

        anchors {
            fill: parent
            bottomMargin: selectionPanel.visible ? selectionPanel.visibleSize : 0
            topMargin: searchPanel.visibleSize
        }
        clip: true

        header: PageHeader {
            title: qsTr("Artists")
        }
        delegate: MediaContainerSelectionDelegate {
            id: artistDelegate

            title: Theme.highlightText(model.displayedArtist, searchPanel.searchText, Theme.highlightColor)
            description: qsTr("%n album(s)", String(), model.albumsCount)
            mediaArt: Unplayer.LibraryUtils.randomMediaArtForArtist(model.artist)
            menu: Component {
                ContextMenu {
                    MenuItem {
                        text: qsTr("All tracks")
                        onClicked: pageStack.push(tracksPageComponent)

                        Component {
                            id: tracksPageComponent
                            TracksPage {
                                pageTitle: model.displayedArtist
                                allArtists: false
                                artist: model.artist
                            }
                        }
                    }

                    MenuItem {
                        text: qsTr("Add to queue")
                        onClicked: player.queue.addTracks(artistsModel.getTracksForArtist(artistsProxyModel.sourceIndex(model.index)))
                    }

                    MenuItem {
                        text: qsTr("Add to playlist")
                        onClicked: pageStack.push("AddToPlaylistPage.qml", { tracks: artistsModel.getTracksForArtist(artistsProxyModel.sourceIndex(model.index)) })
                    }
                }
            }

            onClicked: {
                if (selectionPanel.showPanel) {
                    artistsProxyModel.select(model.index)
                } else {
                    pageStack.push(artistPageComponent)
                }
            }

            Component {
                id: artistPageComponent
                ArtistPage {
                    displayedArtist: model.displayedArtist
                    artist: model.artist
                    albumsCount: model.albumsCount
                    tracksCount: model.tracksCount
                    duration: model.duration
                    mediaArt: artistDelegate.mediaArt
                }
            }

            Connections {
                target: Unplayer.LibraryUtils
                onMediaArtChanged: mediaArt = Unplayer.LibraryUtils.randomMediaArtForArtist(model.artist)
            }
        }
        model: Unplayer.FilterProxyModel {
            id: artistsProxyModel

            filterRole: Unplayer.ArtistsModel.DisplayedArtistRole
            sourceModel: Unplayer.ArtistsModel {
                id: artistsModel
            }
        }

        PullDownMenu {
            SelectionMenuItem {
                text: qsTr("Select artists")
            }

            SearchMenuItem { }
        }

        ListViewPlaceholder {
            text: qsTr("No artists")
        }

        VerticalScrollDecorator { }
    }
}
