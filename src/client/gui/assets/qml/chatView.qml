import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.0

Item {
    property alias text: chatViewText.text

    Rectangle {
        color: "black"

        anchors {
            left: parent.left
            top: parent.top
//            bottom: parent.bottom
        }

        height: 300
        width: 500

        TextEdit {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: chatTextBox.bottom
            }

            id: chatViewText

            wrapMode: TextEdit.WrapAnywhere
            readOnly: true
            selectByMouse: true
            selectByKeyboard: true

            color: "white"
        }

        OreTextBox {
            anchors {
                left: parent.left
                bottom: parent.bottom
          //      top: chatViewText.bottom
            }

            id: chatTextBox

            text: "EDITABLE!"
        }

        OreButton {
            anchors {
                left: chatTextBox.right
                right: parent.right
                top: chatTextBox.top
                bottom: chatTextBox.bottom

                leftMargin: 10
                rightMargin: 10
            }

            text: "Send"
        }
    }
}