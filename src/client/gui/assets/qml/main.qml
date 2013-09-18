import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import OpenGLUnderQML 1.0
import QtQuick.Layouts 1.0

//property alias label: text.text

Item {
    OptionsDialog {
        width: 200
        height: 200
        x: 300
        y: 300
    }

    Loader {
       id: mainMenuLoader
       source: "mainMenu.qml"
       parent: parent
       anchors.fill: parent
    }

    //FIXME: gynormous hack, strip win size out into C++
    width: 1024
    height: 768

    Client {
        SequentialAnimation on t {
            NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
            NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
            loops: Animation.Infinite
            running: true
        }
    }
}
