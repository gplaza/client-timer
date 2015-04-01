import QtQuick 2.0
import QtGraphicalEffects 1.0
import "../qml"

Flipable {
    id: myFlip
    x:60
    y:50
    width: 800
    height: 430

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
            id: base
            width: 800
            height: 430
            color:"white"
            border.color: "yellowgreen"
            border.width: 3
            radius: 15
            smooth: true
        }

        Text {
            x: 0
            y:200
            text: "Bienvenido"
        }

        Image {
            id: saver
            anchors.verticalCenter: base.verticalCenter
            anchors.horizontalCenter: base.horizontalCenter
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
            width: 800
            height: 430
            color:"white"
            border.color: "black"
            border.width: 1
            radius: 15
        }

        Text {
            x: 0
            y:200
            text: "My super cool red view"
        }

        Image {
            id: foto_estudiante
            x: 5
            y: 20
            width: 300
            height: 300
            source: "images/seba.jpg"
        }
    }
}

/*
Item {

    id: root
    width: 1024
    height: 768

    Clock
    {
        anchors.right: parent.right
        width: parent.width
        height: 50
    }

    Image {
        id: logo_federico
        x: 50
        y: -40
        width: 300
        height: 300
        source: "images/logo_federico.jpeg"
    }


    Text {
        id: rut_label
        x: 80
        y: 600
        text: qsTr("RUT : 16.486.738-2")
        font.pixelSize: 30
        color: "green"
    }

    DropShadow {
        anchors.fill: rut_label
        horizontalOffset: 3
        verticalOffset: 3
        radius: 8.0
        samples: 16
        color: "#80000000"
        source: rut_label
    }

    Text {
        id: nombre_label
        x: 80
        y: 640
        text: qsTr("Nombre : PICARDO VERGARA SEBASTIAN […]")
        font.pixelSize: 25
        color: "green"
    }

    DropShadow {
        anchors.fill: nombre_label
        horizontalOffset: 3
        verticalOffset: 3
        radius: 8.0
        samples: 16
        color: "#80000000"
        source: nombre_label
    }
}

*/
