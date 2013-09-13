import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import OpenGLUnderQML 1.0
import QtQuick.Layouts 1.0

//property alias label: text.text

Item {
    ColumnLayout {
        anchors.centerIn: parent

        Button {
            style: buttonStyle

            text: "mainmenu"
        }

        Button {
            style: buttonStyle

            text: "mainmenu"
        }

        Button {
            style: buttonStyle

            text: "Options"
        }

        Button {
            style: buttonStyle

            text: "Exit"
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

}
