import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Item {

    property alias text: button.text
    Button {
        id: button

        style: buttonStyle
    }

    ButtonStyle {
        id: buttonStyle
        panel:
        Item {
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
