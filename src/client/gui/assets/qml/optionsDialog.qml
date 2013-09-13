import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import QtQuick.Layouts 1.0

    Window {
        id: win
           x: 100
        y: 100

//        width: 100
//        height: 100

        //minHeight: label.contentHeight
        //minWidth: label.contentWidth
        Text {
        id: label
        color: "black"
        wrapMode: Text.Wrap//Anywhere
        text: "OPTIONS DIALOG!"

        anchors.fill: parent
        //        anchors.fill: rect
        //        anchors.right: parent.right
        //        anchors.left: parent.left
        //        anchors.bottom: parent.bottom
        //        height: 100
        }

        Button {
            style: buttonStyle

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }

            text: "options dialog"
        }
    }
