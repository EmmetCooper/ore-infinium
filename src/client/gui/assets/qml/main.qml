import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

import SceneGraphRendering 1.0
import QtQuick.Layouts 1.0
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
                    ClientBackend.setEscapeMenuVisible(true);
                    stackView.push(escapeMenu)
        }

        onFrameTimeChanged: {
            //print("FRAMETIME" + frameTime);
            clientFrameTimeGraph.addSample(frameTime);
            //rint("SERVER, qml, FRAMETIME" + ClientBackend.serverFrameTime);
            serverFrameTimeGraph.addSample(ClientBackend.serverFrameTime);
//            serverFrameTimeGraph.addSample(10.0);
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

    Keys.onEscapePressed: {
        print("MAIN ESC PRESSED");
    }

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
            print("main.qml ESCAPE PRESSED CAUGHT, escape menu visible: " + ClientBackend.escapeMenuVisible());

                print("PRE-eval-PRE-POP count: " + stackView.depth);
            if (ClientBackend.gameConnected) {
                if (ClientBackend.escapeMenuVisible()) {
                print("PRE-POP count: " + stackView.depth);
                    ClientBackend.setEscapeMenuVisible(false);

                    stackView.pop(null);
                print("POST-POP count: " + stackView.depth);
                } else {
//                    stackView.push(escapeMenu)
                } //!gameconnected
            } //game connected
        } //onEscapePressed
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
    width: 1600
    height: 900

    Timer {
        id: timer1
        interval: 5000

        onTriggered: {
            chatView.model = chatModel
            print("TIRRGGER");
        }

    }

    ChatModel {
        id: chatModel
    }

    ListView {
        id: chatView


        Component.onCompleted: {
            timer1.start()

        }

        anchors.fill: parent

        delegate: Rectangle {
            height: 50
            width: 800

            Text {
                text: "timeStamp: " + timeStamp + "PLAYERNAME: " + playerName + "CHAT TEXT: " + chatText
            }
        }
    }
}
