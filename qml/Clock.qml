import QtQuick 2.0

Item {

    id:root

    Rectangle {
        id:clockBar
        color:"white"
        width: parent.width
        height: parent.height
    }

    Image {
        source :"images/printer_KO.png"
        width: 50
        height: 50

        anchors.top : clockBar.top;
        anchors.right: clockBar.right;

        anchors.rightMargin: 20
    }

    Text {

        id: clockText

        property real fontSize: 50
        property real fontScale: 0.5
        property color textColor: "green"
        property string fontFamily: "Times"
        font.weight: Font.DemiBold

        text: currentTime();

        function currentTime() {
            var d = new Date();
            var mo = d.getMonth() + 1;
            var da = d.getDate();
            var y = d.getFullYear();
            var m = d.getMinutes();
            if (mo < 10) mo = "0" + mo;
            if (m < 10) m = "0" + m;
            return d.getHours() + ":" + m + " " + da + "/" + mo + "/" + y;
        }

        color: textColor;
        font.family: fontFamily;
        font.pixelSize: fontSize * fontScale;

        anchors.top : clockBar.top;
        anchors.right: clockBar.right;
        anchors.rightMargin: 200

        Timer {
            interval: 60000;
            repeat: true;
            running: true
            onTriggered: clockText.text = clockText.currentTime();
        }
    }
}





