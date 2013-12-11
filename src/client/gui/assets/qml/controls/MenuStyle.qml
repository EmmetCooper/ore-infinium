import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

ComboBoxStyle {
    id: styleRoot

    property string __menuItemType: "menuitem"

    property Component frame: Rectangle {
        implicitHeight: parent ? parent.contentHeight : 0
        implicitWidth: parent ? parent.contentWidth : 0
        color: "transparent"
        border.color: "black"
        border.width: 2
    }

    property Component menuItem: Rectangle {
        implicitHeight: 40
        implicitWidth: 200
        color: mArea.containsMouse ? "#d2d7da" : "#a6afb6"
        border.color: "black"
        border.width: 1
        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: mArea.containsMouse ? 10 : 5
            text: !!parent.parent && parent.parent.text
            color: "white"
        }
        MouseArea {
            id: mArea
            anchors.fill: parent
            hoverEnabled: true
        }
    }
}
