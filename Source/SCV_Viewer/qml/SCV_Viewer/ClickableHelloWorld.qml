import QtQuick 2.0


Rectangle {
    width: 360
    height: 360
    Text {
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }

    ListModel {
        id: nameModel
        ListElement { name: "Alice"; team: "Crypto" }
        ListElement { name: "Bob"; team: "Crypto" }
        ListElement { name: "Jane"; team: "QA" }
        ListElement { name: "Victor"; team: "QA" }
        ListElement { name: "Wendy"; team: "Graphics" }
    }
    Component {
        id: nameDelegate
        Text {
            text: if (index == 0)
                name + " (default)"
            else
                name
            font.pixelSize: 24
            anchors.left: parent.left
            anchors.leftMargin: 2
        }
    }

    ListView {
        anchors.fill: parent
        model: nameModel
        delegate: nameDelegate
        focus: true
        highlight: Rectangle {
            color: "lightblue"
            width: parent.width
        }
        section {
            property: "team"
            criteria: ViewSection.FullString
            delegate: Rectangle {
                color: "#b0dfb0"
                width: parent.width
                height: childrenRect.height + 4
                Text { anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 16
                    font.bold: true
                    text: section
                }
            }
        }
    }

    Component {     //instantiated when header is processed
        id: bannercomponent
        Rectangle {
            id: banner
            width: parent.width; height: 50
            gradient: clubcolors
            border {color: "#9EDDF2"; width: 2}
            Text {
                anchors.centerIn: parent
                text: "Club Members"
                font.pixelSize: 32
            }
        }
    }
    Gradient {
        id: clubcolors
        GradientStop { position: 0.0; color: "#8EE2FE"}
        GradientStop { position: 0.66; color: "#8EFEE2"}
    }
}
