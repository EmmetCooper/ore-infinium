
import QtQuick 2.0
import OpenGLUnderQML 1.0

Item {

    //FIXME: gynormous hack, strip win size out into C++
    width: 1024
    height: 768

    Squircle {
        SequentialAnimation on t {
            NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
            NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
            loops: Animation.Infinite
            running: true
        }
    }
    Rectangle {
        id: rect
        color: Qt.rgba(1, 1, 1, 0.7)
        radius: 10
        border.width: 1
        border.color: "white"
        opacity: 0.5
//        anchors.fill: label

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right

        }

        height: 100

        anchors.bottomMargin: 30


    }

    Text {
        id: label
        color: "black"
        wrapMode: Text.WordWrap
        text: "The background here is a squircle rendered with raw OpenGL using the 'beforeRender()' signal in QQuickWindow. This text label and its border is rendered using QML"
        anchors.fill: rect
//        anchors.right: parent.right
//        anchors.left: parent.left
//        anchors.bottom: parent.bottom
//        height: 100
    }
}
