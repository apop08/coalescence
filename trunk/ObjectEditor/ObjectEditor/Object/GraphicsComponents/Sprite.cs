using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

namespace ObjectEditor
{
    public class Sprite : IComponent
    {
        public Sprite()
        {
        }

        public Sprite(string name)
        {
            _name = name;
            _pBox = new PictureBox();
            _pBox.Image = Image.FromFile(_name);
            _pBox.Width = _pBox.Image.Width;
            _pBox.Height = _pBox.Image.Height;

            _size.X = _pBox.Width;
            _size.Y = _pBox.Height;

            _color = new Vector4<int>(1, 1, 1, 1);

            _zOrder = 0;

            ((EditWindow)(Application.OpenForms[0])).GetPanel().Width = _pBox.Width + ((EditWindow)(Application.OpenForms[0])).GetOffset().X;
            ((EditWindow)(Application.OpenForms[0])).GetPanel().Height = _pBox.Height + ((EditWindow)(Application.OpenForms[0])).GetOffset().Y;
            Application.OpenForms[0].Invalidate();
        }

        private PictureBox _pBox;
        public PictureBox PBox
        {
            get { return _pBox; }
            set { _pBox = value; }
        }

        private string _name;
        public string Name
        {
            get { return _name; }
            set { _name = value; }
        }

        private Point _size;
        public Point Size
        {
            get { return _size; }
            set { _size = value; }
        }

        private Vector4<int> _color;
        public Vector4<int> Color
        {
            get { return _color; }
            set { _color = value; }
        }

        private int _zOrder;
        public int ZOrder
        {
            get { return _zOrder; }
            set { _zOrder = value; }
        }

        public void LoadData()
        {
        }

        public void SaveData()
        {
        }

        public void UpdateData()
        {
            ((EditWindow)(Application.OpenForms[0])).GetPanel().Refresh();
        }

        public void Draw(Panel tPanel)
        {
            _pBox.Width = _size.X;
            _pBox.Height = _size.Y;

            Graphics g = tPanel.CreateGraphics();
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
            g.DrawImage(_pBox.Image, ((EditWindow)(Application.OpenForms[0])).GetOffset().X, ((EditWindow)(Application.OpenForms[0])).GetOffset().Y,
                _pBox.Width + ((EditWindow)(Application.OpenForms[0])).GetOffset().X, _pBox.Height + ((EditWindow)(Application.OpenForms[0])).GetOffset().Y);
        }

        public override string ToString()
        {
            return ("Sprite" + 
                Environment.NewLine + _name + 
                Environment.NewLine + _size.X + " " + _size.Y + 
                Environment.NewLine + _color + 
                Environment.NewLine + _zOrder);
        }
    }
}
