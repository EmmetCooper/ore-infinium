import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import OpenGLUnderQML 1.0
import QtQuick.Layouts 1.0

//property alias label: text.text

Item {
    ColumnLayout {
    anchors.centerIn: parent

        OreButton {
            text: "mainmenu"
        }

        OreButton {
            text: "mainmenu"
        }

        OreButton {
            text: "Options"
        }

        OreButton {
            text: "Exit"
        }
    }
}
