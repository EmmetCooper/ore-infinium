import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

TextField {
    anchors {
        topMargin: 5
        bottomMargin: 5
        leftMargin: 10
        rightMargin: 10
    }

        style: TextFieldStyle {
            textColor: "black"


            background: BorderImage  {
                id: borderImage

                anchors.fill: parent

                antialiasing: true

                border.bottom: 8
                border.top: 8
                border.left: 8
                border.right: 8

                anchors {
                    topMargin: 2
                    bottomMargin: 2
                }

                source: {
                    if (control.hovered) {
                        "../textbox-focused.png"
                    } else {
                        "../textbox-normal.png"
                    }
                }
        }
    }

}
