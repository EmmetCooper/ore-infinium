import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import QtQuick.Layouts 1.0

import "controls"

Item {
    id: main

    signal joinClicked()
    signal hostClicked()
    focus: true

   Keys.onEscapePressed: {
        main.Stack.view.pop()
   }

    Component.onCompleted: {
    }

    OreLabel {
        anchors {
            horizontalCenter: mainButtonLayout.horizontalCenter
            top: parent.top
            topMargin: 100
        }

       text: "Multiplayer"
       font.pixelSize: 40
    }

    ColumnLayout {
        id: mainButtonLayout

        anchors.centerIn: parent

        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            onClicked: {
                hostClicked()
            }

            text: "Host a Game"
        }

        OreButton {
            Layout.fillWidth: true
            Layout.fillHeight: true

            text: "Join a Game"

            onClicked: {
                joinClicked()
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
            main.Stack.view.pop()
        }

        text: "Back"
    }
}
