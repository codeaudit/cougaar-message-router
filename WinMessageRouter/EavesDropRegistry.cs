using System;

using System.Threading;
using System.Collections;

namespace WinMessageRouter
{
	/// <summary>
	/// Summary description for EavesDropRegistry.
	/// </summary>
	public class EavesDropRegistry
	{
		private Mutex mutex = new Mutex();
		private Hashtable eavesDroppers = new Hashtable();
		private ArrayList globalEavesDroppers = new ArrayList();
 
		public EavesDropRegistry()
		{
			//
			// TODO: Add constructor logic here
			//
		}
		
		/**
		 * Register an eavesdropper connection for a desigated target
		 */
		public void registerEavesDropper(string target, ClientConnection eavesdropper) 
		{
			mutex.WaitOne();
			ArrayList targetEavesDroppers = (ArrayList)eavesDroppers[target];
			if (targetEavesDroppers == null) 
			{
				targetEavesDroppers = new ArrayList();
				eavesDroppers[target] = targetEavesDroppers;
			}
			targetEavesDroppers.Add(eavesdropper);
			mutex.ReleaseMutex();
		}

		/**
		 * Removes the designated eavesdropper from all lists
		 */
		public void deregisterAllEavesDroppers(ClientConnection eavesdropper) 
		{
			mutex.WaitOne();
			IDictionaryEnumerator e = eavesDroppers.GetEnumerator();
			while (e.MoveNext()) 
			{
				((ArrayList)(e.Current)).Remove(eavesdropper);
			}
			mutex.ReleaseMutex();
		}

		public void registerGlobalEavesDropper(ClientConnection eavesdropper) 
		{
			mutex.WaitOne();
			globalEavesDroppers.Add(eavesdropper);
			mutex.ReleaseMutex();
		}

		public void deregisterEavesDropper(string target, ClientConnection eavesdropper) 
		{
			mutex.WaitOne();
			ArrayList list = (ArrayList)eavesDroppers[target];
			if (list != null) 
			{
				list.Remove(eavesdropper);
			}
			mutex.ReleaseMutex();
		}

		public void removeTarget(string target) 
		{
			mutex.WaitOne();
			ArrayList targetEavesDroppers =(ArrayList) eavesDroppers[target];
			if (targetEavesDroppers != null) 
			{
				targetEavesDroppers.Clear();
			}
			eavesDroppers.Remove(target);
			mutex.ReleaseMutex();
		}

		public void deregisterGlobalEavesDropper(ClientConnection eavesdropper) 
		{
			mutex.WaitOne();
			globalEavesDroppers.Remove(eavesdropper);
			mutex.ReleaseMutex();
		}

		public void checkMessage(Message msg) 
		{
			mutex.WaitOne();
			sendLocalMessage(msg.From, msg);
			sendLocalMessage(msg.To, msg);
			sendGlobalMessage(msg);
			mutex.ReleaseMutex();
		}

		private void sendLocalMessage(string to, Message msg) 
		{
			ArrayList list = (ArrayList)eavesDroppers[to];
			if (list != null) 
			{
				IEnumerator e = list.GetEnumerator();
				while (e.MoveNext()) 
				{
					((ClientConnection)e.Current).sendMessage(msg);
				}
			}
		}

		private void sendGlobalMessage(Message msg) 
		{
			IEnumerator e = globalEavesDroppers.GetEnumerator();
			while (e.MoveNext()) 
			{
				((ClientConnection)e.Current).sendMessage(msg);
			}
		}
	}
}
