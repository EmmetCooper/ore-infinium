import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import OpenGLUnderQML 1.0
import QtQuick.Layouts 1.0

//property alias label: text.text

Item {
    id: main

    Loader {
        id: mainMenuLoader
        anchors.fill: parent

        // loaded at init
        source: "mainMenu.qml"
    }

    Component {
        id: singlePlayerMenu

        SingleplayerMenu
        {

        }
    }

    Connections {
       id: mainMenuConnections
       target: mainMenuLoader.item

       onSingleplayerClicked: {
           stackView.push(singlePlayerMenu)
       }
    }

    StackView {
        id: stackView

        delegate: StackViewDelegate {
            function transitionFinished(properties)
            {
                properties.exitItem.opacity = 1
            }

            property Component pushTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem
                    property: "opacity"
                    from: 0
                    to: 1
                }
                PropertyAnimation {
                    target: exitItem
                    property: "opacity"
                    from: 1
                    to: 0
                }
            }
        }

        anchors.fill: parent

        initialItem: mainMenuLoader.item
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
