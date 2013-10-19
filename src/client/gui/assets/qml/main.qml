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
        id: singleplayerMenu

        SingleplayerMenu
        {
            id: menu

            onCreateWorldClicked: {
                stackView.push(singleplayerCreateMenu)
            }

            onLoadWorldClicked: {
                stackView.push(singleplayerLoadMenu)
            }
        }
    }

    Component {
        id: multiplayerMenu

        MultiplayerMenu
        {
            id: menu

            onHostClicked: {
                stackView.push(multiplayerHostMenu)
            }

            onJoinClicked: {
                stackView.push(multiplayerJoinMenu)
            }
        }
    }


    Component {
        id: singleplayerLoadMenu

        SingleplayerLoadMenu
        {
        }
    }


    Component {
        id: singleplayerCreateMenu

        SingleplayerCreateMenu
        {
        }
    }

    Component {
        id: multiplayerHostMenu

        MultiplayerHostMenu
        {
        }
    }

    Component {
        id: multiplayerJoinMenu

        MultiplayerJoinMenu
        {
        }
    }

    Connections {
       id: mainMenuConnections
       target: mainMenuLoader.item

       onSingleplayerClicked: {
           stackView.push(singleplayerMenu)
       }

        onMultiplayerClicked: {
           stackView.push(multiplayerMenu)
       }
    }

    StackView {
        id: stackView

        anchors.fill: parent

        //focus: true

        delegate: StackViewDelegate {
            function transitionFinished(properties)
            {
                properties.exitItem.opacity = 1
                properties.enterItem.focus = true
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
        } // delegate

        initialItem: mainMenuLoader.item
    }

    //FIXME: gynormous hack, strip win size out into C++
    width: 1600
    height: 900

    Client {
        SequentialAnimation on t {
            NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
            NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
            loops: Animation.Infinite
            running: true
        }

        onPlayNowStarted: {
            print("YO DOA");
            stackView.clear()
        }
    }
}
