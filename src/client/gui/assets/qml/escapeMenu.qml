import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import QtQuick.Layouts 1.0

Item {
    id: escapeMenu
    width: 1024
    height: 768

    focus: true

    signal disconnectClicked()

    Keys.onEscapePressed: {
    }

    Component.onCompleted: {
    }

    Loader {
        id: optionsLoader
        parent: escapeMenu
//        anchors.fill: mainMenu
//        anchors.centerIn: parent
        z: 200

        onLoaded: {
            item.x = escapeMenu.width * 0.5 - (item.width * 0.5)
            item.y = escapeMenu.height * 0.5 - (item.height * 0.5)
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

       text: "Ore Infinium (in game)"
       font.pixelSize: 40
    }

    ColumnLayout {
        id: mainButtonLayout

        anchors.centerIn: parent

        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            onClicked: {
                escapeMenu.Stack.view.pop()
            }

            text: "Resume"
        }

        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            onClicked: {
                disconnectClicked()
            }

            text: "Disconnect"
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
