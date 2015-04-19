import QtQuick 2.0
import QtGraphicalEffects 1.0
import "../qml"
import jbQuick.Charts 1.0

Item {

    id: mainWindow
    width: 1024
    height: 768

    function toggle() {
        infoUserWindows.flipped = !infoUserWindows.flipped;
    }

    function changeStatusPrinter(status) {
        console.log(status);
        statusPrinter.source = (status)? "images/printer_OK.png" : "images/printer_KO.png";
        statusPrinter.update();
    }

    /*
      see : http://jwintz.me/blog/2014/02/15/qchart-dot-js-qml-binding-for-chart-dot-js/
    Chart {
        id : hcrat_line
        width : 400
        height : 400
        anchors.right: mainWindow.right
        chartAnimated : false
        chartAnimationEasing: Easing.InOutElastic
        chartAnimationDuration: 2000
        chartType: Charts.ChartType.LINE
        Component.onCompleted: {
            chartData = [
                        {
                            label: "My First dataset",
                            fillColor: "rgba(220,220,220,0.2)",
                            strokeColor: "rgba(220,220,220,1)",
                            pointColor: "rgba(220,220,220,1)",
                            pointStrokeColor: "#fff",
                            pointHighlightFill: "#fff",
                            pointHighlightStroke: "rgba(220,220,220,1)",
                            data: [65, 59, 80, 81, 56, 55, 40]
                        }
                    ]
        }
    }*/

    Rectangle {

        id : containerCasinoCounter
        width: 150
        height: 170
        anchors.right: parent.right
        anchors.rightMargin: 100
        anchors.top: toolbarObject.bottom
        anchors.topMargin: 100
        color:"white"
        border.color: "yellowgreen"
        border.width: 7
        radius: width/2
        opacity: 0.4

        gradient: Gradient {
            GradientStop { position: 0.0; color: "white" }
            GradientStop { position: 1.0; color: "yellowgreen" }
        }
    }


    Text {
        id : casinoCounter
        objectName: "casinoCount"
        anchors.horizontalCenter: containerCasinoCounter.horizontalCenter
        anchors.verticalCenter: containerCasinoCounter.verticalCenter
        styleColor: "yellowgreen"
        style: Text.Outline
        text: "99"
        font.pointSize: mainWindow.width / 34
    }

    Text {

        id : labelCasinoCounter
        anchors.horizontalCenter: containerCasinoCounter.horizontalCenter
        anchors.bottom: casinoCounter.top
        styleColor: "yellowgreen"
        style: Text.Outline
        text : "servicios"
        font.pointSize: mainWindow.width / 55
    }

    Text {

        id : casinoName
        anchors.horizontalCenter: containerCasinoCounter.horizontalCenter
        anchors.bottom: containerCasinoCounter.top
        anchors.bottomMargin: 20
        styleColor: "yellowgreen"
        style: Text.Outline
        text : "COMEDORUSM"
        font.pointSize: mainWindow.width / 55
    }

    ToolBar {

        id : toolbarObject
        anchors.right: parent.right
        width: parent.width
        height: 50

        Image {
            id : statusPrinter
            source :"images/printer_OK.png"
            objectName: "statusPrinter"
            cache: false
            width: 30
            height: 30
            anchors.top : parent.top;
            anchors.right: parent.right;
            anchors.topMargin: 10
            anchors.rightMargin: 10
        }
    }

    Flipable {

        id: infoUserWindows
        x: 10
        anchors.bottom: mainWindow.bottom
        anchors.bottomMargin: mainWindow.height / 15
        width: mainWindow.width / 1.5
        height: mainWindow.height / 1.3

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
                anchors.topMargin: 10
                font.pointSize: mainWindow.width / 34
                styleColor: "yellowgreen"
                style: Text.Outline
                text: "Bienvenido/a"
            }

            Image {
                id: saver
                anchors.verticalCenter: baseFront.verticalCenter
                anchors.horizontalCenter: baseFront.horizontalCenter
                width: baseFront.width/ 1.5
                height: baseFront.width/ 1.5
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

            Image {
                id: fotoStudent
                objectName: "fotoStudent"
                anchors.left: baseBack.left
                anchors.verticalCenter: baseBack.verticalCenter
                anchors.leftMargin: 15
                width: baseBack.height/3
                height: baseBack.height/3
                source: "image://getimagebyrut/16486738"
            }

            Text {
                id : errorMsg
                objectName: "errorMsg"
                width : baseBack.width - (10 + fotoStudent.width + 10)
                anchors.verticalCenter: baseBack.verticalCenter
                anchors.left : fotoStudent.right
                anchors.leftMargin: 10
                wrapMode: Text.WordWrap
                font.capitalization : Font.AllLowercase
                color: "red"
                text: ""
                font.pointSize: 20
            }

            Text {
                id : rut
                objectName: "rutText"
                anchors.top : fotoStudent.bottom
                anchors.horizontalCenter: fotoStudent.horizontalCenter
                anchors.topMargin: 15
                text: ""
                color: "green"
            }

            Text {
                id : name
                objectName: "nameText"
                anchors.top : rut.bottom
                anchors.left: rut.left
                anchors.topMargin: 10
                text: ""
                color: "green"
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
