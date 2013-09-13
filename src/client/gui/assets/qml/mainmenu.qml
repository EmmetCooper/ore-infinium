import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import OpenGLUnderQML 1.0
import QtQuick.Layouts 1.0

//property alias label: text.text

Item {
    OptionsDialog {
        width: 200
        height: 200
        x: 300
        y: 300
    }


    ColumnLayout {
        anchors.centerIn: parent

        Button {
            style: buttonStyle

            text: "test"
        }
                Button {
            style: buttonStyle

            text: "test"
        }
                Button {
            style: buttonStyle

            text: "test"
        }
                Button {
            style: buttonStyle

            text: "test"
        }
    }

    Window {
        x: 100
        y: 100
//       anchors.fill: parent

//        width: 100
//        height: 100

        //minHeight: label.contentHeight
        //minWidth: label.contentWidth
        Text {
        id: label
        color: "black"
        wrapMode: Text.Wrap//Anywhere
        text: "HERE IS SOME TEST TEXT. hopefully it'll know that it needs to expand the contents of the window and not fuck up, autosensing the size........................................................................blah"

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

            text: "test"
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
        id: buttonStyle

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

                source: {
                    if (control.pressed) {
                        "../button-pressed.png"
                    } else if (control.hovered) {
                        "../button-hovered.png"
                    } else {
                        "../button-normal.png"
                    }
                }


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
