import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import OpenGLUnderQML 1.0
import QtQuick.Layouts 1.0

Item {
    id: singleplayerCreateMenu

    Component.onCompleted: {

    }

    OreLabel {
        anchors {
            horizontalCenter: mainButtonLayout.horizontalCenter
            top: parent.top
            topMargin: 100
        }

       text: "Singleplayer, create a world"
       font.pixelSize: 40
    }

    ColumnLayout {
        id: mainButtonLayout

        anchors.centerIn: parent

        RowLayout {
            Layout.fillHeight: true

            anchors {
                left: parent.left
                right: parent.right
            }

            OreLabel {
                id: playerNameLabel

                text: "Player Name"
            }

            OreTextBox {
                id: playerName

                text: "Player123123kl"
            }
        }

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

            singleplayerCreateMenu.Stack.view.pop()
        }

        text: "Back"
    }
}
