/*
 * Unplayer
 * Copyright (C) 2015-2019 Alexey Rochev <equeim@gmail.com>
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

Page {
    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column

            anchors {
                left: parent.left
                leftMargin: Theme.horizontalPageMargin
                right: parent.right
                rightMargin: Theme.horizontalPageMargin
            }

            PageHeader {
                title: qsTranslate("unplayer", "About")
            }

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                asynchronous: true
                source: {
                    var iconSize = Theme.iconSizeExtraLarge
                    if (iconSize < 108) {
                        iconSize = 86
                    } else if (iconSize < 128) {
                        iconSize = 108
                    } else if (iconSize < 256) {
                        iconSize = 128
                    } else {
                        iconSize = 256
                    }
                    return "/usr/share/icons/hicolor/%1x%2/apps/harbour-unplayer.png".arg(iconSize).arg(iconSize)
                }
            }

            Item {
                width: parent.width
                height: Theme.paddingMedium
            }

            Label {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: Theme.fontSizeLarge
                text: "Unplayer %1".arg(Qt.application.version)
            }

            Label {
                horizontalAlignment: Text.AlignHCenter
                text: qsTranslate("unplayer", "Simple music player for Sailfish OS")
                width: parent.width
                wrapMode: Text.WordWrap
            }

            Label {
                horizontalAlignment: implicitWidth > width ? Text.AlignLeft : Text.AlignHCenter
                width: parent.width
                font.pixelSize: Theme.fontSizeExtraSmall
                text: "<style type=\"text/css\">A { color: %1; }</style>".arg(Theme.highlightColor) +
                      "\u00a9 2015-2019 Alexey Rochev &lt;<a href=\"mailto:equeim@gmail.com\">equeim@gmail.com</a>&gt;"
                textFormat: Text.RichText
                truncationMode: TruncationMode.Fade
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Item {
                width: parent.width
                height: Theme.paddingLarge
            }

            Column {
                width: parent.width
                spacing: Theme.paddingMedium

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: Theme.buttonWidthLarge
                    text: qsTranslate("unplayer", "Source Code")
                    onClicked: Qt.openUrlExternally("https://github.com/equeim/unplayer")
                }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: Theme.buttonWidthLarge
                    text: qsTranslate("unplayer", "Translations")
                    onClicked: Qt.openUrlExternally("https://hosted.weblate.org/projects/unplayer/translations")
                }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: Theme.buttonWidthLarge
                    text: qsTranslate("unplayer", "Donate")
                    onClicked: pageStack.push("DonatePage.qml")
                }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: Theme.buttonWidthLarge
                    text: qsTranslate("unplayer", "Authors")
                    onClicked: pageStack.push("AuthorsPage.qml")
                }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: Theme.buttonWidthLarge
                    text: qsTranslate("unplayer", "Translators")
                    onClicked: pageStack.push("TranslatorsPage.qml")
                }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: Theme.buttonWidthLarge
                    text: qsTranslate("unplayer", "License")
                    onClicked: pageStack.push("LicensePage.qml")
                }
            }
        }

        VerticalScrollDecorator { }
    }
}
