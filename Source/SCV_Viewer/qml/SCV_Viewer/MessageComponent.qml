import QtQuick 2.0

Item {
    id: delegateMessage
    property alias running: idAnim.running
    property Item centerItem
    property var ptFrom: source ? mapFromItem(source,
                                     source.width/2,
                                     source.height/2) : {"x":0, "y":0}
    property var ptTo: target ? mapFromItem(target,
                                     target.width/2,
                                     target.height/2) : {"x":0, "y":0}
    property var ptCenter: mapFromItem(centerItem,
                                       centerItem.width/2,
                                       centerItem.height/2)
    visible: idAnim.running

    Item {
        id: travel
//        z: -10
//        scale: 0.5
//        Image {
//            id: imgMessage
//            source: "qrc:Pruebas64.png"
//            //anchors.centerIn: parent
//        }
        Rectangle {
            anchors.fill: colMsg
            anchors.margins: -5
            radius: 5
            border.color: "#30B0C0"
            border.width: 3
            gradient: Gradient {
                GradientStop { position: 0.0; color: "lightsteelblue" }
                GradientStop { position: 1.0; color: "lightcyan" }
            }
        }

        Column {
            id: colMsg
            anchors.centerIn: parent
            Text {
                id: txtlastMessageRecv
                text: "<b>"+model.type+"</b> "+model.name
            }
            Text {
                id: txtlastMessageFrom
                text: "<b>"+model.programFrom.name+"</b> -> <b>"+model.programTo.name+"</b>"
            }
        }
    }

    ParallelAnimation {
        id: idAnim
        //running: model.id !== ""
        //loops: 1000
        PathAnimation {
            id: pathAnim

            duration: 2000
            easing.type: Easing.OutInQuad
            target: travel
            //orientation: PathAnimation.RightFirst
            //anchorPoint: Qt.point(imgMessage.width/2, imgMessage.height/2)
            path: Path {
                startX: ptFrom.x; startY: ptFrom.y
                PathQuad {  x: ptTo.x; y: ptTo.y; controlX: ptCenter.x; controlY: ptCenter.y }
            }
        }
        SequentialAnimation {
            NumberAnimation { target: source; property: "scale"; to: 0.7; duration: 50 }
            NumberAnimation { target: source; property: "scale"; to: 1; duration: 100 }
        }
        SequentialAnimation {
            NumberAnimation { target: travel; property: "scale"; from: 0; to: 1; duration: 300 }
            PauseAnimation { duration: pathAnim.duration-400 }
            ParallelAnimation {
                NumberAnimation { target: travel; property: "scale"; to: 0; duration: 200 }
                SequentialAnimation {
                    NumberAnimation { target: model.target; property: "scale"; to: 1.4; duration: 50 }
                    NumberAnimation { target: model.target; property: "scale"; to: 1; duration: 100 }
                }
                //ColorAnimation { target: target; property: "color"; to: "grey"; duration: 200 }
            }
        }
        onRunningChanged: {
            if (!running) {
                travel.scale=1
                model.source = null
                model.target = null
                //delegateMessage.visible = false
                //model.id = ""
                //model.name = "*"
                //model.type = "+"
                //delegateMessage.destroy()
            } else {
                //idAnim.start()
            }
        }
    }
}

