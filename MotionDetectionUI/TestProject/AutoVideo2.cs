using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using AForge.Video.FFMPEG;

namespace TestProject
{
    public partial class AutoVideo2 : Form
    {
        public AutoVideo2()
        {
            InitializeComponent();
        }

        VideoFileSource _videoFile;
        private void button1_Click(object sender, EventArgs e)
        {
            if (this.button1.Text == "Start")
            {
                string filePath = @"C:\Users\Hot Dog\Desktop\Rehabot_영상\팔꿈치_재활운동용.mp4";
                _videoFile = new VideoFileSource(filePath);

                this.videoSourcePlayer1.VideoSource = _videoFile;
                this.videoSourcePlayer1.Start();

                this.button1.Text = "Stop";
            }
            else
            {
                this.button1.Text = "Start";
                _videoFile.Stop();
            }
        }
    }
}
