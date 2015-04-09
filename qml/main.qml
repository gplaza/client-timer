import QtQuick 2.0
import QtGraphicalEffects 1.0
import "../qml"

Item {

    id: mainWindow
    width: 1920
    height: 1080

    function toggle() {
        if(rot.angle ==0)
            rot.angle = 180;
        else
            rot.angle = 0;
    }

    function showFront() {
        rot.angle=0;
    }

    function showBack() {
        rot.angle=180;
    }

    Clock {}

    Flipable {
        id: infoUserWindows
        x:60
        y:50
        width: 1200  //800
        height: 600 //430

        transform: Rotation {
            id: rot
            origin.x: 400;
            origin.y: 100;
            axis.x:0; axis.y:1; axis.z:0
            angle:0

            Behavior on angle { PropertyAnimation{} }
        }

        front: Item {
            Rectangle {
                id: baseFront
                width: 1200
                height: 600
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
                origin.x: 400;
                origin.y:100;
                axis.x:0; axis.y:1; axis.z:0
                angle:180
            }

            Rectangle {
                id:baseBack
                width: 1200
                height: 600
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
                color: "red"
                text: ""
                font.pointSize: 30
            }

            Image {
                id: foto_estudiante
                x: 15
                anchors.verticalCenter: baseBack.verticalCenter
                width: 300
                height: 300
                source: "images/seba.jpg"
            }

            Text {
                id : lunchInfo
                objectName: "lunchInfo"
                anchors.top : baseBack.top
                anchors.topMargin: 30
                anchors.left: baseBack.left
                anchors.leftMargin: 250
                color: "green"
                text: "Almuerzos disponibles :"
                font.pointSize: 20
            }

            Text {
                id : dinnerInfo
                objectName: "dinnerInfo"
                anchors.top : baseBack.top
                anchors.topMargin: 30
                anchors.right: baseBack.right
                anchors.rightMargin: 250
                color: "green"
                text: "Cenas disponibles :"
                font.pointSize: 20
            }
        }
    }
}
