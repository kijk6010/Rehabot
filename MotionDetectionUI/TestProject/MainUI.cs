using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using QuartzTypeLib;

namespace TestProject
{
    public partial class MainUI : Form
    {
        public MainUI()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Point parentPoint = this.Location;

            VideoForm vf = new VideoForm();
            vf.StartPosition = FormStartPosition.Manual;
            vf.Location = new Point(0, 0);
            
            vf.Show();
            

        }

        private void button2_Click(object sender, EventArgs e)
        {
            Point parentPoint = this.Location;

            AutoVideo1 av1 = new AutoVideo1();
            av1.StartPosition = FormStartPosition.Manual;
            av1.Location = new Point(0, 0);

            av1.Show();
            System.Diagnostics.Process ps = new System.Diagnostics.Process();
            ps.StartInfo.FileName = "C:\\Users\\Hot Dog\\Desktop\\Release_어깨\\OPENCVproject.exe";

            ps.Start();

        }

        private void button3_Click(object sender, EventArgs e)
        {
            Point parentPoint = this.Location;

            AutoVideo2 av2 = new AutoVideo2();
            av2.StartPosition = FormStartPosition.Manual;
            av2.Location = new Point(0, 0);

            av2.Show();
            System.Diagnostics.Process ps = new System.Diagnostics.Process();
            ps.StartInfo.FileName = "C:\\Users\\Hot Dog\\Desktop\\Release_팔꿈치\\OPENCVproject.exe";
            ps.Start();
        }
    }
}


