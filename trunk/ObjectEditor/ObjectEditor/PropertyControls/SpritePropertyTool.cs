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
    public partial class SpritePropertyTool : PropertyTools
    {
        Label sizeLabel;
        Label sizeXLabel;
        Label sizeYLabel;
        NumericUpDown sizeXNUD;
        NumericUpDown sizeYNUD;

        Label colorLabel;
        Label colorRLabel;
        Label colorGLabel;
        Label colorBLabel;
        Label colorALabel;
        NumericUpDown colorRNUD;
        NumericUpDown colorGNUD;
        NumericUpDown colorBNUD;
        NumericUpDown colorANUD;

        Label zOrderLabel;
        NumericUpDown zOrderNUD;

        Sprite curSprite;

        public SpritePropertyTool()
        {
            InitializeComponent();

            _pNameLabel.Text = "Edit Sprite:";
        }

        public SpritePropertyTool(Sprite tempSprite)
        {
            InitializeComponent();

            Point tempPoint = tempSprite.Size;

            _pNameLabel.Text = "Edit Sprite:";
            _pNameBox.Text = tempSprite.Name;

            sizeLabel = new Label();
            sizeLabel.Top = _pNameBox.Bottom + 5;
            sizeLabel.Left = _pSeparatorLine.Right + 2;
            sizeLabel.Height = 15;
            sizeLabel.Text = "Size:";
            this.Controls.Add(sizeLabel);

            sizeXLabel = new Label();
            sizeXLabel.Top = sizeLabel.Bottom + 5;
            sizeXLabel.Left = sizeLabel.Left + 5;
            sizeXLabel.Height = 15;
            sizeXLabel.Width = 20;
            sizeXLabel.Text = "X:";
            this.Controls.Add(sizeXLabel);

            sizeXNUD = new NumericUpDown();
            sizeXNUD.Top = sizeXLabel.Top;
            sizeXNUD.Left = sizeXLabel.Right;
            sizeXNUD.Width = 75;
            sizeXNUD.Increment = 1;
            sizeXNUD.Minimum = 1;
            sizeXNUD.Maximum = 10000;
            sizeXNUD.Value = tempPoint.X;
            sizeXNUD.ValueChanged += new EventHandler(sizeXNUD_ValueChanged);
            this.Controls.Add(sizeXNUD);

            sizeYLabel = new Label();
            sizeYLabel.Top = sizeLabel.Bottom + 5;
            sizeYLabel.Left = sizeXNUD.Right + 5;
            sizeYLabel.Height = 15;
            sizeYLabel.Width = 20;
            sizeYLabel.Text = "Y:";
            this.Controls.Add(sizeYLabel);

            sizeYNUD = new NumericUpDown();
            sizeYNUD.Top = sizeYLabel.Top;
            sizeYNUD.Left = sizeYLabel.Right;
            sizeYNUD.Width = 75;
            sizeYNUD.Increment = 1;
            sizeYNUD.Minimum = 1;
            sizeYNUD.Maximum = 10000;
            sizeYNUD.Value = tempPoint.Y;
            sizeYNUD.ValueChanged += new EventHandler(sizeYNUD_ValueChanged);
            this.Controls.Add(sizeYNUD);


            colorLabel = new Label();
            colorLabel.Top = sizeXNUD.Bottom + 5;
            colorLabel.Left = _pSeparatorLine.Right + 2;
            colorLabel.Height = 15;
            colorLabel.Text = "Color:";
            this.Controls.Add(colorLabel);

            colorRLabel = new Label();
            colorRLabel.Top = colorLabel.Bottom + 5;
            colorRLabel.Left = colorLabel.Left + 5;
            colorRLabel.Height = 15;
            colorRLabel.Width = 20;
            colorRLabel.Text = "R:";
            this.Controls.Add(colorRLabel);

            colorRNUD = new NumericUpDown();
            colorRNUD.Top = colorRLabel.Top;
            colorRNUD.Left = colorRLabel.Right;
            colorRNUD.Width = 75;
            colorRNUD.Increment = 1;
            colorRNUD.Minimum = 1;
            colorRNUD.Maximum = 255;
            colorRNUD.Value = tempSprite.Color.R;
            colorRNUD.ValueChanged += new EventHandler(colorRNUD_ValueChanged);
            this.Controls.Add(colorRNUD);

            colorGLabel = new Label();
            colorGLabel.Top = colorLabel.Bottom + 5;
            colorGLabel.Left = colorRNUD.Right + 5;
            colorGLabel.Height = 15;
            colorGLabel.Width = 20;
            colorGLabel.Text = "G:";
            this.Controls.Add(colorGLabel);

            colorGNUD = new NumericUpDown();
            colorGNUD.Top = colorGLabel.Top;
            colorGNUD.Left = colorGLabel.Right;
            colorGNUD.Width = 75;
            colorGNUD.Increment = 1;
            colorGNUD.Minimum = 1;
            colorGNUD.Maximum = 255;
            colorGNUD.Value = tempSprite.Color.G;
            colorGNUD.ValueChanged += new EventHandler(colorGNUD_ValueChanged);
            this.Controls.Add(colorGNUD);

            colorBLabel = new Label();
            colorBLabel.Top = colorGNUD.Bottom + 5;
            colorBLabel.Left = colorLabel.Left + 5;
            colorBLabel.Height = 15;
            colorBLabel.Width = 20;
            colorBLabel.Text = "B:";
            this.Controls.Add(colorBLabel);

            colorBNUD = new NumericUpDown();
            colorBNUD.Top = colorBLabel.Top;
            colorBNUD.Left = colorBLabel.Right;
            colorBNUD.Width = 75;
            colorBNUD.Increment = 1;
            colorBNUD.Minimum = 1;
            colorBNUD.Maximum = 255;
            colorBNUD.Value = tempSprite.Color.B;
            colorBNUD.ValueChanged += new EventHandler(colorBNUD_ValueChanged);
            this.Controls.Add(colorBNUD);

            colorALabel = new Label();
            colorALabel.Top = colorGNUD.Bottom + 5;
            colorALabel.Left = colorBNUD.Right + 5;
            colorALabel.Height = 15;
            colorALabel.Width = 20;
            colorALabel.Text = "A:";
            this.Controls.Add(colorALabel);

            colorANUD = new NumericUpDown();
            colorANUD.Top = colorALabel.Top;
            colorANUD.Left = colorALabel.Right;
            colorANUD.Width = 75;
            colorANUD.Increment = 1;
            colorANUD.Minimum = 1;
            colorANUD.Maximum = 255;
            colorANUD.Value = tempSprite.Color.A;
            colorANUD.ValueChanged += new EventHandler(colorANUD_ValueChanged);
            this.Controls.Add(colorANUD);


            zOrderLabel = new Label();
            zOrderLabel.Top = colorANUD.Bottom + 5;
            zOrderLabel.Left = _pSeparatorLine.Right + 2;
            zOrderLabel.Height = 15;
            zOrderLabel.Text = "Z Order:";
            this.Controls.Add(zOrderLabel);

            zOrderNUD = new NumericUpDown();
            zOrderNUD.Top = zOrderLabel.Bottom + 5;
            zOrderNUD.Left = _pSeparatorLine.Right + 27;
            zOrderNUD.Width = 75;
            zOrderNUD.Increment = 1;
            zOrderNUD.Minimum = 0;
            zOrderNUD.Maximum = 10000;
            zOrderNUD.Value = tempSprite.ZOrder;
            zOrderNUD.ValueChanged += new EventHandler(zOrderNUD_ValueChanged);
            this.Controls.Add(zOrderNUD);

            curSprite = tempSprite;
        }

        void zOrderNUD_ValueChanged(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }

        void colorANUD_ValueChanged(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }

        void colorBNUD_ValueChanged(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }

        void colorGNUD_ValueChanged(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }

        void colorRNUD_ValueChanged(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }

        void sizeYNUD_ValueChanged(object sender, EventArgs e)
        {
            Point tempPoint = new Point((int)sizeXNUD.Value, (int)sizeYNUD.Value);
            curSprite.Size = tempPoint;

            curSprite.UpdateData();
        }

        void sizeXNUD_ValueChanged(object sender, EventArgs e)
        {
            Point tempPoint = new Point((int)sizeXNUD.Value, (int)sizeYNUD.Value);
            curSprite.Size = tempPoint;

            curSprite.UpdateData();
        }
    }
}
