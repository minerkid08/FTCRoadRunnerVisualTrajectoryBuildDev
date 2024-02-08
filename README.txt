How two make path

1. Shift click the field to place nodes
2. Click on the starting node to select it
3. Ctrl click on the rest of the nodes in path order to connect them
4. Edit the settings of the nodes and path segments by clicking on them then editing the settings at the bottom of the ui panel

Click the save as button to save the path
Enter the name of the path in the text box then click the + button
Once it is saved once then the save button can be used to save the path

After the path is saved press the export button to export the path

Node settings
1. pos: position of the node
2. rot: rotation of the node used for spline pathing
3. heading: heading of the robot, only used if the heading is linear
4. layer: used to group nodes

Segments settings
1. layer: used to group path segments.
2. heading mode: change how the heading moves over time
3. path type: change the type of path

Layer system
Global layer: layer set in the top part of the ui
Object layer: layer set in the object settings
Objects can only be selected if the global layer is equal to the object layer
When the global layer is -1 all objects can be selected
Objects that cant be selected will be transparent