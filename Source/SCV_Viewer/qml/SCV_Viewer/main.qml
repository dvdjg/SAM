import QtQuick 2.0
import QtQuick.Particles 2.0

Rectangle {
    id: frame
    width: 450; height: 800
    //anchors.fill: parent
    gradient: Gradient {
        GradientStop { position: 0.0; color: "lightgrey" }
        GradientStop { position: 1.0; color: "grey" }
    }
    property int itemSize: 50

    function extmessage(from, to, type)
    {
//        console.log("message("+from+"-"+from.name+", "+to.name+", "+type+")")
//        for(var i = 0; i < view.children.length; ++i)
//        {
//            var item = view.children[i];

//            console.log("item-"+item.model+"-"+item.x+","+item.y)

//            for(var prop in item) {
//                console.log("prop-"+prop)
//            }
////            console.log("item-"+item.data+"-"+item.x+","+item.y)

////            for(var prop in item.data) {
////                console.log("prop-"+item.data[prop])
////            }
//        }//interfaces.
    }


    ListModel {
        id: appModel
        ListElement { name: "Qt"; icon: "qrc:Pruebas64.png" }
        ListElement { name: "Music"; icon: "qrc:bluetooth.svg" }
        ListElement { name: "Movies"; icon: "qrc:cell.svg" }
        ListElement { name: "Camera"; icon: "qrc:gprs.svg" }
        ListElement { name: "Calendar"; icon: "qrc:lan.svg" }
        ListElement { name: "Messaging"; icon: "qrc:umts.svg" }
        ListElement { name: "Todo List"; icon: "qrc:unknown.svg" }
        ListElement { name: "Contacts"; icon: "qrc:wlan.svg" }
    }

    ListView  {
        id: interfacesView
        width: 250
        height: 600

        model: interfaces
        delegate: InterfaceDelegate {}

    }

//    VisualItemModel {
//        id: visualMsgModel
//        MessageComponent { visible: false; centerItem: broadcast; srcItem: broadcast; dstItem: broadcast }
//        MessageComponent { visible: false; centerItem: broadcast; srcItem: broadcast; dstItem: broadcast }
//        MessageComponent { visible: false; centerItem: broadcast; srcItem: broadcast; dstItem: broadcast }
//        MessageComponent { visible: false; centerItem: broadcast; srcItem: broadcast; dstItem: broadcast }
//        MessageComponent { visible: false; centerItem: broadcast; srcItem: broadcast; dstItem: broadcast }
//        MessageComponent { visible: false; centerItem: broadcast; srcItem: broadcast; dstItem: broadcast }
//    }

//    ListView {
//        property int nVisual: 0
//        id: visualMsgList
//        model: visualMsgModel
//    }

    ListView  {
        id: lstMessagesView
        width: 250
        height: 600

        anchors.left: interfacesView.right
        anchors.leftMargin: 5
        model: messages
        delegate: Item {
            width: childrenRect.width; height: childrenRect.height
            property string sota: source+"->"+target
            Text { text: id + ": <b>" + type  + "-" + name + "</b> "+sota }
//            onSotaChanged: {
//                if(source && target) {
////                    var component = Qt.createComponent("MessageComponent.qml")
////                    var sprite = component.createObject(
////                                lstMessagesView,
////                                {"visible": true,
////                                 "srcItem": source,
////                                 "dstItem": target,
////                                 "centerItem": broadcast});

////                    visualMsgList.currentIndex = (++visualMsgList.currentIndex) % visualMsgModel.count
////                    visualMsgList.currentItem.visible=true
////                    visualMsgList.currentItem.srcItem=source
////                    visualMsgList.currentItem.dstItem=target
//                }
//            }
        }
        //model: appModel
        //delegate: Text { text: "args: " + name + ", " + icon }
    }


    // ExplosionSystem { }

    Item {
        id: broadcast
        anchors.centerIn: view
        width: imgBroadcast.width; height: imgBroadcast.height
        Image {
            id: imgBroadcast
            source: "qrc:wlan.svg"
            //anchors.centerIn: parent
        }

        //ParticleFlame {}
    }

    Component {
        id: appDelegate
        Rectangle {
            id: delegateRectangle
            property color baseColor: "#d0e6e6e6"
            width: colPrograms.width+10; height: colPrograms.height+10
            //anchors.margins: -25
            //width: txtDelegate.width+radius+radius+3; height: txtDelegate.height+radius+radius
            //Behavior on height { NumberAnimation { duration: 100 } }
            //Behavior on width { NumberAnimation { duration: 100 } }
            //color: Qt.tint(border.color, baseColor)
            gradient: Gradient {
                GradientStop { position: 0.0; color: Qt.tint(border.color, "#30e6e6e6") }
                GradientStop { position: 0.3; color: Qt.tint(border.color, "#e0e6e6e6") }
                GradientStop { position: 1.0; color: Qt.tint(border.color, baseColor) }
            }
            border.width: 3
            radius: 5
            scale: PathView.iconScale

            state: turned
            states: [
                State {
                    name: "On"
                    PropertyChanges {
                        target: delegateRectangle
                        border.color: "green"
                    }
                },
                State {
                    name: "Off"
                    PropertyChanges {
                        target: delegateRectangle
                        border.color: "red"
                    }
                },
                State {
                    name: "StandBy"
                    PropertyChanges {
                        target: delegateRectangle
                        border.color: "yellow"
                    }
                },
                State {
                    name: "None"
                    PropertyChanges {
                        target: delegateRectangle
                        border.color: "pink"
                    }
                }
            ]
            transitions: [
                Transition {
                    ColorAnimation { duration: 1000 }
                }
            ]

            ParticleFlame { z: 1;
                state: turned
                //emitting: turned == "Off";
                emitterX: width/2
            }

            Column {
                id: colPrograms
                spacing: 3
                anchors.horizontalCenter: parent.horizontalCenter
                //anchors.top: txtDelegate.bottom; anchors.left: txtDelegate.left
                Text {
                    id: txtDelegate
                    text: name + "(" + turned + ")"
                    font.family: "Arial narrow"
                    x: 5
                    //anchors.centerIn: parent
                }

                Repeater {
                    model: programs
                    delegate: Rectangle {
                        id: rcPrograms
                        //border.color: "grey"
                        border.width: 2
                        radius: 5
                        width: colProgramMsg.width+radius+radius; height: colProgramMsg.height+radius
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "grey" }
                            GradientStop { position: 0.2; color: border.color }
                            GradientStop { position: 0.5; color: Qt.tint(border.color, baseColor) }
                            GradientStop { position: 1.0; color: Qt.tint(border.color, baseColor) }
                        }
                        ParticleFlame { z: 1;
                            state: turned
                            //emitting: turned == "Off";
                            emitterX: 0; emitterY: height/2
                            velocity: PointDirection{ x: -90; xVariation: 20; }
                        }
                        state: turned
                        states: [
                            State {
                                name: "On"
                                PropertyChanges {
                                    target: rcPrograms
                                    border.color: "green"
                                }
                            },
                            State {
                                name: "Off"
                                PropertyChanges {
                                    target: rcPrograms
                                    border.color: "red"
                                }
                            },
                            State {
                                name: "StandBy"
                                PropertyChanges {
                                    target: rcPrograms
                                    border.color: "yellow"
                                }
                            },
                            State {
                                name: "None"
                                PropertyChanges {
                                    target: rcPrograms
                                    border.color: "pink"
                                }
                            }
                        ]
                        transitions: [
                            Transition {
                                ColorAnimation { duration: 1000 }
                            }
                        ]
                        Column {
                            id: colProgramMsg
                            spacing: 5
                            width: childrenRect.width+spacing
                            //height: childrenRect.height+3
                            Text {
                                id: txtProgram
                                //anchors.centerIn: parent
                                //anchors.left: parent.left
                                //anchors.top: parent.top
                                //anchors.margins: 5
                                x:5
                                text: "<b>"+name+":</b> " + user + ", " + type
                                color: "white"
                            }
                            Row {
                                id: rowLastMessage
                                x: spacing
                                //anchors.horizontalCenter: parent.horizontalCenter
                                spacing: 5
                                Rectangle {
                                    id: rcLastMessageSent
                                    border.color: "darkgoldenrod"
                                    width: txtlastMessageSent.width+radius+radius; height: txtlastMessageSent.height+radius
                                    radius: 3
                                    Text {
                                        id: txtlastMessageSent
                                        anchors.centerIn: parent
                                        text: lastMessageSent.id+". <b>"+lastMessageSent.type+"</b>\n"+lastMessageSent.name
                                        onTextChanged: {
                                            lastMessageSent.source = rcLastMessageSent
                                        }
                                    }
                                    gradient: Gradient {
                                        GradientStop { position: 0.0; color: "goldenrod" }
                                        GradientStop { position: 1.0; color: "palegoldenrod" }
                                    }
                                }
                                Rectangle {
                                    id: rcLastMessageRecv
                                    border.color: "orange"
                                    width: txtlastMessageRecv.width+radius+radius; height: txtlastMessageRecv.height+radius
                                    radius: 3
                                    Text {
                                        id: txtlastMessageRecv
                                        anchors.centerIn: parent
                                        text: lastMessageReceived.id+". <b>"+lastMessageReceived.type+"</b>\n"+lastMessageReceived.name
                                        onTextChanged: {
                                            lastMessageReceived.target = rcLastMessageRecv
                                        }
                                    }
                                    gradient: Gradient {
                                        GradientStop { position: 0.0; color: "moccasin" }
                                        GradientStop { position: 1.0; color: "papayawhip" }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: appHighlight
        Rectangle { width: itemSize+30; height: itemSize+30; color: "lightsteelblue" }
    }


    PathView {
        id: view
        anchors.fill: parent
        //highlight: appHighlight
        preferredHighlightBegin: 0.5
        preferredHighlightEnd: 0.7
        focus: true
        model: interfaces
        delegate: appDelegate
        //model: appModel
        //delegate: appDelegate
        //delegate: Interf

        /*path: Path {
            startX: 10
            startY: 50
            PathAttribute { name: "iconScale"; value: 0.5 }
            PathQuad { x: 200; y: 150; controlX: 50; controlY: 200 }
            PathAttribute { name: "iconScale"; value: 1.0 }
            PathQuad { x: 390; y: 50; controlX: 350; controlY: 200 }
            PathAttribute { name: "iconScale"; value: 0.5 }
        }*/
        path: Path {
            startX: frame.width/2+1; startY: itemSize

            PathAttribute { name: "iconScale"; value: 1.0 }
            PathArc {
                x: frame.width/2-1; y: itemSize
                radiusX: frame.width/2-itemSize; radiusY: frame.height/2-itemSize
                useLargeArc: true
            }
        }
    }
    ListView {
        id: lstMssg
        delegate: MessageComponent { running: model.id !== "" && model.source !== null && model.target !== null; centerItem: broadcast}
        model: messages
    }
}

