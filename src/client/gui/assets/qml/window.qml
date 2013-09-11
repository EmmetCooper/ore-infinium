import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import OpenGLUnderQML 1.0

Item {
    id: win
    width: 150
    height: 150

    property int minWidth: 100
    property int minHeight: 100

//    anchors.bottomMargin: 30
    anchors {
        bottomMargin: 10
        topMargin: 10
        leftMargin: 10
        rightMargin: 10
    }

    Rectangle {
        id: contents
        color: Qt.rgba(1, 1, 1, 0.7)
        radius: 10
        border.width: 1
        border.color: "white"
        opacity: 0.8

        anchors {
            bottomMargin: -10
            topMargin: -10
            leftMargin: -10
            rightMargin: -10
        }

        anchors.fill: parent

        MouseArea {
            id: bottomRightDragMouseArea
            anchors {
                bottom: parent.bottom
                right: parent.right
            }

            Rectangle {
                id: bottomRightDragHandle
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
                win.anchors.left = undefined
                win.anchors.right = undefined
                win.anchors.bottom = undefined
                win.anchors.top = undefined
                oldMouseX = mouseX
                oldMouseY = mouseY
            }

            onPositionChanged: {
                if (pressed) {
                    var mouseXDelta = (mouseX - oldMouseX)
                    var mouseYDelta = (mouseY - oldMouseY)

                    if (win.width + mouseXDelta >= minWidth) {
                        win.width += mouseXDelta
                    }

                    if (win.height + mouseYDelta >= minHeight) {
                        win.height += mouseYDelta
                    }

                    //if (win.height >= minHeight) {
                    //    win.height += mouseYDelta
                    //} else if (win.height < minHeight) {
                    //    win.height = minHeight
                    //}
                } // pos changed

                cursorShape = Qt.SizeFDiagCursor
            }
        } // mouse area
    } //rectangle

    Rectangle {
        id: titlebar

        color: "yellow"
        opacity: 1

        height: 20
        width: 100

        anchors {
            left: contents.left
            right: contents.right
            bottom: contents.top
        }

        MouseArea {
            id: dragHandle

            anchors.fill: parent

            hoverEnabled: true

            drag.target: win
        }
    }
}
