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
				if (args[i].Equals("-i")) 
				{
					Logger logger = Context.getInstance().getLogger();
					logger.CurrentLevel = Logger.LEVEL_INFO;
					logger.enable();
					System.Console.WriteLine("Debugging Enabled at level INFO");
				}
				if (args[i].Equals("-w")) 
				{
					Logger logger = Context.getInstance().getLogger();
					logger.CurrentLevel = Logger.LEVEL_WARN;
					logger.enable();
					System.Console.WriteLine("Debugging Enabled at level WARN");
				}
				if (args[i].Equals("-d")) 
				{
					Logger logger = Context.getInstance().getLogger();
					logger.CurrentLevel = Logger.LEVEL_DEBUG;
					logger.enable();
					System.Console.WriteLine("Debugging Enabled at level DEBUG");
				}
				if (args[i].Equals("-s")) 
				{
					Logger logger = Context.getInstance().getLogger();
					logger.CurrentLevel = Logger.LEVEL_SHOUT;
					logger.enable();
					System.Console.WriteLine("Debugging Enabled at level SHOUT");
				}
				else if (args[i].Equals("-r")) 
				{
					Context.getInstance().setAllowDuplicateConnections(false);
					System.Console.WriteLine("Disabling duplicate connections");
				}
				else if (args[i].Equals("-e")) 
				{
					Context.getInstance().enableEavesDropping();
					System.Console.WriteLine("Eavesdropping enabled");
				}
				else if (args[i].Equals("-m")) 
				{
					Context.getInstance().enableErrorMessages();
					Console.WriteLine("error messages enabled");
				}
				if (args[i].Equals("-h")) 
				{
					System.Console.WriteLine("Options are:");
					System.Console.WriteLine("   -b Block Read enabled");
					System.Console.WriteLine("   -i Info-level logging enabled");
					System.Console.WriteLine("   -w Wanr-level logging enabled");
					System.Console.WriteLine("   -d Debug-level logging enabled");
					System.Console.WriteLine("   -s Shout-level logging enabled");
					System.Console.WriteLine("   -r Refuse duplicate connections");
					System.Console.WriteLine("   -e Enable eavesdropping");
					System.Console.WriteLine("   -m Enable error messages");

					return;
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
				Context.getInstance().getLogger().log("System exiting due to exception: " + ex.Message, Logger.LEVEL_DEBUG);
			}
		}
	}
}
