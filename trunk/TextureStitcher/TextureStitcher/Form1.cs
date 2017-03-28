using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace TextureStitcher
{
    public partial class Form1 : Form
    {
        ListBox fileList;
        int timePerFrame = 1;
        int numVertFrames = 1;

        /// <summary>
        /// Constructor.  Initializes the form layout.
        /// </summary>
        public Form1()
        {
            InitializeComponent();

            this.Height = 350;

            fileList = new ListBox();
            fileList.Height = 195;
            fileList.Width = 200;
            fileList.Top = 5;
            fileList.Left = (this.Width / 2) - (fileList.Width / 2);
            this.Controls.Add(fileList);

            Button fileUpButton = new Button();
            fileUpButton.Top = fileList.Top + fileList.Height / 2 - 40;
            fileUpButton.Left = fileList.Right + 5;
            fileUpButton.Width = 20;
            fileUpButton.Text = "^";
            fileUpButton.Click += new EventHandler(fileUpButton_Click);
            this.Controls.Add(fileUpButton);

            Button fileDownButton = new Button();
            fileDownButton.Top = fileUpButton.Bottom + 20;
            fileDownButton.Left = fileUpButton.Left;
            fileDownButton.Width = 20;
            fileDownButton.Text = "v";
            fileDownButton.Click += new EventHandler(fileDownButton_Click);
            this.Controls.Add(fileDownButton);

            Button addFileButton = new Button();
            addFileButton.Top = fileList.Bottom + 5;
            addFileButton.Left = (this.Width / 2) - (addFileButton.Width);
            addFileButton.Text = "Add";
            addFileButton.Click += new EventHandler(addFileButton_Click);
            this.Controls.Add(addFileButton);

            Button removeFileButton = new Button();
            removeFileButton.Top = addFileButton.Top;
            removeFileButton.Left = addFileButton.Right + 5;
            removeFileButton.Text = "Remove";
            removeFileButton.Click += new EventHandler(removeFileButton_Click);
            this.Controls.Add(removeFileButton);

            Label frameTimeLabel = new Label();
            frameTimeLabel.Top = addFileButton.Bottom + 5;
            frameTimeLabel.Left = addFileButton.Left;
            frameTimeLabel.Width = 80;
            frameTimeLabel.Text = "Time per frame:";
            this.Controls.Add(frameTimeLabel);

            NumericUpDown frameTimeNUD = new NumericUpDown();
            frameTimeNUD.Top = addFileButton.Bottom + 5;
            frameTimeNUD.Left = frameTimeLabel.Right + 3;
            frameTimeNUD.Width = 70;
            frameTimeNUD.Minimum = 1;
            frameTimeNUD.ValueChanged += new EventHandler(frameTimeNUD_ValueChanged);
            this.Controls.Add(frameTimeNUD);

            Label modeLabel = new Label();
            modeLabel.Top = frameTimeNUD.Bottom + 5;
            modeLabel.Left = frameTimeLabel.Left;
            modeLabel.Width = 120;
            modeLabel.Text = "Set Horizontal Mode: ";
            this.Controls.Add(modeLabel);

            CheckBox modeBox = new CheckBox();
            modeBox.Name = "ModeBox";
            modeBox.Top = modeLabel.Top;
            modeBox.Left = modeLabel.Right + 5;
            modeBox.Width = 20;
            modeBox.Click += new EventHandler(modeBox_Click);
            this.Controls.Add(modeBox);

            Button stitchFileButton = new Button();
            stitchFileButton.Top = modeLabel.Bottom + 5;
            stitchFileButton.Left = (this.Width / 2) - (stitchFileButton.Width / 2);
            stitchFileButton.Text = "Stitch";
            stitchFileButton.Click += new EventHandler(stitchFileButton_Click);
            this.Controls.Add(stitchFileButton);
        }

        /// <summary>
        /// Event handler for toggling frame mode.  This is super hacky and should really be a vector of frame lists.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void modeBox_Click(object sender, EventArgs e)
        {
            CheckBox sentBox = (CheckBox)sender;
            if (sentBox.Checked)
            {
                NumericUpDown vertFrameNUD = new NumericUpDown();
                vertFrameNUD.Name = "VertFrameNUD";
                vertFrameNUD.Top = sentBox.Top;
                vertFrameNUD.Left = sentBox.Right + 5;
                vertFrameNUD.Width = 50;
                vertFrameNUD.Minimum = 1;
                vertFrameNUD.ValueChanged += new EventHandler(vertFrameNUD_ValueChanged);
                this.Controls.Add(vertFrameNUD);
            }
            else
            {
                this.Controls.RemoveByKey("VertFrameNUD");
            }
        }

        /// <summary>
        /// Event handler for the box to set number of vertical frames when in horizontal mode.  Another hack!
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void vertFrameNUD_ValueChanged(object sender, EventArgs e)
        {
            numVertFrames = (int)((NumericUpDown)sender).Value;
        }

        /// <summary>
        /// Event handler for moving a frame down the list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void fileDownButton_Click(object sender, EventArgs e)
        {
            if (fileList.SelectedIndex >= 0)
            {
                if (fileList.SelectedIndex + 1 < fileList.Items.Count)
                {
                    object temp = fileList.Items[fileList.SelectedIndex];
                    fileList.Items[fileList.SelectedIndex] = fileList.Items[fileList.SelectedIndex + 1];
                    fileList.Items[fileList.SelectedIndex + 1] = temp;
                    fileList.SelectedIndex++;
                }
            }
        }

        /// <summary>
        /// Event handler for moving a frame up the list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void fileUpButton_Click(object sender, EventArgs e)
        {
            if (fileList.SelectedIndex >= 0)
            {
                if (fileList.SelectedIndex - 1 > -1)
                {
                    object temp = fileList.Items[fileList.SelectedIndex];
                    fileList.Items[fileList.SelectedIndex] = fileList.Items[fileList.SelectedIndex - 1];
                    fileList.Items[fileList.SelectedIndex - 1] = temp;
                    fileList.SelectedIndex--;
                }
            }
        }

        /// <summary>
        /// Event handler to update number of frames.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void frameTimeNUD_ValueChanged(object sender, EventArgs e)
        {
            timePerFrame = (int)((NumericUpDown)sender).Value;
        }

        /// <summary>
        /// Event handler for stitching button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void stitchFileButton_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFile = new SaveFileDialog();
            saveFile.Filter = "PNG Image Files (*.png)|*.png";
            saveFile.FileOk += new CancelEventHandler(saveFile_FileOk);
            saveFile.ShowDialog();
        }

        /// <summary>
        /// Logic for texture stitching.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void saveFile_FileOk(object sender, CancelEventArgs e)
        {
            string curFile = ((SaveFileDialog)sender).FileName;

            List<Image> images = new List<Image>();
            int imageHeight = 0;
            int imageWidth = 0;

            Control[] tempControls = this.Controls.Find("ModeBox", true);
            bool bHorMode = false;

            if (tempControls.Length > 0)
            {
                bHorMode = ((CheckBox)tempControls[0]).Checked;
            }

            if (bHorMode)
            {
                for (int i = 0; i < fileList.Items.Count; ++i)
                {
                    images.Add(Image.FromFile((string)fileList.Items[i]));
                    imageHeight += images[i].Height;

                    if (images[i].Width > imageWidth)
                    {
                        imageWidth = images[i].Width;
                    }
                }

                Bitmap bmp = new Bitmap(imageWidth, imageHeight);
                Graphics g = Graphics.FromImage(bmp);

                int imageDrawHeight = 0;

                for (int i = 0; i < images.Count; ++i)
                {
                    g.DrawImage(images[i], new Rectangle(0, imageDrawHeight, images[i].Width, images[i].Height));
                    imageDrawHeight += images[i].Height;
                }

                g.Dispose();
                bmp.Save(curFile, System.Drawing.Imaging.ImageFormat.Png);
                bmp.Dispose();

                System.IO.File.WriteAllText(curFile + ".data", numVertFrames + Environment.NewLine + images.Count + Environment.NewLine + timePerFrame);
            }
            else
            {
                for (int i = 0; i < fileList.Items.Count; ++i)
                {
                    images.Add(Image.FromFile((string)fileList.Items[i]));
                    imageWidth += images[i].Width;

                    if (images[i].Height > imageHeight)
                    {
                        imageHeight = images[i].Height;
                    }
                }

                Bitmap bmp = new Bitmap(imageWidth, imageHeight);
                Graphics g = Graphics.FromImage(bmp);

                int imageDrawWidth = 0;

                for (int i = 0; i < images.Count; ++i)
                {
                    g.DrawImage(images[i], new Rectangle(imageDrawWidth, 0, images[i].Width, images[i].Height));
                    imageDrawWidth += images[i].Width;
                }

                g.Dispose();
                bmp.Save(curFile, System.Drawing.Imaging.ImageFormat.Png);
                bmp.Dispose();

                System.IO.File.WriteAllText(curFile + ".data", images.Count + Environment.NewLine + "1" + Environment.NewLine + timePerFrame);
            }
        }

        /// <summary>
        /// Event handler to remove source files from the stitch list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void removeFileButton_Click(object sender, EventArgs e)
        {
            if(fileList.SelectedIndex >= 0)
            {
                fileList.Items.RemoveAt(fileList.SelectedIndex);
            }
        }

        /// <summary>
        /// Event handler for adding source files to the stitch list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void addFileButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFile = new OpenFileDialog();
            openFile.Filter = "PNG Image Files (*.png)|*.png";
            openFile.FileOk += new CancelEventHandler(openFile_FileOk);
            openFile.ShowDialog();
        }

        /// <summary>
        /// Logic to add source files to the stitch list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void openFile_FileOk(object sender, CancelEventArgs e)
        {
            fileList.Items.Add(((OpenFileDialog)sender).FileName);
        }
    }
}
