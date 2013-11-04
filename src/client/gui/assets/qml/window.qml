import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Item {
    id: win
    width: 150
    height: 150

    clip: true


    /**
     * true if accepted, false if cancelled
     */
    signal closed(bool accepted);

    property alias winContents: contents
    property alias title: winTitle
    property int minWidth: 100
    property int minHeight: 100

    anchors {
        bottomMargin: 0
        topMargin: 10
        leftMargin: 20
        rightMargin: 10
    }

    focus: true

    Rectangle {
        id: titlebar

        color: "yellow"
        opacity: 1

        height: 20
        width: 100

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        Text {
            id: winTitle
            text: "Amazing Window"

            anchors.centerIn: parent
            renderType: Text.NativeRendering
        }

        MouseArea {
            id: dragHandle

            anchors.fill: parent

            hoverEnabled: true

            onPositionChanged: {
            }

            drag.target: win
        }
    } // titlebar

    Rectangle {
        id: contents
        color: Qt.rgba(1, 1, 1, 0.7)
        radius: 10
        border.width: 1
        border.color: "white"
        opacity: 0.8

        anchors {
//            leftMargin: 10
 //           rightMargin: -10

            bottom: parent.bottom
            top: titlebar.bottom
            left: parent.left
            right: parent.right
        }

        MouseArea {
            anchors.fill: parent
            focus: true
            hoverEnabled: true
        }

        MouseArea {
            id: bottomRightDragMouseArea
            anchors {
                bottom: parent.bottom
                right: parent.right
            }

            z: 1000

            Rectangle {
                id: bottomRightDragHandle
                anchors.fill: parent
                color: "red"
            }

            width: 10
            height: 10

            hoverEnabled: true

            acceptedButtons: Qt.LeftButton

            property int oldMouseX
            property int oldMouseY

            onPressed: {
                win.z = 900
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
                } // pos changed

                cursorShape = Qt.SizeFDiagCursor
            }
        } // mouse area
    } //rectangle contents
}
