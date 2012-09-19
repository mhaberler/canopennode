Usage of CANopen master:
Copy contents from diskA to DB240 demo board. Reboot, open homepage and use:
 - CAN log,
 - CANopen emergency log,
 - SDO master,
 - NMT master,
 - custom CAN message.


Update location of the project for One Workbench project files.

Edit \Example_IO\.project file:
Near the end of the file set <location> tag to location of the CANopen_stack in your filesystem.
For example: <location>D:/_/Dokumenti/CANopen/CANopen_stack</location>
Relative paths don't work ???
