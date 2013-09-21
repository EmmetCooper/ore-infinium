import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

Button {
    id: button

//    property alias pixelSize: label.font.pixelSize;
    property int fontSize: 20

    style: ButtonStyle {
        id: oreButtonStyle
        panel:
        Item {
            id: styleItem

            implicitHeight: label.implicitHeight
            implicitWidth: label.implicitWidth

            BorderImage {
                id: borderImage

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
                    id: label

                    text: control.text
                    anchors.centerIn: parent
                    color: "white"
                    font.pixelSize: fontSize
                    renderType: Text.NativeRendering
                }
            }
        }
    }
}
