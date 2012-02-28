//------------------------------------------------------------------------
//  Readme.txt
//------------------------------------------------------------------------
This project is a codewarrior project which aim is to port CANopenNode
on freescale MCF5282.

It is NOT using the valid stack present in other directories for 2 reasons : 
- modifications have been done to local CANOpenNode stack, to avoir errors
due to variables definitions inside a function core
- codewarrior makes compulsary the use of a whole project directory

These issues should be solved later, by the followings:

- validation by official developpers of the edited stack (located in /mcf5282_ongoingdev/sources/CANopenStack)
- release of a working port

Laurent


