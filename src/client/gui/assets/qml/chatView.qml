import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.0

import "controls"

Item {
    property alias text: chatViewText.text

    height: 300
    width: 500

    Rectangle {
        color: "black"

        anchors {
            left: parent.left
            right: parent.right

            top: parent.top
            bottom: parent.bottom
        }

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
        }

        OreButton {
            id: send

            anchors {
                left: chatTextBox.right
                right: parent.right
                top: chatTextBox.top
                bottom: chatTextBox.bottom

                leftMargin: 10
                rightMargin: 10
            }

            onClicked: {
                ClientBackend.chatSendClicked(chatTextBox.text)
            }

            text: "Send"
        }
    }
}
