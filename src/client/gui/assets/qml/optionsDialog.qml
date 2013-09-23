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

        RowLayout {
            anchors {
                left: parent.left
                right: parent.right
            }

            OreLabel {
                id: label
                text: "OPTIONS DIALOG!"
            }

            OreTextBox {

                text: "options dialog"
            }
        }

        RowLayout {
            anchors {
                left: parent.left
                right: parent.right
            }

            OreLabel {
                id: label2
                text: "OPTIONS DIALOG!"
            }

            OreTextBox {

                text: "options dialog"
            }
        } // rowlayout
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
