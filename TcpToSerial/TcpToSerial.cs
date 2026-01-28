using System;
using System.IO;
using System.IO.Ports;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

class Program
{
    static int TcpPort;
    static string SerialPortName = "";
    static int SerialBaudRate;

    static void Main()
    {
        LoadConfig("config.txt");

        using SerialPort serial = new SerialPort(SerialPortName, SerialBaudRate)
        {
            Encoding = Encoding.ASCII,
            NewLine = "\r",
            ReadTimeout = 1000,
            WriteTimeout = 1000
        };

        serial.Open();
        Console.WriteLine($"Seriale aperta: {SerialPortName} @ {SerialBaudRate}");

        TcpListener listener = new TcpListener(IPAddress.Any, TcpPort);
        listener.Start();
        Console.WriteLine($"In ascolto su TCP porta {TcpPort}...");

        while (true)
        {
            TcpClient client = listener.AcceptTcpClient();
            Console.WriteLine("Client connesso");

            ThreadPool.QueueUserWorkItem(_ => HandleClient(client, serial));
        }
    }

    static void HandleClient(TcpClient client, SerialPort serial)
    {
        using (client)
        using (NetworkStream stream = client.GetStream())
        {
            byte[] buffer = new byte[1024];
            int bytesRead;

            while ((bytesRead = stream.Read(buffer, 0, buffer.Length)) > 0)
            {
                string command = Encoding.ASCII.GetString(buffer, 0, bytesRead).Trim();
                Console.WriteLine($"Ricevuto: {command}");

                // Comandi speciali per aprire/chiudere la seriale
                switch (command.ToUpper())
                {
                    case "SERIAL_CLOSE":
                        if (serial.IsOpen)
                        {
                            serial.Close();
                            Console.WriteLine("Seriale chiusa");
                        }
                        break;

                    case "SERIAL_OPEN":
                        if (!serial.IsOpen)
                        {
                            serial.Open();
                            Console.WriteLine($"Seriale riaperta: {SerialPortName} @ {SerialBaudRate}");
                        }
                        break;

                    default:
                        if (serial.IsOpen)
                            serial.Write(command + "\r");
                        else
                            Console.WriteLine("Seriale chiusa, comando ignorato");
                        break;
                }
            }
        }

        Console.WriteLine("Client disconnesso");
    }

    static void LoadConfig(string path)
    {
        if (!File.Exists(path))
            throw new Exception("File di configurazione non trovato: " + path);

        foreach (string line in File.ReadAllLines(path))
        {
            if (string.IsNullOrWhiteSpace(line) || line.StartsWith("#"))
                continue;

            string[] parts = line.Split('=', 2);
            if (parts.Length != 2) continue;

            string key = parts[0].Trim();
            string value = parts[1].Trim();

            switch (key)
            {
                case "TcpPort":
                    TcpPort = int.Parse(value);
                    break;
                case "SerialPort":
                    SerialPortName = value;
                    break;
                case "BaudRate":
                    SerialBaudRate = int.Parse(value);
                    break;
            }
        }
    }
}