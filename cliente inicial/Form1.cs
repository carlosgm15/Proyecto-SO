using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.IO;

namespace Proyecto_V1
{
    public partial class Form1 : Form
    {
        Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        IPEndPoint remoteEP = new IPEndPoint(IPAddress.Parse("192.168.56.102"), 9062);
        byte[] data = new byte[1024];

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            
            try 
            { 
                socket.Connect(remoteEP);
                this.BackColor = Color.Green;
            }
            catch (SocketException ee) 
            { 
                Console.WriteLine("Unable to connect to server. "); 
                Console.WriteLine(ee); 
                return; 
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            //logear
            String registro = "0/" + B_Mail.Text + "/" + B_Password.Text + "\0";
            socket.Send(Encoding.ASCII.GetBytes(registro));

            int dataSize = socket.Receive(data);
            string resultado = (Encoding.ASCII.GetString(data, 0, dataSize));

            if (resultado == "0\0") //Tengo que consieguir poder quitar esto
            {
                MessageBox.Show("Correctamente Logueado");
                MessageBox.Show(resultado);
            }
            else
            {
                MessageBox.Show("Ha habido algun error con el Log In");
                MessageBox.Show(resultado);
            }

        }

        private void dataGridView1_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        /*private void button3_Click(object sender, EventArgs e)
        {
            String registro = "0/" + B_Mail.Text + "/" + B_Password.Text + "\0";
            socket.Send(Encoding.ASCII.GetBytes(registro));

            int dataSize = socket.Receive(data);
            string resultado = (Encoding.ASCII.GetString(data, 0, dataSize));
        } */

        private void B_cuestionar_Click(object sender, EventArgs e)
        {
            if (C_1.Checked) //Dar el jugador que es  mayor de edad
            {
                String registro = "1/1";
                // Enviamos al servidor el nombre tecleado
                socket.Send(Encoding.ASCII.GetBytes(registro));

                //Recibimos la respuesta del servidor
                int dataSize = socket.Receive(data);
                string resultado = (Encoding.ASCII.GetString(data, 0, dataSize));
                MessageBox.Show("El nombre del jugador mayor de edad es : " + resultado);
            }

            if (C_2.Checked) //Dar el id del jugador con el mayor puntaje de la partida
            {
                String registro = "2/";
                // Enviamos al servidor el nombre tecleado
                socket.Send(Encoding.ASCII.GetBytes(registro));

                //Recibimos la respuesta del servidor
                int dataSize = socket.Receive(data);
                string resultado = (Encoding.ASCII.GetString(data, 0, dataSize));
                MessageBox.Show("El id del jugador con mayor puntaje es:" + resultado);
            }

            if (C_3.Checked) //Dar la divison del jugador 
            {
                String registro = "3/" + cuestionador.Text;
                // Enviamos al servidor el nombre tecleado
                socket.Send(Encoding.ASCII.GetBytes(registro));

                //Recibimos la respuesta del servidor
                int dataSize = socket.Receive(data);
                string resultado = (Encoding.ASCII.GetString(data, 0, dataSize));
                MessageBox.Show("La division del jugador : " + cuestionador.Text + " es:" + resultado);
            }
        }

    }
}
