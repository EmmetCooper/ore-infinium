import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import OpenGLUnderQML 1.0
import QtQuick.Layouts 1.0

Item {
    id: singleplayerMenu

   signal createWorldClicked()

    Component.onCompleted: {
    }

    OreLabel {
        anchors {
            horizontalCenter: mainButtonLayout.horizontalCenter
            top: parent.top
            topMargin: 100
        }

       text: "Singleplayer"
       font.pixelSize: 40
    }

    ColumnLayout {
        id: mainButtonLayout

        anchors.centerIn: parent

        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            onClicked: {
                createWorldClicked()
            }

            text: "Create World"
        }

        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "Load World"

            onClicked: {
                optionsLoader.source = "optionsDialog.qml"
            }
        }
    } // column layout

    OreButton {
        id: backButton

        anchors {
            left: parent.left
            bottom: parent.bottom
            bottomMargin: 50
            leftMargin: 50
        }

        width: 50
        height: 50

        onClicked: {
            singleplayerMenu.Stack.view.pop()
        }

        text: "Back"
    }
}
