using System;
using System.Windows.Forms;

using QuartzTypeLib;

namespace TestProject
{
    /// <summary>
    /// 메인 폼
    /// </summary>
    public partial class VideoForm : Form
    {
        //////////////////////////////////////////////////////////////////////////////////////////////////// Field
        ////////////////////////////////////////////////////////////////////////////////////////// Private

        #region Field

        /// <summary>
        /// WM_APP
        /// </summary>
        private const int WM_APP = 0x8000;

        /// <summary>
        /// WM_GRAPHNOTIFY
        /// </summary>
        private const int WM_GRAPHNOTIFY = WM_APP + 1;

        /// <summary>
        /// EC_COMPLETE
        /// </summary>
        private const int EC_COMPLETE = 0x01;

        /// <summary>
        /// WS_CHILD
        /// </summary>
        private const int WS_CHILD = 0x40000000;

        /// <summary>
        /// WS_CLIPCHILDREN
        /// </summary>
        private const int WS_CLIPCHILDREN = 0x2000000;


        /// <summary>
        /// 필터 그래프 관리자
        /// </summary>
        private FilgraphManager filterGraphManager = null;

        /// <summary>
        /// 기본 오디오
        /// </summary>
        private IBasicAudio basicAudio = null;

        /// <summary>
        /// 비디오 윈도우
        /// </summary>
        private IVideoWindow videoWindow = null;

        /// <summary>
        /// 미디어 이벤트
        /// </summary>
        private IMediaEvent mediaEvent = null;

        /// <summary>
        /// 미디어 이벤트 확장
        /// </summary>
        private IMediaEventEx mediaEventEX = null;

        /// <summary>
        /// 미디어 위치
        /// </summary>
        private IMediaPosition mediaPosition = null;

        /// <summary>
        /// 미디어 컨트롤
        /// </summary>
        private IMediaControl mediaControl = null;

        /// <summary>
        /// 미디어 상태
        /// </summary>
        private MediaStatus mediaStatus = MediaStatus.NONE;

        #endregion

        //////////////////////////////////////////////////////////////////////////////////////////////////// Constructor
        ////////////////////////////////////////////////////////////////////////////////////////// Public

        #region 생성자 - MainForm()

        /// <summary>
        /// 생성자
        /// </summary>
        public VideoForm()
        {
            InitializeComponent();

            UpdateToolBar();
            UpdateStatusBar();

            FormClosing             += Form_FormClosing;
            SizeChanged             += Form_SizeChanged;
            this.openMenuItem.Click += openMenuItem_Click;
            this.exitMenuItem.Click += exitMenuItem_Click;
            this.playButton.Click   += playButton_Click;
            this.pauseButton.Click  += pauseButton_Click;
            this.stopButton.Click   += stopButton_Click;
            this.timer.Tick         += timer_Tick;
        }

        #endregion

        //////////////////////////////////////////////////////////////////////////////////////////////////// Method
        ////////////////////////////////////////////////////////////////////////////////////////// Protected

        #region 윈도우 프로시저 처리하기 - WndProc(message)

        /// <summary>
        /// 윈도우 프로시저 처리하기
        /// </summary>
        /// <param name="message">메시지</param>
        protected override void WndProc(ref Message message)
        {
            if(message.Msg == WM_GRAPHNOTIFY)
            {
                int eventCode;
                int parameter1;
                int parameter2;

                while(true)
                {
                    try
                    {
                        this.mediaEventEX.GetEvent(out eventCode, out parameter1, out parameter2, 0);

                        this.mediaEventEX.FreeEventParams(eventCode, parameter1, parameter2);

                        if(eventCode == EC_COMPLETE)
                        {
                            this.mediaControl.Stop();

                            this.mediaPosition.CurrentPosition = 0;

                            this.mediaStatus = MediaStatus.STOPPED;

                            UpdateToolBar();
                            UpdateStatusBar();
                        }
                    } 
                    catch(Exception)
                    {
                        break;
                    }
                }
            }

            base.WndProc(ref message);
        }

        #endregion

        ////////////////////////////////////////////////////////////////////////////////////////// Private
        //////////////////////////////////////////////////////////////////////////////// Event

        #region 폼 크기 변경시 처리하기 - Form_SizeChanged(sender, e)

        /// <summary>
        /// 폼 크기 변경시 처리하기
        /// </summary>
        /// <param name="sender">이벤트 발생자</param>
        /// <param name="e">이벤트 인자</param>
        private void Form_SizeChanged(object sender, EventArgs e)
        {
            if(this.videoWindow != null)
            {
                this.videoWindow.SetWindowPosition
                (
                    this.canvasPanel.ClientRectangle.Left,
                    this.canvasPanel.ClientRectangle.Top,
                    this.canvasPanel.ClientRectangle.Width,
                    this.canvasPanel.ClientRectangle.Height
                );
            }
        }

        #endregion
        #region 폼 닫을 경우 처리하기 - Form_FormClosing(sender, e)

        /// <summary>
        /// 폼 닫을 경우 처리하기
        /// </summary>
        /// <param name="sender">이벤트 발생자</param>
        /// <param name="e">이벤트 인자</param>
        private void Form_FormClosing(object sender, FormClosingEventArgs e)
        {
            ReleaseResource();
        }

        #endregion
        #region 열기 메뉴 항목 클릭시 처리하기 - openMenuItem_Click(sender, e)

        /// <summary>
        /// 열기 메뉴 항목 클릭시 처리하기
        /// </summary>
        /// <param name="sender">이벤트 발생자</param>
        /// <param name="e">이벤트 인자</param>
        private void openMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();

            openFileDialog.Filter = "미디어 파일|*.mpg;*.avi;*.wma;*.wmv;*.mov;*.wav;*.mp2;*.mp3|모든 파일|*.*";

            if(DialogResult.OK == openFileDialog.ShowDialog())
            {
                ReleaseResource();

                this.filterGraphManager = new FilgraphManager();

                this.filterGraphManager.RenderFile(openFileDialog.FileName);

                this.basicAudio = this.filterGraphManager as IBasicAudio;
                
                try
                {
                    this.videoWindow = this.filterGraphManager as IVideoWindow;

                    this.videoWindow.Owner       = (int)this.canvasPanel.Handle;
                    this.videoWindow.WindowStyle = WS_CHILD | WS_CLIPCHILDREN;

                    this.videoWindow.SetWindowPosition
                    (
                        this.canvasPanel.ClientRectangle.Left,
                        this.canvasPanel.ClientRectangle.Top,
                        this.canvasPanel.ClientRectangle.Width,
                        this.canvasPanel.ClientRectangle.Height
                    );
                }
                catch(Exception)
                {
                    this.videoWindow = null;
                }

                this.mediaEvent = this.filterGraphManager as IMediaEvent;

                this.mediaEventEX = this.filterGraphManager as IMediaEventEx;

                this.mediaEventEX.SetNotifyWindow((int) this.Handle,WM_GRAPHNOTIFY, 0);

                this.mediaPosition = this.filterGraphManager as IMediaPosition;

                this.mediaControl = this.filterGraphManager as IMediaControl;

                this.Text = "DirectShow를 사용해 동영상 재생하기 - [" + openFileDialog.FileName + "]";

                this.mediaControl.Run();

                mediaStatus = MediaStatus.RUNNING;

                UpdateToolBar();
                UpdateStatusBar();
            }
        }

        #endregion
        #region 종료 메뉴 항목 클릭시 처리하기 - exitMenuItem_Click(sender, e)

        /// <summary>
        /// 종료 메뉴 항목 클릭시 처리하기
        /// </summary>
        /// <param name="sender">이벤트 발생자</param>
        /// <param name="e">이벤트 인자</param>
        private void exitMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        #endregion
        #region 재생 버튼 클릭시 처리하기 - playButton_Click(sender, e)

        /// <summary>
        /// 재생 버튼 클릭시 처리하기
        /// </summary>
        /// <param name="sender">이벤트 발생자</param>
        /// <param name="e">이벤트 인자</param>
        private void playButton_Click(object sender, EventArgs e)
        {
            this.mediaControl.Run();

            this.mediaStatus = MediaStatus.RUNNING;

            UpdateToolBar();
            UpdateStatusBar();
        }

        #endregion
        #region 중지 버튼 클릭시 처리하기 - pauseButton_Click(sender, e)

        /// <summary>
        /// 중지 버튼 클릭시 처리하기
        /// </summary>
        /// <param name="sender">이벤트 발생자</param>
        /// <param name="e">이벤트 인자</param>
        private void pauseButton_Click(object sender, EventArgs e)
        {
            this.mediaControl.Pause();

            this.mediaStatus = MediaStatus.PAUSED;

            UpdateToolBar();
            UpdateStatusBar();
        }

        #endregion
        #region 중단 버튼 클릭시 처리하기 - stopButton_Click(sender, e)

        /// <summary>
        /// 중단 버튼 클릭시 처리하기
        /// </summary>
        /// <param name="sender">이벤트 발생자</param>
        /// <param name="e">이벤트 인자</param>
        private void stopButton_Click(object sender, EventArgs e)
        {
            this.mediaControl.Stop();

            this.mediaPosition.CurrentPosition = 0;

            this.mediaStatus = MediaStatus.STOPPED;

            UpdateToolBar();
            UpdateStatusBar();
        }

        #endregion
        #region 타이머 틱 처리하기 - timer_Tick(sender, e)

        /// <summary>
        /// 타이머 틱 처리하기
        /// </summary>
        /// <param name="sender">이벤트 발생자</param>
        /// <param name="e">이벤트 인자</param>
        private void timer_Tick(object sender, EventArgs e)
        {
            if(this.mediaStatus == MediaStatus.RUNNING)
            {
                UpdateStatusBar();
            }
        }

        #endregion

        //////////////////////////////////////////////////////////////////////////////// Function

        #region 리소스 해제하기 - ReleaseResource()

        /// <summary>
        /// 리소스 해제하기
        /// </summary>
        private void ReleaseResource()
        {
            if(this.mediaControl != null)
            {
                this.mediaControl.Stop();
            }

            this.mediaStatus = MediaStatus.STOPPED;

            if(this.mediaEventEX != null)
            {
                this.mediaEventEX.SetNotifyWindow(0, 0, 0);
            }

            if(this.videoWindow != null)
            {
                this.videoWindow.Visible = 0;
                this.videoWindow.Owner   = 0;
            }

            if(this.mediaControl != null)
            {
                this.mediaControl = null;
            }

            if(this.mediaPosition != null)
            {
                this.mediaPosition = null;
            }

            if(this.mediaEventEX != null)
            {
                this.mediaEventEX = null;
            }

            if(this.mediaEvent != null)
            {
                this.mediaEvent = null;
            }

            if(this.videoWindow != null)
            {
                this.videoWindow = null;
            }

            if(this.basicAudio != null)
            {
                this.basicAudio = null;
            }

            if(this.filterGraphManager != null)
            {
                this.filterGraphManager = null;
            }
        }

        #endregion
        #region 툴바 갱신하기 - UpdateToolBar()

        /// <summary>
        /// 툴바 갱신하기
        /// </summary>
        private void UpdateToolBar()
        {
            switch(this.mediaStatus)
            {
                case MediaStatus.NONE :
                
                    this.playButton.Enabled  = false;
                    this.pauseButton.Enabled = false;
                    this.stopButton.Enabled  = false;
                    
                    break;
                                          
                case MediaStatus.PAUSED :
                
                    this.playButton.Enabled  = true;
                    this.pauseButton.Enabled = false;
                    this.stopButton.Enabled  = true;
                    
                    break;
                                          
                case MediaStatus.RUNNING :
                
                    this.playButton.Enabled  = false;
                    this.pauseButton.Enabled = true;
                    this.stopButton.Enabled  = true;
                    
                    break;
                                          
                case MediaStatus.STOPPED :
                
                    this.playButton.Enabled  = true;
                    this.pauseButton.Enabled = false;
                    this.stopButton.Enabled  = false;
                    
                    break;
            }
        }

        #endregion
        #region 상태바 갱신하기 - UpdateStatusBar()

        /// <summary>
        /// 상태바 갱신하기
        /// </summary>
        private void UpdateStatusBar()
        {
            switch(this.mediaStatus)
            {
                case MediaStatus.NONE    : this.messageToolStripStatusLabel.Text = "중단";   break;
                case MediaStatus.PAUSED  : this.messageToolStripStatusLabel.Text = "중지";   break;
                case MediaStatus.RUNNING : this.messageToolStripStatusLabel.Text = "재생중"; break;
                case MediaStatus.STOPPED : this.messageToolStripStatusLabel.Text = "중단";   break;
            }

            if(this.mediaPosition != null)
            {
                int secondCount = (int) this.mediaPosition.Duration;
                int hourCount   = secondCount / 3600;
                int minuteCount = (secondCount  - (hourCount * 3600)) / 60;

                secondCount = secondCount - (hourCount * 3600 + minuteCount * 60);
                
                this.totalTimeToolStripStatusLabel.Text = string.Format("{0:D2}:{1:D2}:{2:D2}", hourCount, minuteCount, secondCount);

                secondCount = (int) this.mediaPosition.CurrentPosition;
                hourCount   = secondCount / 3600;
                minuteCount = (secondCount  - (hourCount * 3600)) / 60;
                secondCount = secondCount - (hourCount * 3600 + minuteCount * 60);
                
                this.playTimeToolStripStatusLabel.Text = String.Format("{0:D2}:{1:D2}:{2:D2}", hourCount, minuteCount, secondCount);
            }
            else
            {
                this.totalTimeToolStripStatusLabel.Text = "00:00:00";
                this.playTimeToolStripStatusLabel.Text  = "00:00:00";
            }
        }


        #endregion

        private void VideoForm_Load(object sender, EventArgs e)
        {

        }
    }
}