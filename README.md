**This project was done in a group of two(1. Max Sherwin[Student id:1128059] and 2. Keerthivasan Kalaivanan[Student id: 1190826])**

---------------------------------------------------------------------------------<<Chat Module Demo>>----------------------------------------------------------------------------------------------------------

- The project involves a simple chat program using multiple socket connections from the server to the various clients connected to the server. The clients can allocate, join and leave a room. Other clients can join the same room and chat with others in the room. 
- The messages sent from server to client or vice versa implement Big Endianness in their protocol when implementing serializing and deserializing in Buffer.
- Also the length of the message is added to the start of the message to implement "Length prefix message framing".
- The buffer is initialized to an N value(512 at start) and grows when packet size exceeds. 
- The server is made to act concurrently. Uses select() to handle multiple connections.
- The server broadcasts to all clients in the room when a new client joins or when a previously existing client exits the room.
- Also the logs of every connection and room allocated is displayed on the server terminal. The server makes note of the client's name, socket id and the room allocated to them.
- The project makes use of the CLI User Interface in the best way possible.

-----------------------------------------------------------------------------<<How to build and use it>>-------------------------------------------------------------------------------------------------------

- Open the solution found in the path "ChatModuleDemo/ChatModuleDemo.sln". Once opened, press 'Build' on the "TCPServer" Project to build the entire solution with the solution configuration being "Debug" and the solution platform being "x64". Similarly, press 'Build' on "TCPClient" Project to build the entire solution with the same solution configuration and solution platform.

- Once the builds are a success, the .exe files on the path "ChatModuleDemo/x64/Debug/TCPServer.exe" and "ChatModuleDemo/x64/Debug/TCPServer.exe" can be opened to run the program.

- The "TCPServer" is run first and only once. Then followed by the "TCPClient", which can be run/opened multiple times to create multiple clients connecting to the server.

- Once the terminals open, the socket connections and the success on connecting is displayed and the instructions as to how to operate the user interface for the client is also shown.

- Providing the same instructions below :
	- At first the client asks for the user's name
	- After that, User can press "Escape"/'ESC' to see the options available.
	- Press 'J' to join a room or press 'ESC' again to cancel option loadout.
	- Once 'J' is pressed, the interface prompts for the room name.
	- After typing the room name, chat is enabled for the room. Other users can join the same room by entering the same room name when prompted.
	- By pressing 'TAB', the user will be able to chat with other users in the room. 
	- Else, user can press 'ESC' for more options. 
	- Press 'E' to exit the room or press 'ESC' again to cancel option loadout.
	- Once exited the room, user can join again to the same room or a different one by pressing 'ESC' and 'J' keys.

-----------------------------------------------------------------------<<Conclusion - Our thoughts on the project>>--------------------------------------------------------------------------------------------

- This is a very simple project. We wanted to add more functionality to it. But keeping it simple felt neater since the User Interface was the CLI(Command Line Interface). Expanding the project more is a bit tough job because of the command line terminal being used as the UI.

- There are a few limitations to the project. The program doesnt allow a single client to join multiple rooms. We felt the UI would become complicated if we do so. 

- Also closing a Client terminal terminates the whole program's functionality. We tried for a workaround, but terminating the whole program(including server and other clients) was the only solution we could come up it as we pushed this task to the very last. 

- We would very much like to use ImGUI to expand the project so that more functionality could be added. Functionalities like joining multiple rooms, displaying list of rooms available, displaying a separate chat screen, etc. 

- We have also used erase functionality in the program to erase the previous lines on the terminal i.e. when 'ESC' is pressed and further options are displayed, clicking on it will erase the previously lines and display the click action event.

- We will make sure that the knowledge we have gained on this project will be used to implement something better in the future. 
- Enjoy playing around with the Chat Module Demo :)