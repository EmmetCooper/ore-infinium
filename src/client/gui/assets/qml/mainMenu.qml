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
//        anchors.fill: mainMenu
        anchors.centerIn: parent
        z: 200
    }

    ColumnLayout {
        anchors.centerIn: parent


        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "mainmenu"
        }

        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "mainmenu"
        }

        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "Options"

            onClicked: {
                optionsLoader.source = "optionsDialog.qml"
            }
        }

        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "Exit"
        }
    }
}
