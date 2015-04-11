import QtQuick 2.0
import QtGraphicalEffects 1.0
import "../qml"

Item {

    id: mainWindow
    width: 1024
    height: 768

    function toggle() {
        infoUserWindows.flipped = !infoUserWindows.flipped;
    }

    Clock {}

    Flipable {

        id: infoUserWindows
        x:60
        y:50
        width: 800
        height: 600

        property bool flipped: false

        front: Item {
            Rectangle {
                id: baseFront
                width: infoUserWindows.width
                height: infoUserWindows.height
                color:"white"
                border.color: "yellowgreen"
                border.width: 3
                radius: 15
                smooth: true
            }

            Text {

                anchors.horizontalCenter: baseFront.horizontalCenter
                anchors.top : baseFront.top
                font.pointSize: 40
                styleColor: "yellowgreen"
                style: Text.Outline
                text: "Bienvenido/a"
            }

            Image {
                id: saver
                anchors.verticalCenter: baseFront.verticalCenter
                anchors.horizontalCenter: baseFront.horizontalCenter
                width: 378
                height: 284
                source: "images/saver_alt.jpg"
            }
        }

        back: Item {

            transform:Rotation {
                origin.x: infoUserWindows.width/2 + infoUserWindows.x
                origin.y: infoUserWindows.height/2 + infoUserWindows.y
                axis.x:0; axis.y:1; axis.z:0
                angle:180
            }


            Rectangle {
                id: baseBack
                width: infoUserWindows.width
                height: infoUserWindows.height
                color:"white"
                border.color: "yellowgreen"
                border.width: 3
                radius: 15
                smooth: true
            }

            DropShadow {
                anchors.fill: errorMsg
                horizontalOffset: 3
                verticalOffset: 3
                radius: 8
                samples: 8
                color: "#80000000"
                source: errorMsg
            }

            Text {
                id : errorMsg
                objectName: "errorMsg"
                anchors.verticalCenter: baseBack.verticalCenter
                anchors.right: baseBack.right
                anchors.rightMargin: 40
                font.capitalization : Font.AllLowercase
                color: "red"
                text: ""
                font.pointSize: 20
            }


            Image {
                id: fotoStudent
                objectName: "fotoStudent"
                anchors.leftMargin: 15
                anchors.verticalCenter: baseBack.verticalCenter
                width: baseBack.height/3
                height: baseBack.height/3
                source: "image://getimagebyrut/16486738"
            }


            Text {
                id : lunchInfo
                objectName: "lunchInfo"
                anchors.top : baseBack.top
                anchors.topMargin: 30
                anchors.left: baseBack.left
                anchors.leftMargin: baseBack.width/7
                color: "green"
                text: "Almuerzos disponibles :"
                font.pointSize: 15
            }

            Text {
                id : dinnerInfo
                objectName: "dinnerInfo"
                anchors.top : baseBack.top
                anchors.topMargin: 30
                anchors.right: baseBack.right
                anchors.rightMargin: baseBack.width/7
                color: "green"
                text: "Cenas disponibles :"
                font.pointSize: 15
            }
        }

        transform: Rotation {
            id: rotation
            origin.x: infoUserWindows.width/2 + infoUserWindows.x
            origin.y: infoUserWindows.height/2 + infoUserWindows.y
            axis.x: 0; axis.y: 1; axis.z: 0
            angle: 0
        }

        transitions: Transition {
            NumberAnimation { target: rotation; property: "angle"; duration: 1000 }
        }

        states: State {
            name: "back"
            PropertyChanges { target: rotation; angle: 180 }
            when: infoUserWindows.flipped
        }
    }
}
