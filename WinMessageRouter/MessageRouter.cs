using System;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace WinMessageRouter
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	class MessageRouter
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		//[STAThread]
		static void Main(string[] args)
		{
			Console.WriteLine("STARTED");
			bool use_block_read = false;
			for (int i=0; i<args.Length; i++) 
			{
				if (args[i].Equals("-b")) 
				{
					use_block_read = true;
					System.Console.WriteLine("Block-read enabled.");
				}
				if (args[i].Equals("-d")) 
				{
					Logger logger = Context.getInstance().getLogger();
					logger.enable();
					System.Console.WriteLine("Debugging Enabled.");
				}
			}

			try 
			{
				Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);	
				server.Bind(new IPEndPoint(IPAddress.Any, 6667));
				server.Listen(30);
				while (true) 
				{
					Socket serverSocket = server.Accept();
					ClientConnection cc = new ClientConnection(serverSocket, use_block_read);
					cc.start();
				}
			}
			catch (Exception ex) 
			{
				Console.WriteLine(ex.Message);
				Context.getInstance().getLogger().log("System exiting due to exception: " + ex.Message);
			}
		}
	}
}
