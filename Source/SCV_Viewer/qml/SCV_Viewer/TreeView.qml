/*
 *  Copyright 2011 Ruediger Gad
 *
 *  This file is part of Q To-Do.
 *
 *  Q To-Do is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Q To-Do is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Q To-Do.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  ###################
 *
 *  Note: The code of the TreeView is additionally released under the terms
 *  of the GNU Lesser General Public License (LGPL) as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  Files considered part of the TreeView are: TreeView.qml, NodeListView.qml,
 *  and nodelisthelper.js. These files are additionally licensed under the
 *  terms of the LGPL.
 *
 */

import QtQuick 1.1
import "nodelisthelper.js" as NodeListHelper

Item {
    id: treeView

    property alias model: rootListView.model

    /*
     * Total number of currently opened ListViews.
     */
    property int listViewCount: 0
    /*
     * Level of the tree that is currently displayed starteting with 0
     * for the root level. Setting this property displays the set level
     * of the tree. Note that the view must exist and currentLevel must
     * be < listViewCount and >= 0.
     */
    property int currentLevel: 0

    /*
     * The following properties are intended to access the data shown
     * in the TreeView.
     */
    property Item currentItem: null
    property int currentIndex: -1
    property QtObject currentModel: null

    function addView(parentView){
        console.log("Entering addView...")
        if(NodeListHelper.views.length < (currentLevel + 2) ){
            console.log("Creating new sub view...")
            var view = NodeListHelper.createNextView(parentView)
            NodeListHelper.views.push(view)
            listViewCount++
        }
    }

    function clearSubLists(){
        console.log("Entering clearSublists...")
        /*
         * If a leaf node is selected all sub lists need to be cleared.
         */
        for (var i = currentLevel + 1; i < NodeListHelper.views.length; i++) {
            NodeListHelper.views[i].destroy()
        }
        listViewCount = currentLevel + 1
        NodeListHelper.views.length = listViewCount
    }

    function updateSubView(model, index){
        console.log("Updating sub view for level: " + currentLevel)
        if (NodeListHelper.views.length >= currentLevel + 2) {
            /*
             * If we moved to a higher level and the selection is changed
             * we need to destroy all views that are now out of scope to
             * avoid old data being shown.
             * Note: Smaller numbers represent higher levels. The root level is 0.
             */
            console.log("Destroying obsolete views...")
            for (var i = currentLevel + 2; i < NodeListHelper.views.length; i++) {
                NodeListHelper.views[i].destroy()
            }
            listViewCount = currentLevel + 2
            NodeListHelper.views.length = listViewCount
        }
        /*
         * Clear the ListView selection before filling the new list. Else
         * we would mess up the TreeView selection (currentItem and Index)
         * as well. Besides, this would yield in an incorrectly displayed
         * highlight.
         */
        NodeListHelper.views[currentLevel+1].currentIndex = -1
        NodeListHelper.views[currentLevel+1].model.setParentFromSelection(model, index)
    }

    Component.onCompleted: {
        NodeListHelper.views.push(rootListView)
        listViewCount++
        currentItem = rootListView.currentItem
        currentIndex = rootListView.currentIndex
        currentModel = model
    }

    onCurrentLevelChanged: {
        if(currentLevel >= 0 && currentLevel < listViewCount){
            flickable.contentX = currentLevel * treeView.width
            currentItem = NodeListHelper.views[currentLevel].currentItem
            currentModel = NodeListHelper.views[currentLevel].model
        }else{
            console.log("Invalid current level given: " + currentLevel)
            console.log("List view count is: " + listViewCount)
        }
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentWidth: listsRectangle.width

        /*
         * Disable flicking for now. There is some anoying issue that
         * selections are not correctly handled for about 0.8 - 1 seconds
         * when flicking back from a sub element view.
         * Set this to true to enable flicking.
         */
        interactive: false

        pressDelay: 100
        boundsBehavior: Flickable.StopAtBounds
        property bool animationIsRunning: false
        Behavior on contentX {
            SequentialAnimation {
                PropertyAnimation { duration: 140 }
                ScriptAction { script: flickable.animationIsRunning = false }
            }
        }

        Rectangle{
            id: listsRectangle
            width: treeView.width * listViewCount
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            NodeListView {
                id: rootListView
                width: treeView.width
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
            }

        }

        onFlickStarted: {
            animationIsRunning = true
            if(horizontalVelocity > 0){
                currentLevel = Math.min(++currentLevel, listViewCount)
            }else{
                currentLevel = Math.max(--currentLevel, 0)
            }
        }

        /*
         * If no flick was triggered (no animation is running) the list which
         * currently fills most of the screen (> 50% = 0.5) is moved to the center.
         * This is done to avoid stopping "in between" to lists.
         */
        onMovementEnded: {
            if(! animationIsRunning){
                var newLevel = Math.floor((contentX + 0.5 * treeView.width) / treeView.width)
                if(currentLevel === newLevel){
                    contentX = currentLevel * treeView.width
                }else{
                    currentLevel = newLevel
                }
            }
        }
    }
}
