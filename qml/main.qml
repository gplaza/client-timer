import QtQuick 2.0
import QtGraphicalEffects 1.0
import "../qml"

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

    GridView {
        id: grid_view1
        x: 473
        y: 180
        width: 800
        height: 441
        cellWidth: 250
        model: ListModel {
            ListElement {
                name: "Grey"
                // colorCode: "grey"
            }
            ListElement {
                name: "Grey"
                // colorCode: "grey"
            }
            ListElement {
                name: "Grey"
                // colorCode: "grey"
            }
            ListElement {
                name: "Grey"
                // colorCode: "grey"
            }
            ListElement {
                name: "Grey"
                // colorCode: "grey"
            }
            ListElement {
                name: "Grey"
                // colorCode: "grey"
            }
            ListElement {
                name: "Grey"
                // colorCode: "grey"
            }
            ListElement {
                name: "Grey"
                // colorCode: "grey"
            }

            ListElement {
                name: "Grey"
                // colorCode: "grey"
            }

            ListElement {
                name: "Red"
                // colorCode: "red"
            }

            ListElement {
                name: "Blue"
                // colorCode: "blue"
            }

            ListElement {
                name: "Green"
                // colorCode: "green"
            }
        }
        delegate: Item {
            x: 5
            height: 30
            Column {
                spacing: 3

                Image {
                    width: 200
                    height: 30
                    source : "images/consum.png"
                    anchors.horizontalCenter: parent.horizontalCenter

                }

                Text {
                    x: 0
                    text: name

                    anchors.horizontalCenter: parent.horizontalCenter
                    font.bold: true
                }
            }
        }
        cellHeight: 40
    }

    Image {
        id: logo_federico
        x: 50
        y: -40
        width: 300
        height: 300
        source: "images/logo_federico.jpeg"
    }

    Image {
        id: foto_estudiante
        x: 50
        y: 250
        width: 300
        height: 300
        source: "images/seba.jpg"
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

