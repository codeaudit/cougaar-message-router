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
			bool use_block_read = false;
			for (int i=0; i<args.Length; i++) 
			{
				if (args[i].Equals("-b")) 
				{
					use_block_read = true;
					Context.getInstance().getLogger().forceLog("Block-read enabled.");
				}
				if (args[i].Equals("-i")) 
				{
					Logger logger = Context.getInstance().getLogger();
					logger.CurrentLevel = Logger.LEVEL_INFO;
					logger.enable();
					Context.getInstance().getLogger().forceLog("Debugging Enabled at level INFO");
				}
				if (args[i].Equals("-w")) 
				{
					Logger logger = Context.getInstance().getLogger();
					logger.CurrentLevel = Logger.LEVEL_WARN;
					logger.enable();
					Context.getInstance().getLogger().forceLog("Debugging Enabled at level WARN");
				}
				if (args[i].Equals("-d")) 
				{
					Logger logger = Context.getInstance().getLogger();
					logger.CurrentLevel = Logger.LEVEL_DEBUG;
					logger.enable();
					Context.getInstance().getLogger().forceLog("Debugging Enabled at level DEBUG");
				}
				if (args[i].Equals("-s")) 
				{
					Logger logger = Context.getInstance().getLogger();
					logger.CurrentLevel = Logger.LEVEL_SHOUT;
					logger.enable();
					Context.getInstance().getLogger().forceLog("Debugging Enabled at level SHOUT");
				}
				else if (args[i].Equals("-r")) 
				{
					Context.getInstance().setAllowDuplicateConnections(false);
					Context.getInstance().getLogger().forceLog("Disabling duplicate connections");
				}
				else if (args[i].Equals("-e")) 
				{
					Context.getInstance().enableEavesDropping();
					Context.getInstance().getLogger().forceLog("Eavesdropping enabled");
				}
				else if (args[i].Equals("-m")) 
				{
					Context.getInstance().enableErrorMessages();
					Context.getInstance().getLogger().forceLog("error messages enabled");
				}
				else if (args[i].IndexOf("-l=") != -1) 
				{
					Context.getInstance().getLogger().setLogFilePath(args[i].Substring(3));
				}
				else if (args[i].Equals("-g")) 
				{
					Context.getInstance().getLogger().disable();
					System.Console.WriteLine("Logging disabled");
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
				Context.getInstance().getLogger().forceLog("MessageRouter Started");

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
