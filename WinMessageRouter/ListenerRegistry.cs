using System;
using System.Collections;

namespace WinMessageRouter
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	public class ListenerRegistry
	{
		ArrayList listeners = new ArrayList();

		public ListenerRegistry()
		{
		}

		public void registerListener(ClientConnection cc) 
		{
			listeners.Add(cc);
		}

		public void deregisterListener(ClientConnection cc) 
		{
			listeners.Remove(cc);
		}

		public void notifyListeners(Message msg) 
		{
			for (int i=0; i<listeners.Count; i++ ) 
			{
				((ClientConnection)listeners[i]).sendMessage(msg);
			}
		}
	}
}
