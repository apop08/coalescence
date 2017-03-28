using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ObjectEditor
{
    public partial class RigidBodyPropertyTool : PropertyTools
    {
        Label extentLabel;
        Label extentXLabel;
        Label extentYLabel;
        NumericUpDown extentXNUD;
        NumericUpDown extentYNUD;

        RigidBody curRB;

        public RigidBodyPropertyTool()
        {
            InitializeComponent();

            _pNameLabel.Text = "Edit Rigid Body:";
        }

        public RigidBodyPropertyTool(RigidBody tempRB)
        {
            InitializeComponent();

            PointF tempPoint = tempRB.Extents;

            _pNameLabel.Text = "Edit Rigid Body:";
            _pNameBox.Text = tempRB.Name;

            extentLabel = new Label();
            extentLabel.Top = _pNameBox.Bottom + 5;
            extentLabel.Left = _pSeparatorLine.Right + 2;
            extentLabel.Height = 15;
            extentLabel.Text = "Extent:";
            this.Controls.Add(extentLabel);

            extentXLabel = new Label();
            extentXLabel.Top = extentLabel.Bottom + 5;
            extentXLabel.Left = extentLabel.Left + 5;
            extentXLabel.Height = 15;
            extentXLabel.Width = 20;
            extentXLabel.Text = "X:";
            this.Controls.Add(extentXLabel);

            extentXNUD = new NumericUpDown();
            extentXNUD.Top = extentXLabel.Top;
            extentXNUD.Left = extentXLabel.Right;
            extentXNUD.Width = 75;
            extentXNUD.Increment = 0.1M;
            extentXNUD.DecimalPlaces = 2;
            extentXNUD.Minimum = 1;
            extentXNUD.Maximum = 10000;
            extentXNUD.Value = (decimal)tempPoint.X;
            extentXNUD.ValueChanged += new EventHandler(extentXNUD_ValueChanged);
            this.Controls.Add(extentXNUD);

            extentYLabel = new Label();
            extentYLabel.Top = extentLabel.Bottom + 5;
            extentYLabel.Left = extentXNUD.Right + 5;
            extentYLabel.Height = 15;
            extentYLabel.Width = 20;
            extentYLabel.Text = "Y:";
            this.Controls.Add(extentYLabel);

            extentYNUD = new NumericUpDown();
            extentYNUD.Top = extentYLabel.Top;
            extentYNUD.Left = extentYLabel.Right;
            extentYNUD.Width = 75;
            extentYNUD.Increment = 0.1M;
            extentYNUD.DecimalPlaces = 2;
            extentYNUD.Minimum = 1;
            extentYNUD.Maximum = 10000;
            extentYNUD.Value = (decimal)tempPoint.Y;
            extentYNUD.ValueChanged += new EventHandler(extentYNUD_ValueChanged);
            this.Controls.Add(extentYNUD);

            curRB = tempRB;
        }

        void extentYNUD_ValueChanged(object sender, EventArgs e)
        {
            PointF tempPoint = new PointF((float)extentXNUD.Value, (float)extentYNUD.Value);
            curRB.Extents = tempPoint;

            curRB.UpdateData();
        }

        void extentXNUD_ValueChanged(object sender, EventArgs e)
        {
            PointF tempPoint = new PointF((float)extentXNUD.Value, (float)extentYNUD.Value);
            curRB.Extents = tempPoint;

            curRB.UpdateData();
        }
    }
}
