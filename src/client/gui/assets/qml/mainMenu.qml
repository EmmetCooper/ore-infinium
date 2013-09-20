import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import OpenGLUnderQML 1.0
import QtQuick.Layouts 1.0

//property alias label: text.text

Item {
    id: mainMenu

    Loader {
        id: optionsLoader
        parent: mainMenu
        anchors.fill: mainMenu
        z: 200
    }

    ColumnLayout {
        anchors.centerIn: parent

        OreButton {
            text: "mainmenu"
        }

        OreButton {
            text: "mainmenu"
        }

        OreButton {
            text: "Options"

            onClicked: {
                optionsLoader.source = "optionsDialog.qml"
            }
        }

        OreButton {
            text: "Exit"
        }
    }
}
