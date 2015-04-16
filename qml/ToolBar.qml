import QtQuick 2.0

Item {

    id:root

    Rectangle {
        id: toolBar
        color:"white"
        width: parent.width
        height: parent.height
    }

    Image {

        source :"images/logo_federico.jpeg"
        width: 225
        height: 94
        anchors.top: toolBar.top;
        anchors.left: toolBar.left;
        anchors.topMargin: 10
        anchors.leftMargin: 10
    }

    Text {

        id: clockText

        property real fontSize: 30
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
            return d.getHours() + ":" + m + " " + da + "/" + mo + "/" + y; //TODO : hours 2 digit.
        }

        color: textColor;
        font.family: fontFamily;
        font.pixelSize: fontSize * fontScale;

        anchors.top : toolBar.top;
        anchors.right: toolBar.right;

        anchors.rightMargin: 70
        anchors.topMargin: 10

        Timer {
            interval: 60000;
            repeat: true;
            running: true
            onTriggered: clockText.text = clockText.currentTime();
        }
    }
}





