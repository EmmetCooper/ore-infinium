import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Layouts 1.0

import SceneGraphRendering 1.0
import Graph 1.0

import ChatModel 1.0

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

        onFrameTimeChanged: {
            //print("FRAMETIME" + frameTime);
            clientFrameTimeGraph.addSample(frameTime);
            //rint("SERVER, qml, FRAMETIME" + ClientBackend.serverFrameTime);
            serverFrameTimeGraph.addSample(ClientBackend.serverFrameTime);
            clientPacketReceivedCountGraph.addSample(ClientBackend.clientPacketReceivedCount);
            serverPacketReceivedCountGraph.addSample(ClientBackend.serverPacketReceivedCount);
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

    MouseArea {
        anchors.fill: parent
        onPressed: {
            ClientBackend.mouseAreaPressed(mouse.buttons);
        }

        onReleased: {
            ClientBackend.mouseAreaReleased(mouse.buttons);
        }

        onPositionChanged: {
            ClientBackend.mouseAreaMoved(mouse.x, mouse.y);
        }
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_F11) {
            graphs.visible = !graphs.visible
        }
    }

    Rectangle {
        id: graphs
        anchors {
            left: parent.left
            bottom: parent.bottom
        }

        color: "black"

        Graph {
            id: clientFrameTimeGraph

            anchors {
                left: parent.left
                bottom: parent.bottom

                leftMargin: 20
                bottomMargin: 20
            }

            min: 0.0
            max: 60.0

            height: 300
            width: 300
        }

        Graph {
            id: serverFrameTimeGraph

            anchors {
                left: clientFrameTimeGraph.right
                bottom: clientFrameTimeGraph.bottom

                leftMargin: 20
            }

            min: 0.0
            max: 60.0

            height: 300
            width: 300
        }

        Graph {
            id: clientPacketReceivedCountGraph

            anchors {
                left: serverFrameTimeGraph.right
                bottom: serverFrameTimeGraph.bottom

                leftMargin: 20
            }

            min: 0.0
            max: 60.0

            height: 300
            width: 300
        }

        Graph {
            id: serverPacketReceivedCountGraph

            anchors {
                left: clientPacketReceivedCountGraph.right
                bottom: clientPacketReceivedCountGraph.bottom

                leftMargin: 20
            }

            min: 0.0
            max: 60.0

            height: 300
            width: 300
        }

        OreLabel {
            anchors {
                bottom: clientFrameTimeGraph.top
                horizontalCenter: clientFrameTimeGraph.horizontalCenter
            }

            text: "Client Thread Frame Time (ms / frame)"
        }

        OreLabel {
            anchors {
                bottom: serverFrameTimeGraph.top
                horizontalCenter: serverFrameTimeGraph.horizontalCenter
            }

            text: "Server Thread Frame Time (ms / frame)"
        }

        OreLabel {
            anchors {
                bottom: clientPacketReceivedCountGraph.top
                horizontalCenter: clientPacketReceivedCountGraph.horizontalCenter
            }

            text: "Client Packet Received Count (packets / frame)"
        }

        OreLabel {
            anchors {
                bottom: serverPacketReceivedCountGraph.top
                horizontalCenter: serverPacketReceivedCountGraph.horizontalCenter
            }

            text: "Server Packet Received Count (packets / frame)"
        }

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

    EscapeMenu {
        id: escapeMenu
        visible: false

        anchors.centerIn: parent

        onDisconnectClicked: {
            ClientBackend.disconnectClicked();
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
            //FIXME:
            if (ClientBackend.gameConnected) {
                escapeMenu.visible = !escapeMenu.visible
           }
        }
    } //connections


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
    ChatModel {
        id: chatModel
    }

    ChatView {
        id: chatView
        text: chatModel.chatText
    }

    //text: "timeStamp: " + timeStamp + "PLAYERNAME: " + playerName + "CHAT TEXT: " + chatText
    //wrapMode: Text.WrapAnywhere
}
