import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import OpenGLUnderQML 1.0

Item {
           Rectangle {
            id: rect
            color: Qt.rgba(1, 1, 1, 0.7)
            radius: 10
            border.width: 1
            border.color: "white"
    //        opacity: 0.5
    //        anchors.fill: label

            height: 500
            width: 500
            anchors.bottomMargin: 30

           // anchors {
           //     bottom: parent.bottom
           //     left: parent.left
           //     right: parent.right
           //
           // }

            MouseArea {
                id: mouseArea
                anchors {
                   bottom: parent.bottom
                   right: parent.right
                }

                Rectangle {
                    anchors.fill: parent
                    color: "red"
                }

                width: 30
                height: 30

                hoverEnabled: true

                acceptedButtons: Qt.LeftButton

                property int oldMouseX
                property int oldMouseY

                onPressed: {
                    parent.anchors.left = undefined
                    parent.anchors.right = undefined
                    parent.anchors.bottom = undefined
                    parent.anchors.top = undefined
                    oldMouseX = mouseX
                    oldMouseY = mouseY
                }

                onPositionChanged: {
                    if (pressed) {
                        parent.width += (mouseX - oldMouseX)
                        parent.height += (mouseY - oldMouseY)
                    }

                    cursorShape = Qt.SizeFDiagCursor
                }
            }

            Button {
    //            style: touchstyle

            anchors {
                left: parent.left
                right: parent.horizontalcenter
                top: parent.top
                bottom: parent.bottom
            }

            text: "test"
            onClicked: {
                text = "clicked!"
            }
            }
        }



}
