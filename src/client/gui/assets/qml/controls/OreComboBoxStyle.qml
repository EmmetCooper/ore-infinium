import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
 //   style: ComboBoxStyle
//    {

ComboBoxStyle {
    panel: Item {
        implicitWidth: 200
        implicitHeight: bg.height + popup.height
        property bool popup: false
        BorderImage {
            id: bg
            width: parent.width
            source: "../../button-normal.png"
            border.left: 5; border.top: 5
            border.right: 32; border.bottom: 5
            asynchronous: true
            Text {
                id: txt
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 10
                text: control.currentText
                color: "#393e41"
                font.bold: true
            }
        }
        Rectangle {
            id: popup
            anchors.top: bg.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            visible: control.__pressed
            height: control.__pressed ? column.height : 0
            width: parent.width -10
            color: "#a6afb6"
            clip: true
            radius: 5
            Behavior on height {NumberAnimation {duration: 250}}
            Column {
                id: column
                width: parent.width
                Repeater {
                    model: control.items
                    delegate: Rectangle {
                        id: del
                        color: mArea.containsMouse ? "#d2d7da" : "#a6afb6"
                        width: popup.width
                        height: 30
                        radius: 5
                        Text { id: title;
                            anchors.verticalCenter: parent.verticalCenter;
                            anchors.left: parent.left
                            anchors.leftMargin: 5
                            text: model.text
                            color: "#393e41"
                            font.bold: true
                        }
                        MouseArea {
                            id: mArea
                            anchors.fill: parent
                            preventStealing: true
                            hoverEnabled: true
                        }
                    }
                }
            }
        }
    }

    property Component dropDownStyle: MenuStyle{}
}


//}
