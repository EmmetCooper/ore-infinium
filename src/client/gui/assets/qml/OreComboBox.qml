import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0

    import QtQuick 2.1
    import QtQuick.Controls 1.0
    import QtQuick.Controls.Styles 1.0
    import QtQuick.Controls.Private 1.0

 //   style: ComboBoxStyle
//    {

    Style {
        readonly property ComboBox control: __control
        property Component panel: Item {
            property bool popup: !!styleItem.styleHint("comboboxpopup")

            implicitWidth: 115
            implicitHeight: styleItem.implicitHeight
            anchors.fill: parent
            StyleItem {
                id: styleItem

                height: parent.height
                width: parent.width
                elementType: "combobox"
                sunken: control.pressed
                raised: !sunken
                hover: control.hovered
                enabled: control.enabled
                // The style makes sure the text rendering won't overlap the decoration.
                // In that case, 35 pixels margin in this case looks good enough. Worst
                // case, the ellipsis will be truncated (2nd worst, not visible at all).
                text: elidedText(control.currentText, Text.ElideRight, parent.width - 35)
                hasFocus: control.activeFocus
                // contentHeight as in QComboBox
                contentHeight: Math.max(Math.ceil(textHeight("")), 14) + 2

                hints: control.styleHints
                properties: {
                    "popup": control.__popup
                }
            }
        }

        property Component __popupStyle: MenuStyle {
            __menuItemType: "comboboxitem"
        }

        property Component __dropDownStyle: Style {
            property Component frame: StyleItem {
                elementType: "frame"

                width: (parent ? parent.contentWidth : 0)
                height: (parent ? parent.contentHeight : 0) + 2 * pixelMetric("defaultframewidth")
            }

            property Component menuItem: StyleItem {
                elementType: "itemrow"
                selected: parent ? parent.selected : false

                x: pixelMetric("defaultframewidth")
                y: pixelMetric("defaultframewidth")

                implicitWidth: textItem.contentWidth
                implicitHeight: textItem.contentHeight

                StyleItem {
                    id: textItem
                    elementType: "item"
                    contentWidth: textWidth(text)
                    contentHeight: textHeight(text)
                    text: parent && parent.parent ? parent.parent.text : ""
                    selected: parent ? parent.selected : false
                }
            }
        }
    }

//}
