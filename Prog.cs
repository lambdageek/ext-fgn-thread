using System;
using System.Threading;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

delegate void MyDelegate ();

public class Prog {

	[DllImport("Foo", EntryPoint="FooStart")]
	extern static void FooStart (MyDelegate d);
	
	public static void Main () {
		var d = new MyDelegate (SomeMethod);
		d ();

		FooStart (d);
		
		// var w = new Thread (() => { d (); });
		//w.Start ();
		// w.Join ();
		var w = new Thread (() => { Thread.CurrentThread.IsBackground = true; Thread.Sleep (1 * 1000); } );
		w.Start ();
		Thread.Sleep (100);
		return;
	}

	public static void SomeMethod () {
		var t = Thread.CurrentThread;
		Console.WriteLine ("Called from: {0} ({1})", t.ManagedThreadId,
				   t.IsBackground ? "Background" : "Not Background");
	}

	public static void SomeOtherMethod () {
		var t = Thread.CurrentThread;
		Console.Write ("Some Other Method {0}>>> ", t.IsBackground ?"Background" : "Not Background");
		if (!t.IsBackground) {
			Console.Write (">>> setting background >>> ");
			t.IsBackground = true;
		}
		SomeMethod ();
	}
}
