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
		private EavesDropRegistry eavesdropRegistry;
		private bool errorMessagesAllowed = false;
		private bool eavesDroppingAllowed = false;
		private bool allowDuplicateConnections = true;
		private int maxSendQueueSize = 5000;
		private bool syncSend = false;
		private bool statLogging = false;

		private Context() 
		{
			logger = new Logger();
			logger.start();
			connectionRegistry = new ConnectionRegistry();
			listenerRegistry = new ListenerRegistry();
			eavesdropRegistry = new EavesDropRegistry();
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

		public EavesDropRegistry getEavesDropRegistry() 
		{
			return eavesdropRegistry;
		}

		public void setAllowDuplicateConnections(bool b) 
		{
			allowDuplicateConnections = b;
		}

		public bool getAllowDuplucateConnections() 
		{
			return allowDuplicateConnections;
		}

		public void enableEavesDropping() 
		{
			eavesDroppingAllowed = true;
		}

		public bool isEavesDroppingEnabled() 
		{
			return eavesDroppingAllowed;
		}

		public void disableEavesdropping() 
		{
			eavesDroppingAllowed = false;
		}

		public void enableErrorMessages() 
		{
			errorMessagesAllowed = true;
		}

		public void disableErrorMessages() 
		{
			errorMessagesAllowed = false;
		}

		public bool errorMessageEnabled() 
		{
			return errorMessagesAllowed;
		}
		public void logToFile() 
		{
			logger.use_output_file = true;
		}
		public void logToStdOut() 
		{
			logger.use_output_file = false;
		}
		public int getMaxSendQueueSize() 
		{
			return maxSendQueueSize;
		}
		public void setMaxSendQueueSize(int val) 
		{
			maxSendQueueSize = val;
		}
		public void setSyncSend(bool b) 
		{
			syncSend = b;
		}
		public bool getSyncSend() 
		{
			return syncSend;
		}
		public void enableStatLogging() 
		{
			statLogging = true;
		}
		public void disableStatLogging() 
		{
			statLogging = false;
		}
		public bool getStatLogging() 
		{
			return statLogging;
		}
	}
}
