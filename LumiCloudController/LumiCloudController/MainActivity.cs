using System;
using System.Linq;
using System.Text;
using Android.App;
using Android.Bluetooth;
using Android.Content;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Android.OS;

using Java.Util;

namespace LumiCloudController
{
    [Activity(Label = "LumiCloud", MainLauncher = true, Icon = "@drawable/icon")]
    public class MainActivity : Activity
    {
        BluetoothAdapter adapter = null;
        BluetoothDevice device = null;
        BluetoothSocket socket = null;
        //string cloudBTName = "HC-06";
        string cloudBTAddress = "98:D3:31:FB:26:25";
        char mode = '0';
        //short retry = 0;

        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);

            // Set our view from the "main" layout resource
            SetContentView(Resource.Layout.Main);

            Button btnMode0 = FindViewById<Button>(Resource.Id.btnMode0);
            Button btnModea = FindViewById<Button>(Resource.Id.btnModea);
            Button btnModec = FindViewById<Button>(Resource.Id.btnModec);
            Button btnModeo = FindViewById<Button>(Resource.Id.btnModeo);
            Button btnModel = FindViewById<Button>(Resource.Id.btnModel);
            Button btnModep = FindViewById<Button>(Resource.Id.btnModep);
            Button btnModer = FindViewById<Button>(Resource.Id.btnModer);
            Button btnModes = FindViewById<Button>(Resource.Id.btnModes);
            Button btnModem = FindViewById<Button>(Resource.Id.btnModem);

            btnMode0.Click += delegate { ChangeBtnStates(false); UpdateMode('0'); };
            btnModea.Click += delegate { ChangeBtnStates(false); UpdateMode('a'); };
            btnModec.Click += delegate { ChangeBtnStates(false); UpdateMode('c'); };
            btnModeo.Click += delegate { ChangeBtnStates(false); UpdateMode('o'); };
            btnModel.Click += delegate { ChangeBtnStates(false); UpdateMode('l'); };
            btnModep.Click += delegate { ChangeBtnStates(false); UpdateMode('p'); };
            btnModer.Click += delegate { ChangeBtnStates(false); UpdateMode('r'); };
            btnModes.Click += delegate { ChangeBtnStates(false); UpdateMode('s'); };
            btnModem.Click += delegate { ChangeBtnStates(false); UpdateMode('m'); };

            InitBT();
        }

        private void UpdateMode(char c)
        {
            mode = c;

            socket.OutputStream.WriteByte((byte)mode);
            ChangeBtnStates(true);
        }

        async private void InitBT()
        {
            adapter = BluetoothAdapter.DefaultAdapter;
            if (adapter == null)
                throw new Exception("No Bluetooth adapter found.");

            if (!adapter.IsEnabled)
                throw new Exception("Bluetooth adapter is not enabled.");

            device = (from bd in adapter.BondedDevices where string.Compare(bd.Address, cloudBTAddress, true)==0 select bd).FirstOrDefault();//(from bd in adapter.BondedDevices where bd.Name == cloudBTName select bd).FirstOrDefault();
            if (device == null)
                throw new Exception("Named device not found.");

            socket = device.CreateRfcommSocketToServiceRecord(UUID.FromString("00001101-0000-1000-8000-00805f9b34fb"));
            await socket.ConnectAsync();
            ChangeBtnStates(true);
        }

        //public override void OnActivityReenter(int resultCode, Intent data)
        //{
        //    base.OnActivityReenter(resultCode, data);
        //    ErrorDialog("reenter");
        //}

        //protected override void OnRestart()
        //{
        //    base.OnRestart();
        //    ErrorDialog("restart");
        //}

        protected override void OnResume()
        {
            base.OnResume();
            return;
            ChangeBtnStates(false);

            if (socket != null && !socket.IsConnected)
                socket.Dispose();
            else if (socket != null)
            {
                ChangeBtnStates(true);
                return;
            }

            adapter = BluetoothAdapter.DefaultAdapter;
            if (adapter == null)
                throw new Exception("No Bluetooth adapter found.");

            if (!adapter.IsEnabled)
                throw new Exception("Bluetooth adapter is not enabled.");

            device = (from bd in adapter.BondedDevices where string.Compare(bd.Address, cloudBTAddress, true) == 0 select bd).FirstOrDefault();//(from bd in adapter.BondedDevices where bd.Name == cloudBTName select bd).FirstOrDefault();
            if (device == null)
                throw new Exception("Named device not found.");

            socket = device.CreateRfcommSocketToServiceRecord(UUID.FromString("00001101-0000-1000-8000-00805f9b34fb"));
            socket.Connect();
            ChangeBtnStates(true);

        }


        private void ErrorDialog(string message)
        {
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            AlertDialog dialog = builder.Create();
            dialog.SetTitle("Error");
            dialog.SetMessage(message);
            dialog.Show();
        }

        private void ChangeBtnStates(bool enabled)
        {
            FindViewById<Button>(Resource.Id.btnMode0).Enabled = enabled;
            FindViewById<Button>(Resource.Id.btnModea).Enabled = enabled;
            FindViewById<Button>(Resource.Id.btnModec).Enabled = enabled;
            FindViewById<Button>(Resource.Id.btnModeo).Enabled = enabled;
            FindViewById<Button>(Resource.Id.btnModel).Enabled = enabled;
            FindViewById<Button>(Resource.Id.btnModep).Enabled = enabled;
            FindViewById<Button>(Resource.Id.btnModer).Enabled = enabled;
            FindViewById<Button>(Resource.Id.btnModes).Enabled = enabled;
            FindViewById<Button>(Resource.Id.btnModem).Enabled = enabled;
        }
    }
}

