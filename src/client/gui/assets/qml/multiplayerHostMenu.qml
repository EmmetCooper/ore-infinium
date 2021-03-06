import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import QtQuick.Layouts 1.0

import "controls"

Item {
    id: main

    Component.onCompleted: {

    }

    Keys.onEscapePressed: {
        main.Stack.view.pop()
    }

    focus: true

    OreLabel {
        anchors {
            horizontalCenter: mainButtonLayout.horizontalCenter
            top: parent.top
            topMargin: 100
        }

       text: "Multiplayer, host a game"
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

            OreLabel {
                id: portLabel
                text: "Port:"
            }

            OreTextBox {
                id: port

                text: ClientBackend.defaultPort()
            }
        }

        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            onClicked: {
                ClientBackend.startMultiplayerHostSlot(playerName.text, parseInt(port.text));
                main.Stack.view.clear();
            }

            text: "Play!"
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

            main.Stack.view.pop()
        }

        text: "Back"
    }
}
