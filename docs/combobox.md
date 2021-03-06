
<a id="uicombobox"></a>
## UiCombobox ⇐ <code>UiControl</code>
A drop down combo box that allow list selection only.

**Kind**: global class  
**Extends**: <code>UiControl</code>  

* [UiCombobox](#UiCombobox) ⇐ <code>UiControl</code>
    * [new UiCombobox()](#new_UiCombobox_new)
    * [.selected](#UiCombobox_selected) ⇒ <code>number</code>
    * [.append(text)](#UiCombobox_append)
    * [.onSelected(callback)](#UiCombobox_onSelected)
    * [.enabled](#UiCombobox_enabled) ⇒ <code>boolean</code>
    * [.visible](#UiCombobox_visible) ⇒ <code>boolean</code>
    * [.toplevel](#UiCombobox_toplevel) ⇒ <code>boolean</code>


<a id="new_uicombobox_new"></a>
### new UiCombobox()
Create a new UiCombobox object.


<a id="uicombobox_selected"></a>
### uiCombobox.selected ⇒ <code>number</code>
Return or set the current selected item by index.

**Kind**: instance property of [<code>UiCombobox</code>](#UiCombobox)  

<a id="uicombobox_append"></a>
### uiCombobox.append(text)
Append a new text item to the drop down list.

**Kind**: instance method of [<code>UiCombobox</code>](#UiCombobox)  

| Param | Type | Description |
| --- | --- | --- |
| text | <code>string</code> | the item to append. |


<a id="uicombobox_onselected"></a>
### uiCombobox.onSelected(callback)
Add a listener to the `selected` event. Emitted whenever the selected
item changes.

**Kind**: instance method of [<code>UiCombobox</code>](#UiCombobox)  

| Param | Type | Description |
| --- | --- | --- |
| callback | <code>function</code> | callback to execute when the event is fired. |


<a id="uicombobox_enabled"></a>
### uiCombobox.enabled ⇒ <code>boolean</code>
Set or return whether the control is enabled.

**Kind**: instance property of [<code>UiCombobox</code>](#UiCombobox)  

<a id="uicombobox_visible"></a>
### uiCombobox.visible ⇒ <code>boolean</code>
Set or return whether the control is visible.

**Kind**: instance property of [<code>UiCombobox</code>](#UiCombobox)  

<a id="uicombobox_toplevel"></a>
### uiCombobox.toplevel ⇒ <code>boolean</code>
Set or return whether the control is a top level one.

**Kind**: instance property of [<code>UiCombobox</code>](#UiCombobox)  
