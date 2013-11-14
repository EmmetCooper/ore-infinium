import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import QtQuick.Layouts 1.0

Item {
    id: main
    width: 1024
    height: 768

    focus: true

    signal disconnectClicked()

    Keys.onEscapePressed: {
        print("ESCP MENU, key pressed, setting visible to false, clearing stack");
        ClientBackend.setEscapeMenuVisible(false);
        main.Stack.view.pop(null)
        print("done");
    }

    Component.onCompleted: {
    }

    Loader {
        id: optionsLoader
        parent: main
//        anchors.fill: mainMenu
//        anchors.centerIn: parent
        z: 200

        onLoaded: {
            item.x = main.width * 0.5 - (item.width * 0.5)
            item.y = main.height * 0.5 - (item.height * 0.5)
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
                main.Stack.view.clear()
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
