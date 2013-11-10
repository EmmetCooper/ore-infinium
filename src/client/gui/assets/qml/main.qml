import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import SceneGraphRendering 1.0
import QtQuick.Layouts 1.0
import Graph 1.0

//property alias label: text.text

Item {
    id: main

    Component.onCompleted: {

    }

    Connections {
        target: ClientBackend

        onGameStarted: {
            stackView.clear()
        }
    }

    /**
     *
     * NOTE: at the top so it's underneath everything in z
     *instantiation order (because qml reads from top of file to end of file, and z increases as it gets further down)..
     */
    Renderer {
        id: renderer
        anchors.fill: parent
    }

    Graph {
        anchors {
            left: parent.left
            bottom: parent.bottom

            leftMargin: 20
            bottomMargin: 20
        }

        height: 300
        width: 300
    }

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
        id: escapeMenu

        EscapeMenu
        {
            id: menu

            onDisconnectClicked: {
                ClientBackend.disconnectClicked();
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

        onEscapePressed: {
            if (ClientBackend.gameConnected) {
                stackView.push(escapeMenu)
            }
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
}
