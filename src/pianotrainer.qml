import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.0

ApplicationWindow
{
    Rectangle {
        x: 264
        y: 179
        width: 500
        height: 500
        color: "red"

        RadioButton {
            id: radioButton
            x: 421
            y: 531
            text: qsTr("Radio Button")
        }

    }

    visible: true
    width: 1000
    height: 900
    title: "Piano"
}
