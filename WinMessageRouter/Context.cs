using System;

namespace WinMessageRouter
{
	/// <summary>
	/// 
	/// </summary>
	public sealed class Context
	{
		private static Context me;
		private Logger logger;
		private ListenerRegistry listenerRegistry;
		private ConnectionRegistry connectionRegistry;

		private Context() 
		{
			logger = new Logger();
			connectionRegistry = new ConnectionRegistry();
			listenerRegistry = new ListenerRegistry();
		}

		public static Context getInstance()
		{
			if (me == null) 
			{
				me = new Context();
			}
			return me;
		}

		public Logger getLogger() 
		{
			return logger;
		}

		public ListenerRegistry getListenerRegistry() 
		{
			return listenerRegistry;
		}

		public ConnectionRegistry getConnectionRegistry() 
		{
			return connectionRegistry;
		}
	}
}
