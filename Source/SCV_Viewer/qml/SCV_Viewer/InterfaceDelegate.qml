import QtQuick 2.0

Rectangle {
    id: interfaceRectangle
    width: interfaceRectangle.ListView.view.width
    height: updateCol.height
    color: "#c0c0c0"

//    ExplosionSystem {
//        id: particleSystem
//        anchors.fill: parent
//    //    running: !startScreen.visible
//    }

    Column {
        id: updateCol
        width: parent.width

        Text {
            id: updateText
            width: parent.width
            text: "<b>"+ip+":</b> "+name
            color: (turned == "On") ? "green" : (turned == "Off") ? "red" : "yellow"
        }

        Rectangle {
            id: commentsBox
            color: "white"
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            height: commentsCol.height

            Column {
                id: commentsCol
                width: parent.width

                Repeater {
                    id: comments
                    width: parent.width
                    model: programs
                    delegate: Column {
                        Text { text: "<b>"+name+":</b> user=" + user + ", type=" + type }
                        Text { text: "lastMessageSent=" + lastMessageSent.type + "-" + lastMessageSent.name +
                                     ", lastMessageReceived=" + lastMessageReceived.type + "-" + lastMessageReceived.name }
                        Rectangle {
                            id: instatesBox
                            color: "#e0e0e0"
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 5
                            anchors.rightMargin: 5
                            height: instatesCol.height

                            Column {
                                id: instatesCol
                                width: parent.width

                                Repeater {
                                    id: instatesRepeater
                                    width: parent.width
                                    model: props
                                    delegate: Text {
                                        id:idText
                                        text:"<b>"+name+"</b>=" + value
                                        onTextChanged: console.log("Text has changed to:"+idText.text)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            id: statesBox
            color: "#d0d0d0"
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            height: statesCol.height

            Column {
                id: statesCol
                width: parent.width

                Repeater {
                    id: statesRepeater
                    width: parent.width
                    model: props
                    delegate: Text {
                        id:idPropText
                        text: name + "=" + value
                        onTextChanged: console.log("Text has changed to:"+idPropText.text)
                    }
                }
            }
        }
    }
}

