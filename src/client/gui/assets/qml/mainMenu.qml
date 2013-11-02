import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import QtQuick.Layouts 1.0

//property alias label: text.text

Item {
    id: mainMenu
    width: 1024
    height: 768

    signal singleplayerClicked()
    signal multiplayerClicked()

    focus: true

    Keys.onEscapePressed: {
        exit.clicked()
    }

    Component.onCompleted: {
    }

    Loader {
        id: optionsLoader
        parent: mainMenu
//        anchors.fill: mainMenu
//        anchors.centerIn: parent
        z: 200

        onLoaded: {
            item.x = mainMenu.width * 0.5 - (item.width * 0.5)
            item.y = mainMenu.height * 0.5 - (item.height * 0.5)
        }

        Connections {
            property var options: optionsLoader.item

            target: optionsLoader.item

            onClosed: {
                optionsLoader.source = ""
            }
        }
    }

    OreLabel {
        anchors {
            horizontalCenter: mainButtonLayout.horizontalCenter
            bottom: mainButtonLayout.top
            bottomMargin: 100
        }

       text: "Ore Infinium"
       font.pixelSize: 40
    }

    ColumnLayout {
        id: mainButtonLayout

        anchors.centerIn: parent

        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            onClicked: {
                singleplayerClicked()
            }

            text: "Singleplayer"
        }

        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            onClicked: {
                multiplayerClicked()
            }

            text: "Multiplayer"
        }

        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "Options"

            onClicked: {
                optionsLoader.source = "optionsDialog.qml"
                optionsLoader.item.focus = true
            }
        }

        OreButton {
            id: exit
            Layout.fillWidth: true
            Layout.fillHeight: true

            onClicked: {
                ClientBackend.exitClicked()
            }

            text: "Exit"
        }
    }
}
