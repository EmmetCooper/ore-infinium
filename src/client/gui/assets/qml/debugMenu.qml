import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import QtQuick.Layouts 1.0

Rectangle {
    id: main

    color: "red"
    opacity: 0.5

    Component.onCompleted: {
    }

    ColumnLayout {

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.top
        }

        OreLabel {
            text: "playerPosition: " + ClientBackend.playerPosition
        }

            OreLabel {
            text: "-" //"playerPosition: " + ClientBackend.playerPosition
        }
    }
}
