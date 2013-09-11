import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import OpenGLUnderQML 1.0

//property alias label: text.text

Item {

    Window {
        x: 100
        y: 100
//       anchors.fill: parent

//        width: 100
//        height: 100
        height: label.implicitHeight
        Text {
        id: label
        color: "black"
        wrapMode: Text.WrapAnywhere
        text: "HERE IS SOME TEST TEXT. hopefully it'll know that it needs to expand the contents of the window and not fuck up, autosensing the size........................................................................blah"

        anchors.fill: parent
        //        anchors.fill: rect
        //        anchors.right: parent.right
        //        anchors.left: parent.left
        //        anchors.bottom: parent.bottom
        //        height: 100
        }
    }

    //FIXME: gynormous hack, strip win size out into C++
    width: 1024
    height: 768

    Client {
        SequentialAnimation on t {
            NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
            NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
            loops: Animation.Infinite
            running: true
        }

    }

    Component {
        id: touchStyle
        ButtonStyle {
            panel: Item {
                implicitHeight: 50
                implicitWidth: 320
                BorderImage {
                    anchors.fill: parent
                    antialiasing: true
                    border.bottom: 8
                    border.top: 8
                    border.left: 8
                    border.right: 8
                    anchors.margins: control.pressed ? -4 : 0
   //                 source: control.pressed ? "../images/button_pressed.png" : "../images/button_default.png"
                    Text {
                        text: control.text
                        anchors.centerIn: parent
                        color: "white"
                        font.pixelSize: 23
                        renderType: Text.NativeRendering
                    }
                }
            }
        }
    }


    /*
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
            */
}
