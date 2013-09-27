import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import OptionsDialogBackend 1.0

Window {
    id: win
    x: 100
    y: 100

    focus: true

    //FIXME: doesn't work..
    Keys.onEscapePressed: {
        win.visible = false

    }

    minWidth: 200
    minHeight: 200

    width: 300
    height: 300

    //minHeight: label.contentHeight
    //minWidth: label.contentWidth
    OptionsDialogBackend {
        id: backend

    }

    ColumnLayout {
        id: mainOptionsColumnLayout

        anchors {
            left: winContents.left
            right: winContents.right
            top: winContents.top
        }

        OreLabel {
            id: playerNameLabel

            anchors {
                left: parent.left
                verticalCenter: playerNameTextBox.verticalCenter
            }

            text: "Player Name"
        }

        OreTextBox {
            id: playerNameTextBox

            anchors {
                top: parent.top
                left: playerNameLabel.right
                right: parent.right
            }

            text: "test"
        }

        OreLabel {
            id: resolutionLabel

            anchors {
                left: parent.left
            }

            text: "Resolution"
        }

        ComboBox {
            id: resolutionComboBox
            style: OreComboBoxStyle {}

            anchors {
                left: resolutionLabel.right
                right: parent.right
                verticalCenter: resolutionLabel.verticalCenter
            }

            model: ["a", "b", "c"]
        }
    } //column

    RowLayout {
        id: mainoptionsRowLayout

        anchors {
            right: winContents.right
            top: mainOptionsColumnLayout.bottom
            bottom: winContents.bottom

            rightMargin: 25
            bottomMargin: 15
        }

        //dialog functionality
        OreButton {
            id: okButton

            anchors {
                rightMargin: 5
                bottom: parent.bottom
            }

            text: "OK"
            onClicked: {
                backend.finalizeSettings()
                closed(true)
            }
        }

        OreButton {
            id: cancelButton
            anchors {
                leftMargin: 5
                left: okButton.right
                bottom: parent.bottom
            }

            text: "Cancel"
            onClicked: {
                //close
                closed(false)
            }
        }
    }
}
