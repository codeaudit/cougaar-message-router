using System;
using System.Collections;

namespace WinMessageRouter
{
	/// <summary>
	/// 
	/// </summary>
	public class ConnectionRegistry
	{
		private Hashtable map = new Hashtable();

		public ConnectionRegistry()
		{
		}

		public void registerConnection(ClientConnection cc, string name) 
		{
			map[name] = cc;
		}

		public ClientConnection findConnection(string name) 
		{
			return (ClientConnection)map[name];
		}

		public void deregisterConnection(string name) 
		{
			map.Remove(name);
		}

		public string listConnections() 
		{
			string ret = "";

			IEnumerator i = map.Keys.GetEnumerator();
			while (i.MoveNext()) 
			{
				ret += ((string)i.Current) + "\n";
			}

			return ret;
		}
	}
}
