# file-transfer-test

An MS Visual Studio 2022 Windows client-server console C++ solution, based on Boost::Asio.  
The client/server file transfer connection is performed via TCP sockets, using SSL encryption.  

To test the solution:  
You can do that from inside the "bin" subfolder, which contains the executables and source folders that contain JPEG files for the test transfer.  
The "bin" folder also contains the OpenSLL dlls and ONLY the server certificate files. The client SSL certificate is loaded from memory and is not kept as a file at the client side.   

1. The server can be started by start_server.bat. It will look for source files into the specified source folders as parameters (check bat file for details).   
By default the server starts listening on port 1234 which could be specified as a parameter too (check bat file for details).   
It will create a Cache folder as specified in bat file, as well.   
As an example there are 4 Source folders: Source, Source1, Source2, Source3 with corresponding jpeg test files into each one of them.  
The server caches the files to be sent in the Cache subfolder prior to sending them to the client(s).  
Each client can choose to "Invalidate" the cached file(s) on the server's side.  
  
2. Client(s) can be started via the start_client1 and start_client2 bat files.  
By default the client tries to connect to localhost on port 1234 which could be specified as a parameter (check bat file for details).   
Clients can be started on remote machines to connect to the server rempotely by supplying the server IP address as the first parameter (blank by default meaning localhost).
You only need to copy the Client.exe and the OpenSSL dlls to a remote machine for testing. The start_client bat file can also be taken to the remote machine for convenience as well.  
  
Each client will create its folder (ClientSroe by default or Client1 and Client2 according to bat file parameters) to store the received files.  
More clients could be easilly started by creating a start_client3.bat with Client3 folder for instance, etc..  
  
Each client will start making the following request cycle in a continuous loop (until stopped with Esc keypress):  
Get 1.jpg  
Get Info 1.jpg  
Invalidate 1.jpg  
Get 2.jpg  
Get Info 2.jpg  
Get 3.jpg  
Get Info 3.jpg  
Invalidate All  
  
The server will try to locate (into Source folders) and copy the requested file if it is not found into its Cache folder prior to sending it (or just its info) to the corresponding client.  
  
The solution allows for two or more clients to receive the same file simultanously, while Invalidate requests are done only when no files are being processed (get/info). Invalidate requests wait for any Get/Get Info requests to be completed first.   
"Invalidate" means to ask the server to remove the file(s) from its Cache folder and get them from the source folders anew.   
  
You'll be able to observe how the files are constantly being copied and erased into the Cache folder and overwritten into each client's folder.  
  
Do not run more than one instance from each bat file because it will overwrite the folders' contents.  
  
  
IMPORTANT:  
If you don't have Visual Studio 2022 installed, you might need to install Visual Studio 2022 C++ X86 [https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170#visual-studio-2015-2017-2019-and-2022] redistributable in order to test the executables directly from "bin" folder.   
NOTE: You need to install only the X86 (32 bit) version of the Visual Studio 2022 C++ redist library.  
  
To build the solution (refer to the Folder_struct.png):  
1. boost_1_83_0 (latest) [https://www.boost.org/users/download/] needs to be installed into the solution folder. Download the windows archive and unzip the root boost folder into the solution folder. Rename the unzipped boost folder (currently boost_1_83_0) to "boost_latest". Run bootstrap.bat first and then b2.exe after that.  
2. OpenSSL for Windows (32 bit version) also needs to be installed. The installer can be obtained from here: Win32 OpenSSL v3.2.0 (exe) [https://slproweb.com/products/Win32OpenSSL.html]. Install OpenSSL_Win32 inside the solution folder.    
  
The solution provides only 32 bit versions of client/server apps. 64 bit versions could be easilly added too if needed.   
  
