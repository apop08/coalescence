using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

namespace ObjectEditor
{
    public class RigidBody : IComponent
    {
        public RigidBody()
        {
        }

        public RigidBody(string name)
        {
            _name = name;

            _extents.X = ((EditWindow)(Application.OpenForms[0])).GetPanel().Width / 2;
            _extents.Y = ((EditWindow)(Application.OpenForms[0])).GetPanel().Height / 2;

            ((EditWindow)(Application.OpenForms[0])).SetOffset();

            _flags = 0;
        }

        private string _name;
        public string Name
        {
            get { return _name; }
            set { _name = value; }
        }

        private PointF _extents;
        public PointF Extents
        {
            get { return _extents; }
            set { _extents = value; }
        }

        private UInt32 _flags;
        public UInt32 Flags
        {
            get { return _flags; }
            set { _flags = value; }
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
            Graphics g = tPanel.CreateGraphics();
            g.DrawRectangle(new Pen(Color.Tomato, 1.0f),
                (((EditWindow)(Application.OpenForms[0])).GetPanel().Width - ((EditWindow)(Application.OpenForms[0])).GetOffset().X) / 2 - _extents.X,
                (((EditWindow)(Application.OpenForms[0])).GetPanel().Height - ((EditWindow)(Application.OpenForms[0])).GetOffset().Y) / 2 - _extents.Y,
                _extents.X * 2,
                _extents.Y * 2);
        }

        public override string ToString()
        {
            return ("PhysBody" +
                Environment.NewLine + _extents.X + " " + _extents.Y +
                Environment.NewLine + _flags);
        }
    }
}